;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: defs.l 794 2008-02-08 20:50:11Z torihat $
;
; kamail3/defs.l
;
; by HATTORI Masashi

(defpackage "kamail3"
  (:use "lisp" "editor")
  (:nicknames "km3"))

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "junk/defs")
  (require "junk/macro")
  (require "junk/mail"))

(require "junk/lib")
(require "junk/log")
(require "junk/mime-encode")
(require "junk/rfc822")
(require "junk/rfc2045")
(require "junk/pop3")
(require "junk/smtp")

(provide "kamail3/defs")

(in-package "kamail3")

(defvar *debug-mode* nil)

(defvar *prog-name* "KaMailV3")
(defvar *prog-version* "0.0.1.4beta7")
(defvar *pframe-name* "KaMailV3")
(defvar *pframe-old* nil)

(defvar *user-agent*
  (format nil "~A/~A xyzzy-~A (~A)"
	  *prog-name*
	  *prog-version*
	  (software-version)
	  (os-platform)))

; (defvar *base-directory* "~/.kamail3")

(defvar *base-directory* "C:/Mail")

(defvar *config-file*
  (merge-pathnames "config.l" *base-directory*))

(defvar *data-directory*
  (merge-pathnames "data" *base-directory*))

(defvar *mail-directory*
  (merge-pathnames "mail" *base-directory*))

(defvar *draft-directory*
  (merge-pathnames "pinbox" *base-directory*))

(defvar *work-directory*
  (merge-pathnames "pwork" *base-directory*))

(defvar *use-pseudo-index* nil)

(defvar *buffer-prefix* "*KaMail-V3 ")

(defconstant *header-status* "x-kamail3-status")
(defconstant *header-tag*    "x-kamail3-tag")
(defconstant *header-folder* "x-kamail3-folder")

(defvar *summary-mark-string-marked* "*")
(defvar *summary-mark-string-normal* " ")

(defconstant *attr-status-unseen*    "U")
(defconstant *attr-status-seen*      "R")
(defconstant *attr-status-answered*  "A")
(defconstant *attr-status-forwarded* "F")

(defvar *attr-tags-list* nil)
(defvar *attr-tags-list-file*
  (merge-pathnames "tag-list.dat" *data-directory*))

(defvar *summary-status-multipart* "@")
(defvar *summary-status-unseen*    "U")
(defvar *summary-status-seen*      " ")
(defvar *summary-status-answered*  "A")
(defvar *summary-status-forwarded* "F")

(defvar *buffer-summary*   (concat *buffer-prefix* "Summary*"))
(defvar *buffer-message*   (concat *buffer-prefix* "Message*"))
(defvar *buffer-draft*     (concat *buffer-prefix* "Draft*"))
(defvar *buffer-edit*      (concat *buffer-prefix* "Edit*"))
(defvar *buffer-send*      (concat *buffer-prefix* "Send*"))
(defvar *buffer-signature* (concat *buffer-prefix* "Signature*"))
(defvar *buffer-source*    (concat *buffer-prefix* "Source*"))
(defvar *buffer-fetch*     (concat *buffer-prefix* "Fetch*"))

(defvar *attribute-summary-mark*       (list :foreground  1))
(defvar *attribute-summary-status*     (list :foreground 13))
(defvar *attribute-summary-multipart*  (list :foreground 12))
(defvar *attribute-summary-date*       (list :foreground 10))
(defvar *attribute-summary-from*       (list :foreground 11))
(defvar *attribute-summary-title*      (list :foreground 14))

(defvar *attribute-message-header*
  (list
   (cons "date"     (cons (list :bold t :foreground 9)
			  (list :bold t :foreground 10)))
   (cons "from"     (cons (list :bold t :foreground 9)
			  (list :bold t :foreground 11)))
   (cons "to"       (cons (list :bold t :foreground 9)
			  (list :bold t :foreground 12)))
   (cons "cc"       (cons (list :bold t :foreground 9)
			  (list :bold t :foreground 13)))
   (cons "subject"  (cons (list :bold t :foreground 9)
			  (list :bold t :foreground 14)))
   (cons "reply-to" (cons (list :bold t :foreground 9)
			  (list :bold t :foreground 11)))
   ))

