;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: edit.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/edit.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/edit")

(in-package "kamail3")

(defvar *edit-major-mode* 'text-mode)
(defvar *edit-mode-hook* nil)

(defvar *edit-winconf* nil)

(defvar *edit-map* (make-sparse-keymap))
(define-key *edit-map* '(#\C-c #\q) 'edit-close)
(define-key *edit-map* '(#\C-x #\C-s) 'edit-save)
(define-key *edit-map* #\M-Insert     'signature-insert)

(defun edit-mode (&optional key)
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
  (funcall *edit-major-mode*)
  (title-mode-line)
  (set-minor-mode-map *edit-map*)
  (run-hooks '*edit-mode-hook*)
  )

(defun edit-winconf-save ()
  (setq *edit-winconf*
	(current-window-configuration)))

(defun edit-winconf-restore ()
  (when *edit-winconf*
    (set-window-configuration *edit-winconf*)
    (setq *edit-winconf* nil)))

(defun edit-buffer-set ()
  (set-buffer (get-buffer-create *buffer-edit*)))

(defun edit-buffer-create (draft &optional pos)
  (edit-buffer-set)
  (edit-mode)
  (junk::modify-read-only-buffer
    (erase-buffer (selected-buffer))
    (let ((body (draft-body draft)))
      (when body
	(insert body))
      (goto-char (or pos (point-min))))))

(defun edit-window-create (line)
  (edit-winconf-save)
  (split-window line)
  (refresh-screen)
  (while (not (pos-visible-in-window-p (point-min)))
    (scroll-window -1))
  (other-window 1))

(defun edit-save ()
  (interactive)
  ; from den8view.l
  (when (re-search-forward "[\xa0-\xdf\X8540-\X889e\Xeb40-\Xffff]+" t)
    (ed::show-match)
    (kamail3-error "ëóêMÇµÇøÇ·Ç‹Ç∏Ç¢ï∂éöî≠å©"))
  (setf (draft-body *draft-current*)
	(buffer-substring (point-min)
			  (point-max)))
  (edit-close)
  (draft-refresh))

(defun edit-close ()
  (interactive)
  (edit-winconf-restore)
  (delete-buffer *buffer-edit*))
