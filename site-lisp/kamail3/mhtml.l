;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: mhtml.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/mhtml.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/mhtml")

(in-package "kamail3")

(defvar *mhtml-browser-position* bx::XPIS_BOTTOM)
(defvar *mhtml-browser-height* 400)

(defvar *mhtml-temp-file*
  (merge-pathnames "mhtml-temp.mht" *data-directory*))

(define-key *summary-map* #\,            'summary-view-html-mail-or-scroll)
(define-key *summary-map* #\q            'mhtml-close-browser-if)

(defun summary-view-html-mail-or-scroll (&optional shell-exec)
  (interactive "p")
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (file (summary-message-file header)))
    (unless file
      (return-from summary-view-html-mail-or-scroll))
    (when (message-window-get)
      (message-close))
    (if (mhtml-browser-open-p)
	(bx:browserex-scroll-down)
      (mhtml-view file shell-exec))))

(defun mhtml-view (file &optional shell-exec)
  (when (copy-file file *mhtml-temp-file*
		   :if-exists :overwrite)
    (if shell-exec
	(shell-execute *mhtml-temp-file*)
      (mhtml-view-html-mail *mhtml-temp-file*))))

(defun mhtml-browser-open-p ()
  (bx::browser-open-p))

(defun mhtml-view-html-mail (file)
  (bx::navigate file
		:position *mhtml-browser-position*
		:size *mhtml-browser-height*))

(defun mhtml-close-browser-if ()
  (interactive)
  (if (mhtml-browser-open-p)
      (mhtml-close-browser)
    (summary-close)))

(defun mhtml-close-browser ()
  (bx::close-browser))
