;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: estcmd.l 740 2006-10-22 17:32:24Z torihat $
;
; kamail3/estcmd.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/estcmd")

(in-package "kamail3")

(defvar *he-mode-hook* nil)

(defvar *buffer-he-parse* (concat *buffer-prefix* "HyperEstraier Parse*"))
(defvar *buffer-he-temp*  (concat *buffer-prefix* "HyperEstraier Temp*"))

(defvar *he-estcmd* "estcmd")
(defvar *he-casket*
  (merge-pathnames "casket" *base-directory*))
(defvar *he-cmd-gather-option* nil)

(defvar *he-attr-index-list* '(
			       ("x-kamail3-folder" . "str")
			       ("x-kamail3-tag"    . "str")
			       ("x-kamail3-status" . "str")
			       ("date"             . "seq")
			       ))

(defvar *he-execute-batch-file*
  (merge-pathnames "he-temp.bat" *data-directory*))

(defvar *he-gather-list-file*
  (merge-pathnames "he-gather.txt" *data-directory*))

(defvar *he-execute-batch-log*
  (merge-pathnames "he-log.txt" *data-directory*))

(defvar *he-dump-directory*
  (merge-pathnames "hedump" *base-directory*))

(defvar *he-last-result* nil)

(defstruct heresult
  key
  number
  time
  docnum
  wordnum
  documents)

(defun he-parse-header ()
  (let (boundary number docnum wordnum time)
    ; 1行目はboundary
    (setq boundary (buffer-substring (progn (goto-bol) (point))
				     (progn (goto-eol) (point))))
    (or (forward-line 1) (return-from he-parse-header))
    ; ヘッダー
    (while (looking-at "\\([^\t\n]+\\)\t\\([^\n]*\\)$")
      (let ((key (match-string 1))
	    (val (match-string 2)))
	(cond ((equal key "HIT")
	       (setq number (parse-integer val)))
	      ((equal key "DOCNUM")
	       (setq docnum (parse-integer val)))
	      ((equal key "WORDNUM")
	       (setq wordnum (parse-integer val)))
	      ((equal key "TIME")
	       (setq time (read-from-string val)))
	      ))
      (or (forward-line) (return)))
    (values boundary number docnum wordnum time)
    ))

(defun he-parse-multipart ()
  (let (boundary
	key
	number
	time
	docnum
	wordnum
	documents
	(i 0))
    (map-utf-8-region (point-min) (point-max))
    (goto-char (point-min))
    (multiple-value-setq (boundary number docnum wordnum time)
      (he-parse-header))
    ; 各パート
    (when (scan-buffer boundary :tail t)
      (loop
	(let (document path beg)
	  (and (looking-for ":END") (return))
	  (or (forward-line) (return))
	  (while (looking-at "\\([^=\n]+\\)=\\([^\n]*\\)$")
	    (push (cons (match-string 1) (match-string 2)) document)
	    (or (forward-line) (return)))
	  (while (not (looking-at "^$"))
	    (or (forward-line) (return)))
	  (setq beg (point))
	  (or (scan-buffer boundary :tail t) (return))
	  (push (list "@snipet" (string-trim " \t\r\n"
					     (buffer-substring beg
							       (- (point) (length boundary)))))
		document)
	  (unless (and (setq path (junk::mail-get-header-value "_lpath" document))
		       (string-match ".kamail-cache$" path))
	    (push (cons (incf i) (nreverse document)) documents)))))
    (make-heresult
     :key key
     :number number
     :time time
     :docnum docnum
     :wordnum wordnum
     :documents (nreverse documents))))

(defun he-search-do (query attrs order max offset)
  (let ((buf *buffer-he-parse*)
	(ed::*expected-fileio-encoding* *encoding-sjis*)
	cmd
	result)
    (declare (special ed::*expected-fileio-encoding*))
    (setq cmd (he-cmd-search query attrs order max offset))
    (when *debug-mode*
      (copy-to-clipboard cmd))
    (save-excursion
      (set-buffer (get-buffer-create buf))
      (execute-shell-command cmd nil buf)
      (setq result (he-parse-multipart))
      (when *debug-mode*
	(he-write-log)))
    (delete-buffer buf)
;    (msgbox "~S~%~S~%~S~%~S~%~S~%~S~%~S"
;	    query
;	    attrs
;	    order
;	    max
;	    offset
;	    cmd
;	    result)
    (setq *he-last-result* result)))

