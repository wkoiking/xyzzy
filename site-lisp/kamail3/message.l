;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: message.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/message.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/message")

(in-package "kamail3")

; (defvar *message-attachment-directory*
;   (map-backslash-to-slash (get-special-folder-location :personal)))

(defvar *message-attachment-directory*
  (map-backslash-to-slash (get-special-folder-location :desktop)))

(defvar *message-autosave-base-directory*
  (merge-pathnames "attachments" *base-directory*))

(defvar *message-mode-hook* nil)

(defvar *message-file-current* nil)
(defvar-local *message-structure* nil)

(defvar *message-map* nil)
(setq *message-map* (make-sparse-keymap))
(define-key *message-map* #\SPC 'message-scroll-forward)
(define-key *message-map* #\C-h 'message-scroll-backward)
(define-key *message-map* #\s   'message-save-attachment-select)
(define-key *message-map* #\x   'message-save-execute-attachment-select)
(define-key *message-map* #\c   'message-create)
(define-key *message-map* #\r   'message-reply)
(define-key *message-map* #\R   'message-reply-all)
(define-key *message-map* #\f   'message-forward)
(define-key *message-map* #\F   'message-forward-as-attachment)
(define-key *message-map* #\q   'message-close)

(defun message-buffer-set ()
  (set-buffer (get-buffer-create *buffer-message*)))

(defun message-window-get ()
  (and (find-buffer *buffer-message*)
       (get-buffer-window *buffer-message*)))

(defun message-window-set ()
  (let ((win (message-window-get)))
    (if win
	(set-window win)
      (progn
	(summary-window-set)
	(split-window *summary-window-height*)
	(other-window 1)
	(message-buffer-set)))))

(defun message-mode ()
  (interactive)
  (kill-all-local-variables)
  (setq buffer-read-only t)
  (setq need-not-save t)
  (setq kept-undo-information nil)
  (setq auto-save nil)
  (toggle-ime nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-line-number* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-newline* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-eof* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-cursor-line* t)
  (set-buffer-fold-width t)
  (use-keymap *message-map*)
  (title-mode-line)
  (run-hooks '*message-mode-hook*))

(defun source-buffer-set ()
  (set-buffer (get-buffer-create *buffer-source*))
  (setq need-not-save t))

(defun message-show (file)
  (message-window-set)
  (setq *message-file-current* file)
  (let (structure)
    (source-buffer-set)
    (erase-buffer (selected-buffer))
    (insert-file-noconv file)
    (setq structure (junk::mail-parse))
    (message-buffer-set)
    (message-mode)
    (setq *message-structure* structure)
    (junk::modify-read-only-buffer
      (erase-buffer (selected-buffer))
      (message-print structure)
      (goto-char (point-min)))
    t))

(defun message-close ()
  (interactive)
  (summary-window-set))

(defun message-print (structure)
  (let ((header (junk::mailstructure-headers structure)))
    (message-print-header header)
    (message-print-body structure)))

(defun message-header-attribute (key)
  (cdr (assoc key *attribute-message-header* :test #'equal)))

(defun message-print-header (header)
  (dolist (field *message-header-field-list*)
    (let* ((attrs (message-header-attribute field))
	   (attr-field (car (or attrs *attribute-message-header-default*)))
	   (attr-value (cdr (or attrs *attribute-message-header-default*)))
	   (value (junk::mail-get-header-value field header)))
      (when value
	(when (listp value)
	  (setq value (car value)))
	(apply #'set-text-attribute
	       (point)
	       (progn
		 (insert (format nil "~A: " (string-capitalize field)))
		 (point))
	       field
	       attr-field)
	(apply #'set-text-attribute
	       (point)
	       (progn
		 (if (listp value)
		     (insert (junk::string-join value ", "))
		   (insert value))
		 (point))
	       field
	       attr-value)
	(insert "\n"))))
  (insert "\n"))

(defun message-print-part (number header)
  (let* ((content-type (junk::mail-get-header-content "content-type" header))
	 (mime-type (junk::mail-get-content-mime-type content-type))
	 (charset (junk::mail-get-content-charset content-type))
	 (encoding (junk::mail-get-transfer-encoding header))
	 (filename (junk::mail-get-filename header)))
    (apply #'set-text-attribute
	   (point)
	   (progn
	     (insert (format nil "[~@[~A:~]~@[ ~A~]~@[ <~A>~]~@[ [~A]~]~@[ (~A)~]]~%"
			     number
			     filename
			     mime-type
			     charset
			     encoding))
	     (point))
	   number
	   *attribute-message-part*)))

(defun header-to-mime-type (header) ;; 自作しました
      (let* ((content-type        (junk::mail-get-header-content "content-type" header))
             (mime-type           (junk::mail-get-content-mime-type content-type))
             (content-disposition (junk::mail-get-header-content "content-disposition" header)))
        (if (not (equalp "attachment" (car content-disposition)))
        mime-type
          nil)))

(defun print-mime-type (mime-types) ;; 自作しました
  (cond ((find-if #'(lambda (str) (equalp str "text/plain")) mime-types) "text/plain")
        ((find-if #'(lambda (str) (equalp str "message/rfc822")) mime-types) "message/rfc822")
        ((find-if #'(lambda (str) (equalp str "text/html")) mime-types) "text/html")
        (t nil)))

(defun mime-types (structure) ;; 自作しました
  (let ((header (junk::mailstructure-headers structure)))
    (if (junk::mail-multipart-p header)
        (let
            ((childs (junk::mailstructure-structure structure)))
          (remove nil (apply #'append (mapcar #'mime-types childs))))
      (let ((mime-type (header-to-mime-type header)))
        (cons mime-type ())))))

(defun message-print-body (structure &optional (print-part t)) ;; 書き換えました
  (message-print-body-sub structure print-part (print-mime-type (mime-types structure))))

(defun message-print-body-sub (structure print-part mime-type-to-print) ;; 書き換えました
  (let ((header (junk::mailstructure-headers structure)))
    (if (junk::mail-multipart-p header)
        (let ((childs (junk::mailstructure-structure structure)))
          (dolist (child childs)
            (message-print-body-sub child print-part mime-type-to-print)))
      (let* ((number (junk::mailstructure-number structure))
             (beg (junk::mailstructure-beg structure))
             (end (junk::mailstructure-end structure))
             (mime-type           (header-to-mime-type header))
             (content-disposition (junk::mail-get-header-content "content-disposition" header))
             (inline-p
;               (or (not mime-type) ;
                  (and (equalp mime-type-to-print mime-type)
                       (not (equalp "attachment" (car content-disposition)))))
             cur
             body)
        (when (and print-part
                   (or number
                       (not inline-p)))
          (message-print-part number header))
        (when inline-p
          (setq cur (point))
          (insert-buffer-substring *buffer-source* beg end)
          (junk::mail-decode-encoding header cur (point))
          (message-quote cur (point-max))
          )))
    ))

; (defun message-print-body (structure &optional (print-part t))
;   (let ((header (junk::mailstructure-headers structure)))
;     (if (junk::mail-multipart-p header)
; 	(let ((childs (junk::mailstructure-structure structure)))
; 	  (dolist (child childs)
; 	    (message-print-body child print-part)))
;       (let ((number (junk::mailstructure-number structure))
; 	    (beg (junk::mailstructure-beg structure))
; 	    (end (junk::mailstructure-end structure))
; 	    (inline-p (junk::mail-inline-p header))
; 	    cur
; 	    body)
; 	(when (and print-part
; 		   (or number
; 		       (not inline-p)))
; 	  (message-print-part number header))
; 	(when inline-p
; 	  (setq cur (point))
; 	  (insert-buffer-substring *buffer-source* beg end)
; 	  (junk::mail-decode-encoding header cur (point))
; 	  (message-quote cur (point-max))
; 	  )))
;     ))

(defun message-scroll-forward (&optional backward)
  (interactive "p")
  (refresh-screen)
  (if (if backward
	  (pos-visible-in-window-p (point-min))
	(pos-visible-in-window-p (point-max)))
      t
    (let ((nlines (max (truncate (* (window-height)
				    *message-scroll-ratio*))
		       1)))
      (scroll-window (if backward
			 (- nlines)
		       nlines))
      nil)))

(defun message-scroll-backward ()
  (interactive)
  (message-scroll-forward t))

(defun message-attribute-quote (num)
  (when *attribute-message-quote*
    (let* ((len (list-length *attribute-message-quote*))
	   (cur (mod num len)))
      (nth cur *attribute-message-quote*))))

(defun message-quote (beg end)
  (let (quotes)
    (save-excursion
      (save-restriction
	(goto-char beg)
	(narrow-to-region beg end)
	(while (scan-buffer *message-regexp-quote* :regexp t :tail nil)
	  (let ((str (match-string 0))
		num
		tmp
		attr)
	    (setq str (string-trim " 　\t" str))
	    (if (setq tmp (member str quotes :test #'equal))
		(setq num (- (list-length quotes) (list-length tmp)))
	      (progn
		(setq num (list-length quotes))
		(setq quotes (append quotes (list str)))))
	    (when (setq attr (message-attribute-quote num))
	      (apply #'set-text-attribute
		     (point)
		     (progn
		       (goto-eol)
		       (point))
		     nil
		     attr))))))))

(defun message-save-execute-attachment-select ()
  (interactive)
  (message-save-attachment-select t))

(defun message-save-attachment-select (&optional execute)
  (interactive "p")
  (let ((attachments (message-get-attachments)))
    (if attachments
	(let ((menu (create-popup-menu)))
	  (dolist (attach attachments)
	    (let ((number (car attach))
		  (filename (cdr attach))
		  (exec execute))
	      (add-menu-item menu
			     number
			     (concat number " " filename)
			     #'(lambda ()
				 (interactive)
				 (message-save-attachment number exec)))))
	  (track-popup-menu menu))
      (kamail3-message "No attachments."))))

(defun message-save-dialog (&optional filename)
  (let ((savefile (file-name-dialog
		   :title "ファイルの保存"
		   :default (and filename (merge-pathnames filename *message-attachment-directory*))
		   :initial-directory *message-attachment-directory*
		   :filter '(("すべてのファイル(*.*)" . "*.*")))))
    (when savefile
      (setq *message-attachment-directory*
	    (directory-namestring savefile)))
    savefile))

(defun message-save-attachment (part &optional execute savefile)
  (unless (junk::mailstructure-p *message-structure*)
    (return-from message-save-attachment))
  (let ((structure (message-get-part *message-structure* part)))
    (if (junk::mailstructure-p structure)
	(let* ((header (junk::mailstructure-headers structure)))
	  (setq savefile
		(or savefile
		    (message-save-dialog (junk::mail-get-filename header))))
	  (when (and savefile
		     (message-save-part structure savefile))
	    (kamail3-message "saved: ~A" savefile)
	    (when (and execute
		       (no-or-yes-p "ファイルを実行しますか？\n~A" savefile))
	      (shell-execute savefile nil))))
      (kamail3-error "part not found: ~S" part))))

(defun message-save-part (structure file)
  (let* ((header (junk::mailstructure-headers structure))
	 (beg (junk::mailstructure-beg structure))
	 (end (junk::mailstructure-end structure))
	 (encoding (junk::mail-get-transfer-encoding header)))
    (save-excursion
      (source-buffer-set)
      (cond ((equalp encoding "quoted-printable")
	     (quoted-printable-decode-region-to-file file beg end))
	    ((equalp encoding "base64")
	     (base64-decode-region-to-file file beg end))
	    (t
	     (write-region beg end file nil *encoding-sjis*))))))

(defun message-get-part (structure part)
  (let ((number (junk::mailstructure-number structure)))
    (if (equal number part)
	structure
      (let ((structures (junk::mailstructure-structure structure)))
	(dolist (st structures)
	  (let ((stchild (message-get-part st part)))
	    (when (junk::mailstructure-p stchild)
	      (return-from message-get-part stchild))))))))

(defun message-get-attachments ()
  (unless (junk::mailstructure-p *message-structure*)
    (return-from message-get-attachments))
  (message-get-attachment-list *message-structure*))

(defun message-get-attachment-list (structure)
  (let ((header (junk::mailstructure-headers structure))
	attachments)
    (if (junk::mail-multipart-p header)
	(let ((childs (junk::mailstructure-structure structure)))
	  (dolist (child childs)
	    (setq attachments
		  (append attachments (message-get-attachment-list child)))))
      (let ((number (junk::mailstructure-number structure))
	    (beg (junk::mailstructure-beg structure))
	    (end (junk::mailstructure-end structure))
	    (inline-p (junk::mail-inline-p header))
	    cur
	    body)
	(when (and number
		   (not inline-p))
	  (push (cons number
		      (message-part-filename header)) attachments))
	))
    attachments))

(defun message-part-filename (header)
  (let* ((content-type (junk::mail-get-header-content "content-type" header))
	 (mime-type (junk::mail-get-content-mime-type content-type))
	 (charset (junk::mail-get-content-charset content-type))
	 (encoding (junk::mail-get-transfer-encoding header))
	 (filename (junk::mail-get-filename header)))
    (or filename mime-type)))

(defun message-get-body (structure)
  (let ((buf (concat " " *buffer-prefix* "Body Temp*"))
	body)
    (save-excursion
      (set-buffer (get-buffer-create buf))
      (erase-buffer (selected-buffer))
      (message-print-body structure nil)
      (setq body (buffer-substring (point-min) (point-max))))
    (delete-buffer buf)
    body))

(defun message-reply (&optional all)
  (interactive "p")
  (draft-create-reply *message-structure* all))

(defun message-reply-all ()
  (interactive)
  (draft-create-reply-all *message-structure*))

(defun message-forward ()
  (interactive)
  (let ((attachments (message-autosave-attachments)))
    (draft-create-forward *message-structure*
			  attachments)))

(defun message-forward-as-attachment ()
  (interactive)
  (draft-create-forward-as-attachment *message-structure*
				      *message-file-current*))

(defun message-autosave-directory (filename)
  (let ((number (fetch-file-number filename)))
    (when number
      (merge-pathnames
       (format nil "~6,'0D" number)
       *message-autosave-base-directory*))))

(defun message-autosave-attachments ()
  (let ((dir (message-autosave-directory *message-file-current*))
	(as (message-get-attachment-list *message-structure*))
	attachments)
    (when (and dir as)
      (create-directory-if *message-autosave-base-directory*)
      (create-directory-if dir)
      (dolist (a as)
	(let ((number (car a))
	      (file   (cdr a))
	      path)
	  (when (and file
		     (not (string-match "/" file)))
	    (setq path (merge-pathnames file dir))
	    (message-save-attachment number nil path)
	    (push path attachments))))
      (nreverse attachments))))

(defun message-cleanup-attachments ()
  (interactive)
  (let (files
	dirs
	(num 0))
    (kamail3-message "searching files ...")
    (setq files (directory *message-autosave-base-directory*
			   :file-only t
			   :absolute t
			   :recursive t))
    (setq dirs (directory *message-autosave-base-directory*
			  :directory-only t
			  :absolute t))
    (when files
      (kamail3-message "deleting files ...")
      (mapc #'(lambda (x)
		(delete-file x)
		(incf num)
		(do-events)) files)
      (when dirs
	(kamail3-message "deleting directories ...")
	(mapc #'(lambda (x)
		  (delete-directory x)
		  (do-events)) dirs)))
    (kamail3-message "~D file~P deleted." num num)))
