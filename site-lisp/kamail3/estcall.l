;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: estcall.l 722 2006-08-30 16:45:53Z torihat $
;
; kamail3/estcall.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(require "kamail3/estcmd")

(provide "kamail3/estcall")

(in-package "kamail3")

(defvar *buffer-estcall-parse* (concat *buffer-prefix* "EstCall Parse*"))
(defvar *buffer-estcall-temp*  (concat *buffer-prefix* "EstCall Temp*"))

(defvar *estcall-cmd* "estcall")
(defvar *estcall-casket-basedir*
  (merge-pathnames "p2p" *base-directory*))

(defvar *estmaster-cmd* "estmaster")

(defvar *estcall-url* "http://localhost:1978/node/kamailv3")
(defvar *estcall-user* "admin")
(defvar *estcall-pass* "admin")
(defvar *estcall-proxy-host* nil)
(defvar *estcall-proxy-port* nil)

(defvar *estcall-casket*
  (merge-pathnames "_node/kamailv3" *estcall-casket-basedir*))

(setq *he-execute-batch-file*
  (merge-pathnames "estcall-temp.bat" *data-directory*))

(setq *he-gather-list-file*
  (merge-pathnames "estcall-gather.txt" *data-directory*))

(setq *he-execute-batch-log*
  (merge-pathnames "estcall-log.txt" *data-directory*))

;(setq *he-dump-directory*
;  (merge-pathnames "estcalldump" *base-directory*))

(defun estcall-auth-option ()
  (format nil "~@[-proxy ~A ~]-auth ~A ~A"
	  (and *estcall-proxy-host*
	       *estcall-proxy-port*
	       (format nil "~A ~A"
		       *estcall-proxy-host*
		       *estcall-proxy-port*))
	  *estcall-user*
	  *estcall-pass*))

(defun he-cmd-search (query &optional attrs order max offset)
  (let ((attr (with-output-to-string (s)
		(dolist (a attrs)
		  (when a
		    (format s " -attr \"~A\"" a)))))) ; ÇªÇÃÇ§ÇøUTF-8Ç…Ç∑ÇÈÇ±Ç∆ÇçlÇ¶ÇÈ
    (format nil "~A search ~A -max ~D -sk ~D~@[~A~]~@[ -ord \"~A\"~] \"~A\"~@[ \"~A\"~]"
	    *estcall-cmd*
	    (estcall-auth-option)
	    (or max -1)
	    (or offset 0)
	    attr
	    order
	    *estcall-url*
	    query)
    ))

(defun he-cmd-edit (file key &optional value)
  (format nil "~A edit ~A \"~A\" \"~A\" ~A \"~@[~A~]\""
	  *estcall-cmd*
	  (estcall-auth-option)
	  *estcall-url*
	  file
	  key
	  value))

(defun he-cmd-put (draft)
  (format nil "~A put ~A \"~A\" \"~A\""
	  *estcall-cmd*
	  (estcall-auth-option)
	  *estcall-url*
	  draft))

(defun he-cmd-gather (file &optional attrs)
  (format nil "~A | ~A put ~A \"~A\"~@[~A~]"
	  (he-cmd-draft file)
	  *estcall-cmd*
	  (estcall-auth-option)
	  *estcall-url*
	  (when attrs
	    (with-output-to-string (s)
	      (dolist (attr attrs)
		(let ((key (car attr))
		      (value (cdr attr)))
		  (format s "~%~A" (he-cmd-edit file key value))))))))

(defun he-cmd-restore (dir)
  (kamail3-error "not supported."))

(defun he-cmd-purge (file)
  (he-cmd-out file))

(defun he-cmd-purge-force (file)
  (he-cmd-out file))

(defun he-cmd-out (uri)
  (format nil "~A out ~A ~A \"~A\""
	  *estcall-cmd*
	  (estcall-auth-option)
	  *estcall-url*
	  uri))

(defun he-cmd-optimize ()
  (kamail3-error "not supported."))