(defun he-cmd-search (query &optional attrs order max offset)
  (let ((attr (with-output-to-string (s)
		(dolist (a attrs)
		  (when a
		    (format s " -attr \"~A\"" a)))))) ; そのうちUTF-8にすることを考える
    (format nil "~A search -ic CP932~@[ -pidx \"~A\"~] -vs -hs -max ~D -sk ~D~@[~A~]~@[ -ord \"~A\"~] \"~A\"~@[ \"~A\"~]"
	    *he-estcmd*
	    (and *use-pseudo-index* *draft-directory* )
	    (or max -1)
	    (or offset 0)
	    attr
	    order
	    *he-casket*
	    query)
    ))

(defun he-cmd-search-file (query &optional attrs order)
  (let ((attr (with-output-to-string (s)
		(dolist (a attrs)
		  (when a
		    (format s " -attr \"~A\"" a)))))) ; そのうちUTF-8にすることを考える
    (format nil "~A search -ic CP932~@[ -pidx \"~A\"~] -vu -max -1 ~@[~A~]~@[ -ord \"~A\"~] \"~A\"~@[ \"~A\"~]"
	    *he-estcmd*
	    (and *use-pseudo-index* *draft-directory* )
	    attr
	    order
	    *he-casket*
	    query)
    ))

(defun he-cmd-edit (file key &optional value)
  (format nil "~A edit -pc CP932 \"~A\" \"~A\" ~A \"~@[~A~]\""
	  *he-estcmd*
	  *he-casket*
	  file
	  key
	  value))

(defun he-cmd-gather-draft (dir)
  (when (string-match "/$" dir)
    (setq dir (substring dir 0 (1- (length dir))))) ; 最後の "/" を消す
  (format nil "~A gather -pc CP932 -fe -cm \"~A\" \"~A\""
	  *he-estcmd*
	  *he-casket*
	  dir))

(defun he-cmd-gather (dir &optional attrs)
  (when (string-match "/$" dir)
    (setq dir (substring dir 0 (1- (length dir))))) ; 最後の "/" を消す
  (format nil "~A gather~@[ ~A~]~{ -px ~A~} -pc CP932 -il ja -fm -cm -sd \"~A\" \"~A\""
	  *he-estcmd*
	  *he-cmd-gather-option*
	  attrs
	  *he-casket*
	  dir))

(defun he-cmd-restore (dir)
  (when (string-match "/$" dir)
    (setq dir (substring dir 0 (1- (length dir))))) ; 最後の "/" を消す
  (format nil "~A gather~@[ ~A~] -pc CP932 -fe \"~A\" \"~A\""
	  *he-estcmd*
	  *he-cmd-gather-option*
	  *he-casket*
	  dir))

(defun he-path-to-uri (path)
  (concat "file:///"
	  (substitute-string path ":" "|")))

(defun he-uri-to-path (uri)
  (substitute-string (si:www-url-decode uri)
		     "^file:///\\([a-zA-Z]\\)|"
		     "\\1:"))

(defun he-cmd-purge (&optional dir)
  (when (and dir
	     (file-directory-p dir)
	     (not (string-match "/$" dir)))
    (setq dir (concat dir "/")))
  (format nil "~A purge -pc CP932 \"~A\"~@[ \"~A\"~]"
	  *he-estcmd*
	  *he-casket*
	  (if dir (he-path-to-uri dir) nil)))

(defun he-cmd-purge-force (dir)
  (unless (string-match "/$" dir)
    (setq dir (concat dir "/")))
  (format nil "~A purge -cl -fc -pc CP932 \"~A\" \"~A\""
	  *he-estcmd*
	  *he-casket*
	  dir))

(defun he-cmd-out (file)
  (format nil "~A out -pc CP932 \"~A\" \"~A\""
	  *he-estcmd*
	  *he-casket*
	  file))

(defun he-cmd-optimize ()
  (format nil "~A optimize \"~A\""
	  *he-estcmd*
	  *he-casket*))

(defun he-cmd-inform ()
  (format nil "~A inform \"~A\""
	  *he-estcmd*
	  *he-casket*))

(defun he-cmd-version ()
  (format nil "~A version \"~A\""
	  *he-estcmd*
	  *he-casket*))

(defun he-cmd-dump ()
  (format nil "~A search -max -1 -dd \"~A\" [UVSET]"
	  *he-estcmd*
	  *he-casket*))

