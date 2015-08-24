;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: folder.l 740 2006-10-22 17:32:24Z torihat $
;
; kamail3/folder.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(require "kamail3/attr")
(require "kamail3/query")

(provide "kamail3/folder")

(in-package "kamail3")

(defvar *folder-file*
  (merge-pathnames "folder.dat" *data-directory*))

(defvar *folder-list-user* nil)

(defconstant *folder-archive* "archive")

(defconstant *folder-inbox*        "inbox")
(defvar      *folder-inbox-alias*  nil)
(defconstant *folder-sent*         "sent")
(defvar      *folder-sent-alias*   nil)
(defconstant *folder-unseen*       "unseen")
(defvar      *folder-unseen-alias* nil)
(defconstant *folder-spam*         "spam")
(defvar      *folder-spam-alias*   nil)
(defconstant *folder-trash*        "trash")
(defvar      *folder-trash-alias*  nil)


(defconstant *folder-order-date-asc*  "date NUMA")
(defconstant *folder-order-date-desc* "date NUMD")

(defvar *folder-attr-list*
  (list *folder-inbox*
	*folder-sent*
; 	*folder-unseen*
	*folder-spam*
	*folder-trash*
		))

(defvar *folder-order-default* *folder-order-date-desc*)

(defstruct folder
  name
  alias
  query)

(defun folder-make-attr-folder (folder alias &key attrs order)
  (make-folder :name folder
	       :alias alias
	       :query (make-query
		       :attrs (or attrs
				  (format nil "~A STREQ ~A"
					  *header-folder*
					  (attr-folder-string folder)))
		       :order (or order
				  *folder-order-default*))))

(defvar *folder-list-archive*
  (folder-make-attr-folder *folder-archive* nil))

(defvar *folder-list-special* nil)

(defun folder-list-special-create ()
  (setq *folder-list-special*
	(list (folder-make-attr-folder *folder-inbox*
				       *folder-inbox-alias*
				       :order *folder-order-date-asc*)
	      (folder-make-attr-folder *folder-sent*
				       *folder-sent-alias*
				       :order *folder-order-date-desc*)
	      (make-folder :name *folder-unseen*
			   :alias *folder-unseen-alias*
			   :query (make-query
				   :attrs (format nil "~A STREQ ~A"
						  *header-status*
						  *attr-status-unseen*)
				   :order *folder-order-date-asc*))
	      (folder-make-attr-folder *folder-spam*
				       *folder-spam-alias*
				       :order *folder-order-date-asc*)
	      (folder-make-attr-folder *folder-trash*
				       *folder-trash-alias*
				       :order *folder-order-date-asc*)
	      )))

(defun folder-load ()
  (when (file-exist-p *folder-file*)
    (setq *folder-list-user*
	  (junk::data-read-from-file *folder-file*))))

(defun folder-save ()
  (junk::data-write-to-file *folder-file* *folder-list-user*))

(defun folder-get (name)
  (if (equal name *folder-archive*)
      *folder-list-archive*
    (car (member name
		 (append *folder-list-special*
			 *folder-list-user*)
		 :test #'equal
		 :key #'folder-name))))

(defun folder-clear (name)
  (let ((folder (folder-get name)))
    (when (folder-p folder)
      (query-search-and-delete (folder-query folder)))))

(defun folder-open (name)
  (interactive "sFolder: ")
  (let ((folder (folder-get name)))
    (when (folder-p folder)
      (query-execute (folder-query folder)
		     name))))

(defun folder-open-inbox ()
  (interactive)
  (folder-open *folder-inbox*))

(defun folder-select-menu ()
  (let ((menu (create-popup-menu)))
    (dolist (folder *folder-list-special*)
      (let ((name (folder-name folder))
	    (alias (folder-alias folder)))
	(add-menu-item menu
		       name
		       (or alias name)
		       #'(lambda ()
			   (interactive)
			   (folder-open name)))))
    (when *folder-list-user*
      (add-menu-separator menu)
      (dolist (folder *folder-list-user*)
	(let ((name (folder-name folder))
	      (alias (folder-alias folder)))
	  (add-menu-item menu
			 name
			 (or alias name)
			 #'(lambda ()
			     (interactive)
			     (folder-open name))))))
    menu))

(defun folder-select ()
  (interactive)
  (let ((menu (folder-select-menu)))
    (track-popup-menu menu)))

(defun folder-add (name query &optional alias)
  (let ((folder (make-folder :name name
			     :query query
			     :alias alias)))
    (when (folder-get name)
      (kamail3-error "Folder exists: ~A" name))
    (setq *folder-list-user*
	  (append *folder-list-user* (list folder)))
    (folder-save)))

(defun folder-delete-select-menu (&optional title-p)
  (let ((menu (create-popup-menu)))
    (when *folder-list-user*
      (when title-p
	(add-menu-item menu
		       nil
		       "ŒŸõƒtƒHƒ‹ƒ_íœ"
		       nil
		       #'(lambda () :disable))
	(add-menu-separator menu))
      (dolist (folder *folder-list-user*)
	(let ((name (folder-name folder))
	      (alias (folder-alias folder)))
	  (add-menu-item menu
			 name
			 (or alias name)
			 #'(lambda ()
			     (interactive)
			     (folder-delete name))))))
    menu))

(defun folder-delete-select ()
  (interactive)
  (let ((menu (folder-delete-select-menu t)))
    (track-popup-menu menu)))

(defun folder-delete (name)
  (setq *folder-list-user*
	(delete name *folder-list-user*
		:test #'equalp
		:key #'folder-name))
  (folder-save))