(defun he-cmd-inform ()
  (format nil "~A inform ~A \"~A\""
	  *estcall-cmd*
	  (estcall-auth-option)
	  *estcall-url*))

(defun he-cmd-version ()
  (kamail3-error "not supported."))

(defun he-cmd-dump ()
  (kamail3-error "not supported."))

(defun he-cmd-create (&optional attrs)
  (format nil "~A init \"~A\""
	  *estmaster-cmd*
	  *estcall-casket-basedir*))

(defun he-optimize ()
  (interactive)
  (kamail3-error "not supported."))

(defun he-gather-files-cmd (files &optional attrs)
  (with-output-to-string (s)
    (dolist (file files)
      (if (stringp file)
	  (format s "~A~%" (he-cmd-gather file))
	(let ((f (car file))
	      (values (cdr file))
	      attr-values)
	  (dotimes (i (length attrs))
	    (push (cons (nth i attrs) (nth i values)) attr-values))
	  (format s "~A~%" (he-cmd-gather f attr-values)))))))

(defun he-gather-files (files &optional attrs)
  (kamail3-message "gathering ...")
  (he-execute-batch (list (he-gather-files-cmd files attrs)))
  (kamail3-message "gathering ... done."))

(defun he-gather-all-cmd ()
  (kamail3-error "not supported."))

(defun he-purge-all-cmd ()
  (kamail3-error "not supported."))

(defun he-purge-files (result)
  (let ((cnt 0)
	cmd-list)
    (dolist (document (heresult-documents result))
      (let* ((num (car document))
	     (header (cdr document))
	     (file (summary-message-file header))
	     (uri (junk::mail-get-header-value "@uri" header)))
	;(when (file-exist-p file)
	;  (delete-file file))
	(incf cnt)
	(push (he-cmd-out uri) cmd-list))
      (do-events))
    (when cmd-list
      (he-execute-batch (nreverse cmd-list)))
    cnt))

(defun he-get-cmd-result (cmd)
  (let ((buf *buffer-estcall-temp*)
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

(defun he-regather-all (&optional background-p)
  (interactive "p")
  (kamail3-error "not supported."))

(defun he-inform ()
  (interactive)
  (kamail3-error "not supported."))

;(defun he-dump (&optional background-p)
;  (interactive "p")
;  (kamail3-error "not supported."))

(defun he-create (&optional background-p)
  (interactive "p")
  (when (file-exist-p *estcall-casket-basedir*)
    (return-from he-create))
  (let ((cmd (he-cmd-create))
	(conf (merge-pathnames "_conf" *estcall-casket-basedir*)))
    (kamail3-message "creating index: ~A ... " *he-casket*)
    ;(he-execute-batch (list cmd) background-p)
    (he-execute-batch (list cmd))
    (unless (file-exist-p conf)
      (kamail3-error "could not find config file"))
    (let ((buf "*temp*"))
      (save-excursion
	(set-buffer (get-buffer-create buf))
	(erase-buffer (selected-buffer))
	(insert-file-contents conf)
	(when (scan-buffer "^attrindex:" :regexp t :tail nil)
	  (dolist (attr *he-attr-index-list*)
	    (insert (format nil "attrindex: ~A{{!}}~A~%"
			    (car attr)
			    (cdr attr))))
	  (write-region (point-min) (point-max) conf)))
      (delete-buffer buf))
    (kamail3-message "creating index: ~A ... done." *he-casket*)))

(defun he-restore (&optional background-p)
  (interactive "p")
  (let (files)
    (kamail3-message "listing up ~A ..." *he-dump-directory*)
    (setq files (directory *he-dump-directory*
			   :wild "*.est"
			   :file-only t
			   :absolute t))
    (kamail3-message "putting dump files ...")
    (he-execute-batch (mapcar #'(lambda (x)
				  (he-cmd-put x))
			      files)
		      background-p)
    (kamail3-message "putting dump files ... ~:[done~;started~]." background-p)
    ))