(defun he-cmd-draft (file &optional dest-file)
  (format nil "~A draft -fm -il ja \"~A\"~@[ > \"~A\"~]"
	  *he-estcmd*
	  file
	  dest-file))

(defun he-cmd-create (&optional attrs)
  (setq attrs (or attrs *he-attr-index-list*))
  (let ((attr (with-output-to-string (s)
		(dolist (a attrs)
		  (when a
		    (format s " -attr ~A ~A" (car a) (cdr a)))))))
    (format nil "~A create ~@[~A~] \"~A\""
	    *he-estcmd*
	    attr
	    *he-casket*)))

(defun he-cmd-list ()
  (format nil "~A list -lp \"~A\""
	  *he-estcmd*
	  *he-casket*))

(defun he-write-text (file text-list)
  (with-open-file (f file
		     :direction :output
		     :if-exists :overwrite
		     :if-does-not-exist :create)
    (mapc #'(lambda (x)
	      (if (stringp x)
		  (format f "~A~%" x)
		(format f "~A~{\t~A~}~%" (car x) (cdr x))))
	  text-list)))

(defun he-execute (cmd)
  (let (res)
    (when *debug-mode*
      (copy-to-clipboard cmd)
      (kamail3-message "~A ..." cmd))
    (setq res (execute-shell-command cmd))
    (when *debug-mode*
      (kamail3-message "~A: ~S" cmd res))
    (equal res 0)))

(defun he-execute-batch (cmd-list &optional (background-p nil) &key environ directory)
  (let (bat
	res
	(buf *buffer-he-temp*)
	(file *he-execute-batch-file*)
	(file-escaped (format nil "\"~A\"" *he-execute-batch-file*)))
    (when (find-buffer buf)
      (let ((process (buffer-process buf)))
	(when (and process
		   (processp process)
		   (eq (process-status process) :run))
	  (kamail3-message "前のバッチがまだ終わっていません。")
	  (return-from he-execute-batch))))
    (he-write-text file cmd-list)
    (save-excursion
      (set-buffer (get-buffer-create buf))
      (erase-buffer (selected-buffer))
      (if background-p
	  (progn
	    (execute-subprocess file-escaped nil buf environ directory)
	    t)
	(progn
	  (when *debug-mode*
	    (kamail3-message "~A ..." file))
	  (setq res (execute-shell-command file-escaped nil buf environ directory))
	  (when *debug-mode*
	    (he-write-log)
	    (kamail3-message "~A: ~S" file res))
	  (equal res 0))))
    ))

(defun he-write-log ()
  (write-region (point-min)
		(point-max)
		*he-execute-batch-log*
		t))

(defun he-optimize ()
  (interactive)
  (kamail3-message "optimizing index: ~A ... " *he-casket*)
  (he-execute-batch (list (he-cmd-optimize)))
  (kamail3-message "optimizing index: ~A ... done." *he-casket*))

(defun he-print-result (results)
  (let (errors
	(num-all (length results))
	(num-suc 0))
    (dolist (r results)
      (let ((cmd (car r))
	    (res (cdr r)))
	(if res
	    (incf num-suc)
	  (push cmd errors))))
    (when errors
      (kamail3-msgbox "Error:~%~%~{~A~%~}" (reverse errors)))
    (kamail3-message "Completed: ~D/~D" num-suc num-all)))

(defun he-gather-files-cmd (files &optional attrs)
  (he-write-text *he-gather-list-file* files)
  (he-cmd-gather *he-gather-list-file* attrs))

(defun he-gather-files (files &optional attrs)
  (kamail3-message "gathering ...")
  (he-execute-batch (list (he-gather-files-cmd files attrs)))
  (kamail3-message "gathering ... done."))

(defun he-gather-all-cmd ()
  (he-cmd-gather *mail-directory*))

(defun he-purge-all-cmd ()
  (he-cmd-purge))

