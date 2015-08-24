;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: expire.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/expire.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/expire")

(in-package "kamail3")

(defvar *expire-config-list* nil)
(defvar *expire-config-file*
  (merge-pathnames "expire-config.dat" *data-directory*))

(defun expire-config-load ()
  (when (file-exist-p *expire-config-file*)
    (setq *expire-config-list*
	  (junk::data-read-from-file *expire-config-file*))))

(defun expire-config-save ()
  (junk::data-write-to-file *expire-config-file*
			    *expire-config-list*))

(defun expire-config-add (query)
  (setq *expire-config-list*
	(append *expire-config-list*
		(list query)))
  (expire-config-save))

(defun expire-config-delete (query)
  (setq *expire-config-list*
	(delete query *expire-config-list*
		:test #'equal))
  (expire-config-save))

(defun expire-config-delete-select-menu (&optional title-p)
  (let ((menu (create-popup-menu)))
    (when *expire-config-list*
      (when title-p
	(add-menu-item menu
		       nil
		       "Expireê›íËçÌèú"
		       nil
		       #'(lambda () :disable))
	(add-menu-separator menu))
      (dolist (query *expire-config-list*)
	(let ((q query))
	  (add-menu-item menu
			 nil
			 (query-folder-name-default q)
			 #'(lambda ()
			     (interactive)
			     (expire-config-delete q))))))
    menu))

(defun expire-config-delete-select ()
  (interactive)
  (let ((menu (expire-config-delete-select-menu)))
    (when *expire-config-list*
      (track-popup-menu menu))))

(defun expire-execute ()
  (let ((cnt 0))
    (when *expire-config-list*
      (dolist (query *expire-config-list*)
	(kamail3-message "expiring ~@[~A~] ..."
			 (query-folder-name-default query))
	(let ((deleted-cnt (query-search-and-delete query)))
	  (when deleted-cnt
	    (incf cnt deleted-cnt)))))
    cnt))
