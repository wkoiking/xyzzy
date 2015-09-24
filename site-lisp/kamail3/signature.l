;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: signature.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/signature.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/signature")

(in-package "kamail3")

(defvar *signature-list-file*
  (merge-pathnames "signature.dat" *data-directory*))

(defvar *signature-list* nil)
(defvar *signature-current-number* 0)

(defvar *signature-winconf* nil)

(defvar *signature-map* nil)
(setq *signature-map* (make-sparse-keymap))
(define-key *signature-map* #\RET 'signature-insert-current)
(define-key *signature-map* #\n   'signature-next)
(define-key *signature-map* #\p   'signature-prev)
(define-key *signature-map* #\q   'signature-close)

(defun signature-mode (&optional key)
  (setq mode-name *prog-name*)
  (setq need-not-save t)
  (setq auto-save nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-line-number* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-newline* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-eof* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-cursor-line* t)
  (set-local-window-flags (selected-buffer)
			  *window-flag-vscroll-bar* t)
  (set-local-window-flags (selected-buffer)
			  *window-flag-ruler* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-just-inverse* t)
  (set-buffer-fold-width nil)
  (make-local-variable 'mode-line-format)
  (title-mode-line)
  (use-keymap *signature-map*)
  (run-hooks '*signature-mode-hook*)
  )

(defun signature-mode-line ()
  (setq mode-line-format (format nil "%b: ~D/~D"
				 (1+ *signature-current-number*)
				 (length *signature-list*))))

#|
(define-key *signature-map* #\d   'signature-delete-current)

(defun signature-list-load ()
  (when (file-exist-p *signature-list-file*)
    (setq *signature-list*
	  (junk::data-read-from-file *signature-list-file*))))

(defun signature-list-save ()
  (junk::data-write-to-file *signature-list-file*
			    *signature-list*))
|#

(defun signature-buffer-set ()
  (set-buffer (get-buffer-create *buffer-signature*)))

(defun signature-winconf-save ()
  (setq *signature-winconf*
	(current-window-configuration)))

(defun signature-winconf-restore ()
  (when *signature-winconf*
    (set-window-configuration *signature-winconf*)
    (setq *signature-winconf* nil)))

(defun signature-window-create ()
  (signature-winconf-save)
  (split-window)
  (other-window 1))

(defun signature-show (number)
  (let ((signature (nth number *signature-list*)))
    (unless signature
      (kamail3-error "署名がありません。"))
    (junk::modify-read-only-buffer
      (erase-buffer (selected-buffer))
      (insert signature)
      (signature-mode-line))))

(defun signature-show-current ()
  (signature-show *signature-current-number*))

(defun signature-show-default ()
  ;;; Draftの送信者を元に自動的に出すような処理をここで
  (let* ((number 0)
	 (header (if (draft-p *draft-current*)
		     (draft-header *draft-current*)))
	 (from (junk::mail-get-header-value "from" header))
	 email)
    (when from
      (multiple-value-bind (email name)
	  (parse-address from)
	(when email
	  (dotimes (i (list-length *signature-list*))
	    (let ((sig (nth i *signature-list*)))
	      (when (string-match (regexp-quote email) sig)
		(setq number i)
		(return)))))))
    (setq *signature-current-number* number)
    (signature-show-current)))

(defun signature-insert ()
  (interactive)
  ;(signature-list-load)
  (setq *signature-current-number* 0)
  (unless *signature-list*
    (kamail3-message "No signature.")
    (return-from signature-insert))
  (if (= 1 (length *signature-list*))
      (insert (car *signature-list*))
    (progn
      (signature-window-create)
      (signature-buffer-set)
      (signature-mode)
      (signature-show-default))))

(defun signature-close ()
  (interactive)
  (signature-winconf-restore)
  (delete-buffer *buffer-signature*))

(defun signature-next (&optional reverse-p)
  (interactive "p")
  (setq *signature-current-number*
	(mod (if reverse-p
		 (1- *signature-current-number*)
	       (1+ *signature-current-number*))
	     (list-length *signature-list*)))
  (signature-show-current))

(defun signature-prev ()
  (interactive)
  (signature-next t))

(defun signature-insert-current ()
  (interactive)
  (let ((signature (nth *signature-current-number* *signature-list*)))
    (signature-close)
    (when signature
      (insert signature))))