(defun he-purge-files (result)
  (let ((cnt 0)
	dirs
	cmd-list)
    (dolist (document (heresult-documents result))
      (let* ((num (car document))
	     (header (cdr document))
	     (file (summary-message-file header))
	     (number (fetch-file-number file))
	     (draft (fetch-draft-file number))
	     (dir (directory-namestring file)))
	(when (file-exist-p file)
	  (delete-file file))
	(incf cnt)
	(when (file-exist-p draft)
	  (delete-file draft))
	(unless (member dir dirs :test #'equal)
	  (push (he-cmd-purge dir) cmd-list)
	  (push dir dirs))))
    (when cmd-list
      (he-execute-batch (nreverse cmd-list)))
    cnt))

(defun he-get-cmd-result (cmd)
  (let ((buf *buffer-he-temp*)
	(ed::*expected-fileio-encoding* *encoding-sjis*)
	result)
    (declare (special ed::*expected-fileio-encoding*))
    (save-excursion
      (set-buffer (get-buffer-create buf))
      (erase-buffer (selected-buffer))
      (execute-shell-command cmd nil buf)
      (setq result (buffer-substring (point-min) (point-max))))
    (delete-buffer buf)
    result))

(defun he-inform-get ()
  (let ((cmd (he-cmd-inform)))
    (he-get-cmd-result cmd)))

(defun he-version-get ()
  (let ((cmd (he-cmd-version)))
    (he-get-cmd-result cmd)))

(defun he-purge-all (&optional background-p)
  (interactive "p")
  (let ((cmd-list (list (he-purge-all-cmd))))
    (kamail3-message "purging index ... ")
    (he-execute-batch cmd-list background-p)
    (kamail3-message "purging index ... done.")))

(defun he-gather-all (&optional background-p)
  (interactive "p")
  (let ((cmd-list (list (he-gather-all-cmd))))
    (kamail3-message "gathering index: ~A ... " *he-casket*)
    (he-execute-batch cmd-list background-p)
    (kamail3-message "gathering index: ~A ... done." *he-casket*)))

(defun he-regather-all ()
  (interactive)
  (he-purge-all)
  (he-gather-unindexed-files))

(defun he-inform ()
  (interactive)
  (let ((version (he-version-get))
	(inform  (he-inform-get)))
    (message-box (format nil "~A~%~A~%~A"
			 version
			 *he-casket*
			 inform)
		 *he-casket*
		 :information)))

(defun he-dump (&optional background-p)
  (interactive "p")
  (let ((cmd (he-cmd-dump)))
    (create-directory-if *he-dump-directory*)
    (kamail3-message "deleting files ...")
    (dolist (file (directory *he-dump-directory*
			     :file-only t
			     :absolute t))
      (delete-file file)
      (do-events))
    (kamail3-message "creating dump files ...")
    (he-execute-batch (list cmd)
		      background-p
		      :directory *he-dump-directory*)
    (kamail3-message "creating dump files ... done.")))

(defun he-create (&optional background-p)
  (interactive "p")
  (when (file-exist-p *he-casket*)
    (return-from he-create))
  (let ((cmd (he-cmd-create)))
    (kamail3-message "creating index: ~A ... " *he-casket*)
    (he-execute-batch (list cmd) background-p)
    (kamail3-message "creating index: ~A ... done." *he-casket*)))

(defun he-restore (&optional background-p)
  (interactive "p")
  (let ((cmd (he-cmd-restore *he-dump-directory*)))
    (when (file-exist-p *he-casket*)
      (let ((backupdir (format nil "~A-~A"
			       *he-casket*
			       (format-date-string "%Y%m%d%H%M%S"))))
	(kamail3-msgbox "既存のcasketを ~A に変更します。" backupdir)
	(unless (rename-file *he-casket* backupdir)
	  (kamail3-error "名称変更失敗: \n~A\n=>~A"
			 *he-casket*
			 backupdir))))
    (kamail3-message "restoring dump files ...")
    (he-execute-batch (list (he-cmd-create)
			    (he-cmd-restore *he-dump-directory*))
		      background-p)
    (kamail3-message "restoring dump files ... done.")))

(defun he-gather-unindexed-files ()
  (interactive)
  (let (file-list-index
	file-list-hash
	files
	(folder (attr-folder-string *folder-inbox*)))
    (long-operation
      (kamail3-message "listup files in index ...")
      (setq file-list-index (he-list-do))
      (setq file-list-hash (make-hash-table :test #'equalp
					    :size (length file-list-index)))
      (dolist (f file-list-index)
	(setf (gethash (map-backslash-to-slash (cdr f)) file-list-hash) 1)
	(do-events))
      (kamail3-message "searching files ...")
      (let ((dirs (directory *mail-directory*
			     :absolute t
			     :recursive nil
			     :directory-only t)))
	(dolist (dir (sort dirs #'string-lessp))
	  (kamail3-message "parsing directory: ~A ..." dir)
	  (dolist (file (directory dir
				   :absolute t
				   :recursive t
				   :file-only t))
	    (unless (gethash file file-list-hash)
	      (push (list file folder) files))
	    (do-events))))
      (when *debug-mode*
	(kamail3-msgbox "files: ~D" (length files)))
      (kamail3-message "gathering files ...")
      (he-gather-files files (list *header-folder*))
      (kamail3-message "done."))
    ))

(defun he-repair-pindex ()
  (interactive)
  (long-operation
    (kamail3-message "purging pseudo index ...")
    (he-execute (he-cmd-purge *draft-directory*)))
  (he-gather-unindexed-files))

(defun he-list-do ()
  (let ((buf *buffer-he-parse*)
	(ed::*expected-fileio-encoding* *encoding-sjis*)
	cmd
	result)
    (declare (special ed::*expected-fileio-encoding*))
    (setq cmd (he-cmd-list))
    (save-excursion
      (set-buffer (get-buffer-create buf))
      (execute-shell-command cmd nil buf)
      (goto-char (point-min))
      (while (not (eolp))
	(let* ((line (split-string (buffer-substring (point)
						     (progn
						       (goto-eol)
						       (point)))
				   "\t"))
	       (num (car line))
	       (path (nth 2 line)))
	  (when (and num path)
	    (push (cons num path) result)))
	(or (forward-line) (return))))
    (delete-buffer buf)
    (nreverse result)))

(defun he-search-file-do (cmd)
  (let ((buf *buffer-he-parse*)
	(ed::*expected-fileio-encoding* *encoding-sjis*)
	result)
    (declare (special ed::*expected-fileio-encoding*))
    (save-excursion
      (set-buffer (get-buffer-create buf))
      (execute-shell-command cmd nil buf)
      (goto-char (point-min))
      (setq result (he-parse-filelist)))
    (delete-buffer buf)
    (nreverse result)))

(defun he-attr-index-filename (key)
  (concat "__str_"
	  (string-upcase
	   (with-output-to-string (s)
	     (dotimes (i (length key))
	       (format s "~x" (char-code (char key i))))))))

(defun he-attr-index-file (key)
  (merge-pathnames
   (he-attr-index-filename key)
   *he-casket*))

(defun he-repair-nofolder-files ()
  (interactive)
  (let ((attr-index-file (he-attr-index-file *header-folder*))
	(folder (completing-read "Folder: "
				 *folder-attr-list*
				 :default *folder-inbox*
				 :must-match t)))
    (when (file-exist-p attr-index-file)
      (let (result)
	(kamail3-message "deleting index: ~A ..." attr-index-file)
	(setq result (delete-file attr-index-file))
	(kamail3-message "deleting index: ~A ... ~A."
			 attr-index-file
			 (if result "done" "failed"))
	(unless result
	  (return-from he-repair-nofolder-files))))
    (let ((cmd (he-cmd-search-file "" (list (format nil "~A !STRINC ~A" *header-folder* *attr-folder-prefix-string*))))
	  result)
      (kamail3-message "finding nofolder files ...")
      ;(message-box cmd)
      ;(copy-to-clipboard cmd)
      (when (setq result (he-search-file-do cmd))
	(when folder
	  (kamail3-message "updating folder ...")
	  (attr-folder-update-batch (mapcar #'(lambda (x)
						(cons (cdr x) folder))
					    result)))))
    (let ((folder-attr (assoc *header-folder* *he-attr-index-list* :test #'equal)))
      (when folder-attr
	(kamail3-message "creating folder index ...")
	(let ((cmd (he-cmd-create (list folder-attr))))
	  (he-execute-batch (list cmd))
	  )))
    (kamail3-message "done.")
    ))

(defun he-parse-filelist ()
  (let (boundary
	key
	number
	time
	docnum
	wordnum
	files
	(i 0))
    (map-utf-8-region (point-min) (point-max))
    (goto-char (point-min))
    (multiple-value-setq (boundary number docnum wordnum time)
      (he-parse-header))
    (when (scan-buffer boundary :tail t)
      (loop
	(or (forward-line) (return))
	(or (looking-at "\\([0-9]+\\)\t\\([^\n]+\\)$") (return))
	(let ((num (match-string 1))
	      (file (he-uri-to-path (match-string 2))))
	  (push (cons num file) files)
	  (do-events)
	  )))
    (values (nreverse files) number docnum wordnum time)
    ))
