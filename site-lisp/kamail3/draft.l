;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: draft.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/draft.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/draft")

(in-package "kamail3")

(defstruct draft
  header
  body
  body-header
  attachments
  boundary
  )

; (defvar *draft-attachment-directory*
;   (map-backslash-to-slash (get-special-folder-location :personal)))

(defvar *draft-attachment-directory*
  (map-backslash-to-slash (get-special-folder-location :desktop)))

(defvar *draft-mode-hook* nil)

(defvar *draft-content-type-default* "text/plain; charset=iso-2022-jp")
(defvar *draft-mime-version-default* "1.0")

(defvar *draft-attachment-encoding* "base64")

(defvar *draft-header-default-fields*
  '("from" "to" "subject"))

(defvar *draft-header-default-values*
  (list (cons "content-type" *draft-content-type-default*)
	(cons "mime-version" *draft-mime-version-default*)))

(defvar *draft-current* nil)
(defvar *draft-history* nil)
(defvar *draft-history-number* nil)

(defvar *draft-winconf* nil)

(defvar *draft-quote-string* "> ")
(defvar *draft-subject-reply-prefix* "Re: ")
(defvar *draft-subject-forward-prefix* "Fw: ")
(defvar *draft-body-reply-prefix* "-- \n")

(defvar *draft-create-pre-hook* nil)
(defvar *draft-create-post-hook* nil)

(defvar *draft-header-field-list*
  '("from" "to" "cc" "bcc" "subject"))

(defvar *draft-header-field-list-quote*
  '("date" "from" "to" "cc" "subject"))

(defvar *draft-line-string* "-")

(defvar *draft-history-file*
  (merge-pathnames "draft-history.dat" *data-directory*))

(defvar *draft-edit-window-height-adjust* 2)

(defvar *draft-map* nil)
(setq *draft-map* (make-sparse-keymap))
(define-key *draft-map* #\RET          'draft-edit-current)
(define-key *draft-map* #\d            'draft-delete-current)
(define-key *draft-map* #\s            'draft-edit-subject)
(define-key *draft-map* #\f            'draft-edit-from)
(define-key *draft-map* #\e            'draft-edit-body)
(define-key *draft-map* #\t            'draft-add-to)
(define-key *draft-map* #\T            'draft-add-to-dialog)
(define-key *draft-map* #\c            'draft-add-cc)
(define-key *draft-map* #\C            'draft-add-cc-dialog)
(define-key *draft-map* #\b            'draft-add-bcc)
(define-key *draft-map* #\B            'draft-add-bcc-dialog)
(define-key *draft-map* #\m            'draft-edit-content-type)
(define-key *draft-map* #\a            'draft-add-attachments)
(define-key *draft-map* #\A            'draft-add-attachment-directory)
(define-key *draft-map* '(#\C-c #\C-c) 'draft-send)
(define-key *draft-map* '(#\C-x #\C-s) 'draft-save)
(define-key *draft-map* #\n            'draft-next)
(define-key *draft-map* #\p            'draft-prev)
(define-key *draft-map* #\C-d          'draft-history-delete-current)
(define-key *draft-map* #\q            'draft-close)

(defun draft-buffer-set ()
  (set-buffer (get-buffer-create *buffer-draft*)))

(defun draft-history-load ()
  (when (file-exist-p *draft-history-file*)
    (setq *draft-history*
	  (junk::data-read-from-file *draft-history-file*))))

(defun draft-history-save ()
  (junk::data-write-to-file *draft-history-file*
			    *draft-history*))

(defun draft-history-push (draft)
  (push draft *draft-history*))

(defun draft-header-set (header field value)
  (let (old)
    (if (setq old (junk::mail-get-header-value field header))
	(setf (junk::mail-get-header-value field header)
	      (if (listp old)
		  (append old (list value))
		(list old value)))
      (push (cons field value) header))
    header))

(defun draft-header-add-or-replace (header field value)
  (let (old)
    (if (setq old (junk::mail-get-header-value field header))
	(setf (junk::mail-get-header-value field header) value)
      (push (cons field value) header))
    header))

(defun draft-body-quote (body)
  (unless (stringp body)
    (return-from draft-body-quote))
  (with-output-to-string (s)
    (let ((lines (split-string body "\n" t)))
      (dolist (line lines)
	(format s "~A~A~%"
		*draft-quote-string*
		line)))))

(defun draft-mode ()
  (interactive)
  (kill-all-local-variables)
  (setq mode-name *prog-name*)
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
  (use-keymap *draft-map*)
  (make-local-variable 'mode-line-format)
  (title-mode-line)
  (run-hooks '*draft-mode-hook*))

(defun draft-mode-line ()
  (setq mode-line-format (format nil "%b: ~D/~D"
				 (1+ *draft-history-number*)
				 (length *draft-history*))))

(defun draft-boundary-string ()
  (format nil "~A_~8,'0X~8,'0X.~8,'0X"
	  *prog-name*
	  (get-universal-time)
	  (random #x7fffffff)
	  (get-internal-real-time)))

(defun draft-set-default-values (draft)
  (let ((header (draft-header draft)))
    (dolist (h *draft-header-default-values*)
      (let ((field (car h))
	    (value (cdr h)))
	(setq header (draft-header-add-or-replace header field value))))
    (dolist (field *draft-header-default-fields*)
      (unless (junk::mail-get-header-value field header)
	(setq header (draft-header-add-or-replace header field nil))))
    (setf (draft-header draft) header)
    draft))

(defun draft-winconf-save ()
  (setq *draft-winconf*
	(current-window-configuration)))

(defun draft-winconf-restore ()
  (when *draft-winconf*
    (set-window-configuration *draft-winconf*)
    (setq *draft-winconf* nil)))

; (defun draft-make-default ()
;   (let ((header (list (cons "from" (draft-from-default)))))
;     (make-draft :header header)))

(defun draft-make-default ()
  (let ((header (list (cons "from" (draft-from-default))))
        (body (default-body-create))) ;;; added by fuji
    (make-draft
     :header header
     :body body ; added by fuji
     )))

(defun default-body-create () ;;; added by fuji
  (concat "\n\n" (if (= 1 (length *signature-list*)) (car *signature-list*) "")))

(defun draft-create (&optional draft)
  (interactive)
  (draft-winconf-save)
  (if draft
      (setq *draft-current* draft)
    (setq *draft-current* (draft-make-default)))
  (setq *draft-current*
	(draft-set-default-values *draft-current*))
  (run-hooks '*draft-create-pre-hook*)
  (draft-history-load)
  (draft-history-push *draft-current*)
  (setq *draft-history-number* 0)
  (draft-buffer-set)
  (draft-mode)
  (draft-print)
  (run-hooks '*draft-create-post-hook*))

(defun draft-save ()
  (interactive)
  (unless *draft-history-number*
    (kamail3-error "履歴番号が設定されていないため、保存できません。"))
  (setf (nth *draft-history-number* *draft-history*) *draft-current*)
  (draft-history-save)
  (when (= 0 *draft-history-number*)
    (draft-history-push (draft-set-default-values
			 (draft-make-default)))
    (incf *draft-history-number*))
  (draft-refresh))

(defun draft-goto-history (number)
  (let ((draft (nth number *draft-history*)))
    (unless draft
      (kamail3-error "履歴が取得できません。"))
    (setq *draft-current* draft)
    (setq *draft-history-number* number)
    (draft-refresh)))

(defun draft-history-delete (number)
  (let (new)
    (when (= 0 number)
      (kamail3-message "このDraftは削除できません。")
      (return-from draft-history-delete))
    (dotimes (i (list-length *draft-history*))
      (unless (= i number)
	(push (nth i *draft-history*) new)))
    (setq *draft-history* (reverse new))
    (draft-history-save)))

(defun draft-history-delete-current ()
  (interactive)
  (when (interactive-p)
    (unless (yes-or-no-p "編集中のメールを破棄しますか？")
      (return-from draft-history-delete-current)))
  (draft-history-delete *draft-history-number*)
  (when (<= (list-length *draft-history*) *draft-history-number*)
    (setq *draft-history-number* (1- (list-length *draft-history*))))
  (setq *draft-current* (nth *draft-history-number* *draft-history*))
  (when (interactive-p)
    (draft-refresh)))

(defun draft-next ()
  (interactive)
  (let ((number (1- *draft-history-number*)))
    (when (< number 0)
      (kamail3-message "次はありません。")
      (return-from draft-next))
    (draft-goto-history number)))

(defun draft-prev ()
  (interactive)
  (let ((number (1+ *draft-history-number*)))
    (when (< (1- (list-length *draft-history*)) number)
      (kamail3-message "前はありません。")
      (return-from draft-prev))
    (draft-goto-history number)))

(defun draft-refresh ()
  (let ((line (current-line-number)))
    (draft-print)
    (goto-line line)))

(defun draft-print ()
  (refresh-screen)
  (let ((width (- (window-width) 10))
	(header (draft-header *draft-current*))
	(body (draft-body *draft-current*))
	(attachments (draft-attachments *draft-current*)))
    (junk::modify-read-only-buffer
      (erase-buffer (selected-buffer))
      (draft-print-header header)
      (insert *draft-line-string* width)
      (insert "\n")
      (when attachments
	(draft-print-attachments attachments)
	(insert *draft-line-string* width)
	(insert "\n"))
      (when body
	(let ((cur (point)))
	  (apply #'set-text-attribute
		 (point)
		 (progn
		   (insert body)
		   (point))
		 'body
		 *attribute-draft-body*)
	  (goto-char cur)))
      (draft-mode-line)
      )))

(defun draft-print-attachments (attachments)
  (draft-print-header-line "attachments"
			   (mapcar #'car attachments)))

(defun draft-print-header (header)
  (dolist (field *draft-header-field-list*)
    (let* ((value (junk::mail-get-header-value field header)))
      (draft-print-header-line field value))))

(defun draft-print-header-line (field value)
  (let* ((attrs (message-header-attribute field))
	 (attr-field (car (or attrs *attribute-message-header-default*)))
	 (attr-value (cdr (or attrs *attribute-message-header-default*))))
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
	     (when value
	       (if (listp value)
		   (let ((delim (concat "\n"
					(substitute-string field "." " ")
					"  ")))
		     (insert (junk::string-join value delim)))
		 (insert (format nil "~@[~A~]" value))))
	     (point))
	   field
	   attr-value)
    (insert "\n")))

(defun draft-body-start-point ()
  (multiple-value-bind (from to tag)
      (find-text-attribute 'body)
    (or from
	(point-max))))

(defun draft-body-current-point ()
  (let ((cur (point))
	(beg 0))
    (multiple-value-bind (from to tag)
	(find-text-attribute 'body)
      (setq beg (or from
		    (point-max))))
    (max 0
	 (- cur beg))))

(defun draft-body-start-line ()
  (let ((pos (draft-body-start-point)))
    (when pos
      (save-excursion
	(goto-char pos)
	(current-line-number)))))

(defun draft-print-header-quote (header)
  (dolist (field *draft-header-field-list-quote*)
    (let* ((value (junk::mail-get-header-value field header)))
      (when value
	(draft-print-header-line field value)))))

(defun draft-get-forward-body (structure)
  (draft-get-reply-body structure nil))

(defun draft-get-reply-body (structure &optional (quote-p t))
  (let* ((header (junk::mailstructure-headers structure))
	 (body (message-get-body structure))
	 (buf (concat " " *buffer-prefix* "Reply Temp*"))
	 str)
    (save-excursion
      (set-buffer (get-buffer-create buf))
      (erase-buffer (selected-buffer))
      (insert (default-body-create)) ; added by fuji
      (insert *draft-body-reply-prefix*)
      (draft-print-header-quote header)
      (insert "\n")
      (when body
	(insert (if quote-p
		    (draft-body-quote body)
		  body)))
      (setq str (buffer-substring (point-min) (point-max))))
    (delete-buffer buf)
    str))

(defun draft-subject-reply (subject)
  (setq subject (or subject ""))
  (unless (string-matchp "^re:" subject)
    (setq subject (concat *draft-subject-reply-prefix* subject)))
  subject)

(defun draft-subject-forward (subject)
  (setq subject (or subject ""))
  (unless (string-matchp "^fw:" subject)
    (setq subject (concat *draft-subject-forward-prefix* subject)))
  subject)

(defun draft-from-default ()
  (let ((addr (cond ((listp *from-address*)
		     (if (consp (car *from-address*))
			 (car *from-address*)
		       *from-address*))
		    (t
		     nil))))
    (when addr
      (format-address (car addr) (cdr addr)))))

(defun draft-create-reply-all (structure)
  (draft-create-reply structure t))

(defun draft-create-reply (structure &optional all)
  (let* ((old-header (junk::mailstructure-headers structure))
	 (old-from (junk::mail-get-header-value "from" old-header))
	 (old-to (junk::mail-get-header-value "to" old-header))
	 (old-cc (junk::mail-get-header-value "cc" old-header))
	 (old-reply-to (junk::mail-get-header-value "reply-to" old-header))
	 (old-subject (junk::mail-get-header-value "subject" old-header))
	 (old-msgid (junk::mail-get-header-value "message-id" old-header))
	 (old-ref (junk::mail-get-header-value "references" old-header))
	 (to (or old-reply-to old-from))
	 cc
	 references
	 (subject (draft-subject-reply old-subject))
	 (from (draft-from-default))
	 (body (draft-get-reply-body structure))
	 (header (list (cons "subject" subject)
		       (cons "to" to)
		       (cons "from" from)
		       )))
    (when all
      (let ((to-addr-list (junk::rfc822-parse-address to))
	    cc-addr-list)
	(dolist (f (list old-to old-cc))
	  (dolist (a (if (stringp f) (list f) f))
	    (dolist (addr (parse-addresses a))
	      (let ((email (car addr))
		    (name  (cdr addr)))
		(unless (or (member email to-addr-list :test #'equalp)
			    (member email cc-addr-list :test #'equalp))
		  (push (format-address email name) cc)
		  (push email cc-addr-list)))))))
      (when cc
	(push (cons "cc" cc) header)))
    (when old-msgid
      (push (cons "in-reply-to" old-msgid) header)
      (push (cons "references"
		  (format nil "~@[~A~% ~]~A"
			  old-ref
			  old-msgid)) header))
    (draft-create (make-draft :header (reverse header)
			      :body body))
    (goto-char (draft-body-start-point))))

(defun draft-create-forward (structure &optional attachments)
  (let* ((old-header (junk::mailstructure-headers structure))
	 (old-subject (junk::mail-get-header-value "subject" old-header))
	 (old-msgid (junk::mail-get-header-value "message-id" old-header))
	 (old-ref (junk::mail-get-header-value "references" old-header))
	 references
	 (subject (draft-subject-forward old-subject))
	 (from (draft-from-default))
	 (body (draft-get-reply-body structure nil))
	 (header (list (cons "subject" subject)
		       (cons "from" from)
		       )))
    (when old-msgid
      (push (cons "in-reply-to" old-msgid) header)
      (push (cons "references"
		  (format nil "~@[~A~% ~]~A"
			  old-ref
			  old-msgid)) header))
    (draft-create (make-draft :header (reverse header)
			      :body body))
    (when attachments
      (dolist (a attachments)
	(draft-add-attachment a))
      (draft-refresh))
    (goto-char (draft-body-start-point))))

(defun draft-create-forward-as-attachment (structure file)
  (let* ((old-header (junk::mailstructure-headers structure))
	 (old-subject (junk::mail-get-header-value "subject" old-header))
	 (old-msgid (junk::mail-get-header-value "message-id" old-header))
	 (old-ref (junk::mail-get-header-value "references" old-header))
	 references
	 (subject (draft-subject-forward old-subject))
	 (from (draft-from-default))
	 (header (list (cons "subject" subject)
		       (cons "from" from)
		       )))
    (when old-msgid
      (push (cons "in-reply-to" old-msgid) header)
      (push (cons "references"
		  (format nil "~@[~A~% ~]~A"
			  old-ref
			  old-msgid)) header))
    (draft-create (make-draft :header (reverse header)))
    (draft-add-attachment file
			  (list (cons "content-type" "message/rfc822")))
    ;(draft-attachment-header file "message/rfc822"))
    (draft-refresh)
    (goto-char (draft-body-start-point))))

(defun draft-close ()
  (interactive)
  (draft-winconf-restore)
  (delete-buffer *buffer-draft*))

(defun draft-add-header-addr-dialog (field)
  (let ((header (draft-header *draft-current*)))
    (address-dialog)
    (when *address-dialog-result-list*
      (dolist (address *address-dialog-result-list*)
	(setq header (draft-header-set header
				       field
				       (format-address (address-email address)
						       (address-name  address)))))
      (setf (draft-header *draft-current*) header)
      (draft-refresh))))

(defun draft-add-header-addr (field &optional value)
  (let* ((header (draft-header *draft-current*)))
    (setq value
	  (or value
	      (address-complete
	       (format nil "~A: " (string-capitalize field))
	       (junk::mail-get-header-value field header))))
    (when value
      (let ((address (address-get value)))
	(setq header (draft-header-set header
				       field
				       (if address
					   (format-address (address-email address)
							   (address-name  address))
					 value)))
	(setf (draft-header *draft-current*) header)
	(draft-refresh)))))

(defun draft-delete-header-addr (field)
  (let* ((header (draft-header *draft-current*))
	 (value (junk::mail-get-header-value field header)))
    (when (stringp value)
      (draft-delete-header-addr-value field value)
      (return-from draft-delete-header-addr))
    (when (listp value)
      (let ((menu (create-popup-menu)))
	(dolist (addr value)
	  (let ((a addr)
		(f field))
	    (add-menu-item menu
			   nil
			   addr
			   #'(lambda ()
			       (interactive)
			       (draft-delete-header-addr-value f a)))))
	(track-popup-menu menu)))))

(defun draft-delete-header-addr-value (field addr)
  (let* ((header (draft-header *draft-current*))
	 (value (junk::mail-get-header-value field header)))
    (unless value
      (return-from draft-delete-header-addr-value))
    (if (listp value)
	(setq header
	      (draft-header-add-or-replace header
					   field
					   (delete addr value :test #'equal)))
      (if (stringp value)
	  (setq header
		(draft-header-add-or-replace header
					     field
					     nil))))
    (setf (draft-header *draft-current*) header)
    (draft-refresh)))

(defun draft-edit-header-string (field &optional value)
  (let* ((header (draft-header *draft-current*)))
    (setq value
	  (or value
	      (read-string (format nil "~A: " (string-capitalize field))
			   :default (junk::mail-get-header-value field header))))
    (when value
      (setq header (draft-header-add-or-replace header field value))
      (setf (draft-header *draft-current*) header)
      (draft-refresh))))

(defun draft-edit-subject ()
  (interactive)
  (draft-edit-header-string "subject"))

(defun draft-edit-content-type ()
  (interactive)
  (draft-edit-header-string "content-type"))

#|
(defun draft-edit-from ()
  (interactive)
  (draft-edit-header-string "from"))
|#

(defun draft-edit-from (&optional edit-p)
  (interactive "p")
  (when edit-p
    (draft-edit-header-string "from")
    (return-from draft-edit-from))
  (unless (and (listp *from-address*)
	       (consp (car *from-address*)))
    (return-from draft-edit-from))
  (let ((menu (create-popup-menu)))
    (dolist (f *from-address*)
      (let* ((email (car f))
	     (name  (cdr f))
	     (from (format-address email name)))
	(add-menu-item menu
		       nil
		       from
		       #'(lambda ()
			   (interactive)
			   (draft-edit-header-string "from" from)))
	))
    (track-popup-menu menu)))

(defun draft-add-to ()
  (interactive)
  (draft-add-header-addr "to"))

(defun draft-add-to-dialog ()
  (interactive)
  (draft-add-header-addr-dialog "to"))

(defun draft-add-cc ()
  (interactive)
  (draft-add-header-addr "cc"))

(defun draft-add-cc-dialog ()
  (interactive)
  (draft-add-header-addr-dialog "cc"))

(defun draft-add-bcc ()
  (interactive)
  (draft-add-header-addr "bcc"))

(defun draft-add-bcc-dialog ()
  (interactive)
  (draft-add-header-addr-dialog "bcc"))

(defun draft-edit-body ()
  (interactive)
  (let ((line (draft-body-start-line))
	(pos (draft-body-current-point)))
    (edit-window-create (- line *draft-edit-window-height-adjust*))
    (edit-buffer-create *draft-current* pos)))

(defun draft-edit-current ()
  (interactive)
  (multiple-value-bind (from to tag foreground background bold underline strike-out prefix extend)
      (find-text-attribute-point (point))
    (cond ((equal tag "subject")
	   (draft-edit-subject))
	  ((equal tag "from")
	   (draft-edit-from))
	  ((or (equal tag "to")
	       (equal tag "cc")
	       (equal tag "bcc"))
	   (draft-add-header-addr tag))
	  (t
	   (draft-edit-body)))))

(defun draft-delete-current ()
  (interactive)
  (multiple-value-bind (from to tag foreground background bold underline strike-out prefix extend)
      (find-text-attribute-point (point))
    (cond ((or (equal tag "to")
	       (equal tag "cc")
	       (equal tag "bcc"))
	   (draft-delete-header-addr tag))
	  ((equal tag "attachments")
	   (draft-delete-attachment))
	  (t
	   nil))))

(defun draft-add-attachments (&optional archive-p)
  (interactive "p")
  (let ((odir (default-directory))
	ndir
	files)
    (set-default-directory *draft-attachment-directory*)
    (setq files
	  (file-name-dialog :title "添付ファイルの追加"
			    :must-exist t
			    :multiple t
			    :filter '(("すべてのﾌｧｲﾙ(*.*)" . "*.*"))))
    (when files
      (setq *draft-attachment-directory*
	    (directory-namestring (car files))))
    (if archive-p
	(let ((archive (draft-attachments-archive-create files)))
	  (when (and archive
		     (file-exist-p archive))
	    (draft-add-attachment archive)))
      (dolist (file files)
	(draft-add-attachment file)))
    (draft-refresh)
    (set-default-directory odir)
    (message "attachments added: ~S" files)
    ))

(defun draft-attachments-archive-create (files)
  (let* ((odir (default-directory))
	 (default (car files))
	 (path (directory-namestring (car files)))
	 (archive (draft-attachment-archive-filename
		   (merge-pathnames
		    (concat (pathname-name default) "." *draft-attachment-archive-suffix*)
		    path))))
    (when (and archive files)
      (create-archive archive
		      files
		      path)
      archive)))

(defun draft-attachment-archive-filename (&optional default)
  (let ((odir (default-directory))
	(path (directory-namestring default))
	archive)
    (set-default-directory path)
    (unwind-protect
	(setq archive
	      (file-name-dialog :title "アーカイブファイル名"
				:save t
				:filter *file-dialog-filter*
				:filter-index *file-dialog-filter-index*
				:overwrite t
				:hide-read-only t
				:default default))
      (set-default-directory odir))
    archive
    ))

(defvar *draft-attachment-archive-suffix* "zip")

(defun draft-add-attachment-directory ()
  (interactive)
  (let ((dir (directory-name-dialog :title "フォルダ指定"
				    :default *draft-attachment-directory*))
	archive
	files)
    (unless dir
      (return-from draft-add-attachment-directory))
    (setq archive (draft-attachment-archive-filename
		   (merge-pathnames
		    (concat (pathname-name dir) "." *draft-attachment-archive-suffix*)
		    (merge-pathnames ".." dir))))
    (setq files (directory dir
			   :absolute t
			   :file-only t
			   :recursive t))
    (when (and archive files)
      (create-archive archive
		      files
		      dir)
      (draft-add-attachment archive)
      (draft-refresh))))

(defun draft-attachment-header (file &optional content-type)
  (let ((filename (file-namestring file)))
    (setq content-type (or content-type
			   (junk::file-content-type file)))
    (list (cons "content-type" content-type)
	  (cons "content-disposition" (format nil "attachment; filename=\"~A\"" filename))
	  (cons "content-transfer-encoding" *draft-attachment-encoding*))))

(defun draft-add-attachment (file &optional attach-header)
  (let* ((header (draft-header *draft-current*))
	 (attachments (draft-attachments *draft-current*))
	 attach
	 boundary
	 content-type
	 )
    (setq attach-header
	  (or attach-header
	      (draft-attachment-header file)))
    (setq attach (cons file attach-header))
    (unless attachments
      (setq boundary (draft-boundary-string))
      (setq content-type
	    (junk::mail-get-header-value "content-type" header))
      (setf (draft-body-header *draft-current*)
	    (list (cons "content-type" content-type)))
      (setq header
	    (draft-header-add-or-replace header
					 "content-type"
					 (format nil "multipart/mixed; boundary=\"~A\""
						 boundary)))
      (setf (draft-header *draft-current*) header)
      (setf (draft-boundary *draft-current*) boundary))
    (setq attachments
	  (append attachments (list attach)))
    (setf (draft-attachments *draft-current*) attachments)
    ))


(defun draft-delete-attachment ()
  (interactive)
  (let* ((attachments (draft-attachments *draft-current*)))
    (when (listp attachments)
      (when (= 1 (list-length attachments))
	(draft-delete-attachment-file (car (car attachments)))
	(return-from draft-delete-attachment))
      (let ((menu (create-popup-menu)))
	(dolist (attachment attachments)
	  (let ((f (car attachment))
		(h (cdr attachment)))
	    (add-menu-item menu
			   nil
			   f
			   #'(lambda ()
			       (interactive)
			       (draft-delete-attachment-file f)))))
	(track-popup-menu menu)))))

(defun draft-delete-attachment-file (file)
  (let* ((attachments (draft-attachments *draft-current*)))
    (unless (and file attachments)
      (return-from draft-delete-attachment-file))
    (setq attachments
	  (delete file attachments :test #'equalp :key #'car))
    (unless attachments
      (let* ((header (draft-header *draft-current*))
	     (body-header (draft-body-header *draft-current*))
	     (content-type (junk::mail-get-header-value "content-type" body-header)))
	(setq header
	      (draft-header-add-or-replace header
					   "content-type"
					   (or content-type
					       *draft-content-type-default*)))
	(setf (draft-header *draft-current*) header)
	(setf (draft-body-header *draft-current*) nil)
	(setf (draft-boundary *draft-current*) nil)))
    (setf (draft-attachments *draft-current*) attachments)
    (draft-refresh)))

(defun draft-header-set-before-send ()
  (draft-header-date-set)
  (draft-header-x-mailer-set))

(defun draft-header-date-set ()
  (let ((header (draft-header *draft-current*)))
    (setq header
	  (draft-header-add-or-replace header
				       "date"
				       (junk::rfc822-format-date-local)))
    (setf (draft-header *draft-current*) header)))

(defun draft-header-x-mailer-set ()
  (let ((header (draft-header *draft-current*)))
    (setq header
	  (draft-header-add-or-replace header "x-mailer" *user-agent*))
    (setf (draft-header *draft-current*) header)))

(defun draft-check ()
  (let ((header (draft-header *draft-current*))
	(fields '("from" "to" "subject"))
	(body (draft-body *draft-current*))
	(attachments (draft-attachments *draft-current*))
	(result t)
	error-message)
    (setq error-message
	  (with-output-to-string (s)
	    (dolist (field fields)
	      (unless (junk::mail-get-header-value field header)
		(format s "~Aがありません。" (string-capitalize field))
		(setq result nil)))
	    (unless body
	      (format s "本文がありません。")
	      (setq result nil))
	    (when attachments
	      (dolist (attachment attachments)
		(let ((file (car attachment)))
		  (unless (file-exist-p file)
		    (format s "添付ファイルが存在しません: ~A" file)))))
	    ))
    (unless result
      (kamail3-error "~A" error-message))))

(defun draft-add-addresses ()
  (let ((header (draft-header *draft-current*)))
    (dolist (f (list "to" "cc" "bcc"))
      (let ((val (junk::mail-get-header-value f header)))
	(dolist (a (if (stringp val)
		       (list val)
		     val))
	  (when a
	    (multiple-value-bind (email name)
		(parse-address a)
	      (when (and email
			 (not (address-get email)))
		(address-push email name *address-group-default*)))))))))

(defun draft-send ()
  (interactive)
  (draft-check)
  (unless (yes-or-no-p "送信しますか？")
    (return-from draft-send))
  (draft-header-set-before-send)
  (send-buffer-create *draft-current*)
  (multiple-value-bind (flag return)
      (send-buffer-send)
    (if flag
	(progn
	  (send-buffer-save-and-clear)
	  (draft-add-addresses)
	  (draft-history-delete-current)
	  (draft-close)
	  (kamail3-message "message sent."))
      (kamail3-error "SMTP Error: ~A" (if return return "unknown")))))