(defvar *attribute-message-header-default*
  (cons (list :foreground 9)
	(list :foreground 15)))

(defvar *attribute-message-part*
  (list :foreground 1))

(defvar *attribute-draft-body* nil)

(defvar *message-header-field-list*
  '("date"
    "from"
    "subject"
    "to"
    "cc"
    "reply-to"
    "x-mailer"
    "user-agent"))

(defvar *attribute-message-quote*
  (list (list :foreground 9)
	(list :foreground 10)
	(list :foreground 11)
	(list :foreground 12)))

;den8view.lより
(defvar *message-regexp-quote* "^\\([ \t　]*[|>＞》]+\\|[A-Za-z0-9]+>\\)[ \t　|>＞》]*")
(defvar *message-scroll-ratio* 2/3)

(setq *summary-window-height* 10)

(defvar *fetch-config-list* nil)
(defvar *fetch-type-list* nil)

(defvar *fetch-execute-all-hook* nil)

(defvar *smtp-config-list* nil)

(defvar *from-address* nil)

(defvar *kamail3-finish-hook* nil)
(defvar *kamail3-start-hook* nil)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun title-mode-line ()
  (make-local-variable 'title-bar-format)
  (setq title-bar-format *user-agent*))

(defun kamail3-message (fmt &rest args)
  (apply #'message (concat *prog-name* ": " fmt) args))

(defun kamail3-msgbox (fmt &rest args)
  (message-box (apply #'format nil fmt args)
	       *prog-name*))

(defun kamail3-error (fmt &rest args)
  (apply #'error (concat *prog-name* "~%~%" fmt) args))

(defun insert-file-noconv (file)
  (let ((*expected-fileio-encoding* *encoding-sjis*))
    (declare (special *expected-fileio-encoding*))
    (insert-file-contents file)))

(defun create-directory-if (dir)
  (unless (file-exist-p dir)
    (or (create-directory dir)
	(kamail3-error "Could not create directory: ~S" dir))))

(defvar *parse-address-regexp* "<?\\([-0-9a-zA-Z\._]+@[-0-9a-zA-Z\._]+\\)>?")

(defun parse-addresses (addr-string)
  (unless (stringp addr-string)
    (return-from parse-addresses))
  (let ((tmp-list (split-string addr-string ","))
	tmp
	addrs)
    (while (setq tmp (car tmp-list))
      (setq tmp-list (cdr tmp-list))
      (if (string-match *parse-address-regexp* tmp)
	  (multiple-value-bind (email name)
	      (parse-address tmp)
	    (when email
	      (pushnew (cons email name) addrs :test #'equal :key #'car)))
	(setf (car tmp-list) (concat tmp (car tmp-list)))))
    (nreverse addrs)))

(defun parse-address (addr-string)
  (when (and (stringp addr-string) (string-match *parse-address-regexp* addr-string))
    (let ((name (string-trim "\"' \n\t" (substring addr-string 0 (match-beginning 0)))))
      (values (match-string 1) (and (not (string= "" name)) name)))))

; (defun parse-address (addr-string)
;   (unless (stringp addr-string)
;     (return-from parse-address))
;   (let (name
; 	email)
;     (when (string-match *parse-address-regexp* addr-string)
;       (setq email (match-string 1))
;       (setq name (concat
; 		  (string-trim " \n\t" (substring addr-string 0 (match-beginning 0)))
; 		  " "
; 		  (string-trim " \n\t" (substring addr-string (match-end 0) (length addr-string))))))
;     (when name
;       (setq name (string-trim " \n\t" name))
;       (when (string-match "^[(（\"'].*[)）\"']$" name)
; 	(setq name (substring name 1 (1- (length name)))))
;       (when (equal name "")
; 	(setq name nil)))
;     (values email name)))

(defun format-address (email name)
  (if name
      (format nil "\"~A\" <~A>" name email)
    email))

(defun abbreviate-menu-string (string)
  (let ((max 74))
    (if (<= (length string) max)
	string
      (concat (abbreviate-string-column string (- max 3))
	      "..."))))

(defun parse-mailto (mailto)
  (when (string-matchp "mailto:\\([^?]+\\)\\(\\?\\(.+\\)\\)?" mailto)
    (let* ((header
	    (cons (cons "to" (subseq mailto (match-beginning 1) (match-end 1)))
		  (and (match-beginning 3)
		       (mapcan #'(lambda (x)
				   (let ((i (position #\= x)))
				     (and i (list (cons (nstring-downcase (subseq x 0 i))
							(subseq x (+ i 1)))))))
			       (split-string (subseq mailto (match-beginning 3)) #\&)))))
	   (encoding (and (find #\% mailto)
			  (detect-char-encoding (si:www-url-decode mailto)))))
      (if encoding
	  (mapcar #'(lambda (x)
		      (let ((f (car x))
			    (v (cdr x)))
			(cons f (convert-encoding-to-internal
				 encoding (si:www-url-decode v)))))
		  header)
	header))))

(defun show-help ()
  (interactive)
  (let ((path
	 (merge-pathnames "site-lisp/kamail3/readme.html"
			  (si:system-root))))
    (shell-execute path t)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun init ()
  (create-directory-if *base-directory*)
  (create-directory-if *data-directory*)
  (create-directory-if *mail-directory*)
  (create-directory-if *draft-directory*)
  ;(create-directory-if *work-directory*)
  (he-create)
  (attr-tags-load)
  (query-history-load)
  (address-load)
  (folder-list-special-create)
  (folder-load)
  (expire-config-load)
  )

(defun load-config ()
  (junk::load-config-file *config-file*))

(defun frame-set ()
  (setq *pframe-old* (ed::pseudo-frame-name (selected-pseudo-frame)))
  (new-pseudo-frame *pframe-name*))

(defun frame-get ()
  (find-pseudo-frame *pframe-name*))

(defun hide ()
  (interactive)
  (let ((old (find-pseudo-frame *pframe-old*)))
    (when old
      (select-pseudo-frame old))))

(defun restore ()
  (interactive)
  (let ((frame (frame-get)))
    (if frame
	  (select-pseudo-frame frame)
      (user::kamail3))))

(defun finish ()
  (interactive)
  (let ((frame (frame-get)))
    (dolist (buf (buffer-list))
      (let ((bufname (buffer-name buf)))
	(when (string-match (concat "^" (regexp-quote *buffer-prefix*)) bufname)
	  (delete-buffer buf))))
    (when frame
      (delete-pseudo-frame frame)))
  (run-hooks '*kamail3-finish-hook*))

(defun user::kamail3-mailto (mailto)
  (let ((header (parse-mailto mailto)))
    (user::kamail3)
    (pushnew (cons "from" (draft-from-default)) header
	     :test #'equal
	     :key #'car)
    (draft-create (make-draft :header header))))

(defun user::kamail3-toggle ()
  (interactive)
  (if (equal *pframe-name*
	     (ed::pseudo-frame-name (selected-pseudo-frame)))
      (hide)
    (restore)))

(defun user::kamail3 ()
  (interactive)
  (when (frame-get)
    (restore)
    (return-from user::kamail3))
  (load-config)
  (init)
  (frame-set)
  (folder-open *folder-inbox*)
  (run-hooks '*kamail3-start-hook*)
  (kamail3-message "started."))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(require "kamail3/estcmd")
(require "kamail3/summary")
(require "kamail3/message")
(require "kamail3/attr")
(require "kamail3/fetch")
(require "kamail3/refile")
(require "kamail3/fetch-mh")
(require "kamail3/fetch-pop3")
(require "kamail3/folder")
(require "kamail3/query")
(require "kamail3/draft")
(require "kamail3/edit")
(require "kamail3/send")
(require "kamail3/expire")
(require "kamail3/address")
(require "kamail3/signature")
