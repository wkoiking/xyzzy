;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: fetch.l 740 2006-10-22 17:32:24Z torihat $
;
; kamail3/fetch.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/fetch")

(in-package "kamail3")

(defvar *fetch-count-file*
  (merge-pathnames "fetch-count.dat" *data-directory*))

(defvar *buffer-fetch-temp*
  (concat *buffer-prefix* "Fetch Temp*"))

(defvar *fetch-count* nil)
(defvar *fetched-file-list* nil)

(defconstant *fetch-store-number* 100)

;;; 保存ファイル
;;; Mail/0000/00
;;; Mail/0000/99
;;; Mail/0001/00
(defun fetch-file-path (number)
  (let ((dir (format nil "~4,'0D" (truncate (/ number *fetch-store-number*))))
	(file (format nil "~2,'0D" (mod number *fetch-store-number*))))
    (merge-pathnames (concat dir "/" file) *mail-directory*)))

(defun fetch-file-number (file)
  (when (string-match "/\\([0-9][0-9][0-9][0-9]\\)/\\([0-9][0-9]\\)$" file)
    (parse-integer (concat (match-string 1) (match-string 2)))))

(defun fetch-create-directory-if (file)
  (let ((dir (directory-namestring file)))
    (create-directory-if dir)))

(defun fetch-count-load ()
  (unless *fetch-count*
    (setq *fetch-count*
	  (if (file-exist-p *fetch-count-file*)
	      (or (car (junk::data-read-from-file *fetch-count-file*))
		  0)
	    0))))

(defun fetch-count-save ()
  (junk::data-write-to-file *fetch-count-file* (list *fetch-count*)))

(defun fetch-file-prepare-next ()
  (let (file)
    (fetch-count-load)
    (loop
      (setq file (fetch-file-path *fetch-count*))
      (or (file-exist-p file) (return))
      (incf *fetch-count*))
    (incf *fetch-count*)
    (fetch-count-save)
    (fetch-create-directory-if file)
    file))

(defun fetched-file-add (file)
  (pushnew file *fetched-file-list* :test #'equal))

(defun fetched-file-clear ()
  (setq *fetched-file-list* nil))

(defun fetch-buffer-set ()
  (set-buffer (get-buffer-create *buffer-fetch*))
  (erase-buffer (selected-buffer)))

(defun fetched-file-store ()
  (let ((file (fetch-file-prepare-next))
	(encoding (buffer-fileio-encoding)))
    (unless file
      (kamail3-error "Fetch: could not prepare file: ~S" file))
    (set-buffer-fileio-encoding *encoding-sjis*)
    (write-region (point-min)
		  (point-max)
		  file)
    (set-buffer-fileio-encoding encoding)
    file))

(defstruct fetchconf
  type
  name
  execute)

(defun fetch-type-register (conf)
  (pushnew (cons (fetchconf-type conf)
		 conf)
	   *fetch-type-list*
	   :test #'equal
	   :key #'car))

(defun fetch-execute-func (type)
  (let ((fetchconf (cdr (assoc type *fetch-type-list*))))
    (and (fetchconf-p fetchconf)
	 (fetchconf-execute fetchconf))))

(defun fetch-name-func (type)
  (let ((fetchconf (cdr (assoc type *fetch-type-list*))))
    (and (fetchconf-p fetchconf)
	 (fetchconf-name fetchconf))))

#|
(defun fetch-sync (fetched &key folder tags status)
  (let (cmd-list)
    ;;; gather
    (push (he-gather-files-cmd fetched) cmd-list)
    ;;; folder
    (when (and folder
	       (folder-get folder))
      (dolist (file fetched)
	(push (attr-folder-update-cmd file folder) cmd-list)))
    ;;; tags
    (when tags
      (dolist (file fetched)
	(push (attr-tag-update-cmd file (attr-tags-join tags)) cmd-list)))
    ;;; status
    (when status
      (dolist (file fetched)
	(push (attr-status-update-cmd file status) cmd-list)))
    ;;; 実行
    (when cmd-list
      (kamail3-message "updating index ...")
      (he-execute-batch (nreverse cmd-list))
      (kamail3-message "updating index ... done."))))
|#
#|
(defun fetch-sync (fetched &key folder tags status)
  (let (cmd-list attrs)
    (if (and folder
	     (folder-get folder))
	(push (cons *header-folder* (attr-folder-string folder)) attrs)
      (push (cons *header-folder* (attr-folder-string *folder-inbox*)) attrs))
    (when tags
      (push (cons *header-tag* (attr-tags-join tags)) attrs))
    (if status
	(push (cons *header-status* status) attrs)
      (push (cons *header-status* *attr-status-unseen*) attrs))
    (he-gather-files fetched attrs)))
|#

(defun fetch-sync (fetched-files)
  (let ((attr-field-list (list *header-folder*
			       *header-tag*
			       *header-status*)))
    (if *use-pseudo-index*
	(fetch-convert-draft fetched-files attr-field-list)
      (he-gather-files fetched-files attr-field-list))
    ))

(defun fetch-draft-file (number)
  (merge-pathnames (format nil "~6,'0D.est" number)
		   *draft-directory*))

(defun fetch-buffer-temp-set ()
  (set-buffer (get-buffer-create *buffer-fetch-temp*))
  (erase-buffer (selected-buffer))
  (setq need-not-save t)
  (setq kept-undo-information nil)
  (setq auto-save nil))

(defun fetch-buffer-temp-delete ()
  (when (find-buffer *buffer-fetch-temp*)
    (delete-buffer *buffer-fetch-temp*)))

#|
(defun fetch-convert-draft (fetched-files attr-field-list)
  (let ((cmd-list)
	(ed::*expected-fileio-encoding* *encoding-sjis*))
    (declare (special ed::*expected-fileio-encoding*))
    (kamail3-message "creating draft files ...")
    (save-excursion
      (dolist (f fetched-files)
	(let* ((file (car f))
	       (values (cdr f))
	       (number (fetch-file-number file))
	       (draft (fetch-draft-file number))
	       (cmd (he-cmd-draft file)))
	  (fetch-buffer-temp-set)
	  (execute-shell-command cmd nil *buffer-fetch-temp*)
	  (when (eobp)
	    (kamail3-error "no draft created."))
	  (map-utf-8-region (point-min) (point-max))
	  (goto-char (point-min))
	  (when (scan-buffer "^$" :regexp t)
	    (dotimes (i (length attr-field-list))
	      (let ((field (nth i attr-field-list))
		    (value (nth i values)))
		(insert (format nil "~A=~@[~A~]~%" field value)))))
	  (write-region (point-min) (point-max) draft nil *encoding-utf8n* *eol-lf*)
	  )))
    (fetch-buffer-temp-delete)
    (kamail3-message "creating draft files ... done.")
    ))
|#

(defun fetch-convert-draft (fetched-files attr-field-list)
  (let (cmd-list)
    (kamail3-message "creating draft files ...")
    (dolist (f fetched-files)
      (let* ((file (car f))
	     (values (cdr f))
	     (number (fetch-file-number file))
	     (draft (fetch-draft-file number)))
	(push (he-cmd-draft file draft) cmd-list)
	(do-events)))
    (he-execute-batch (reverse cmd-list))
    (save-excursion
      (let ((ed::*expected-fileio-encoding* *encoding-sjis*))
	(declare (special ed::*expected-fileio-encoding*))
	(fetch-buffer-temp-set)
	(dolist (f fetched-files)
	  (let* ((file (car f))
		 (values (cdr f))
		 (number (fetch-file-number file))
		 (draft (fetch-draft-file number)))
	    (erase-buffer (selected-buffer))
	    (insert-file-contents draft)
	    (when (eobp)
	      (kamail3-error "no draft created."))
	    (map-utf-8-region (point-min) (point-max))
	    (goto-char (point-min))
	    (when (scan-buffer "^$" :regexp t)
	      (dotimes (i (length attr-field-list))
		(let ((field (nth i attr-field-list))
		      (value (nth i values)))
		  (insert (format nil "~A=~@[~A~]~%" field value)))))
	    (write-region (point-min) (point-max) draft nil *encoding-utf8n* *eol-lf*)
	    (do-events)
	    ))
	))
    (fetch-buffer-temp-delete)
    (kamail3-message "creating draft files ... done.")
    ))

(defun fetch-execute (conf)
  (let* ((type (car conf))
	 (opt  (cdr conf))
	 (func (fetch-execute-func type))
	 fetched
	 cnt)
    (unless func
      (kamail3-error "Fetch: undefined func: ~S:~S" func conf))
    (setq fetched (funcall func opt))
    (setq cnt (length fetched))
    (kamail3-message "~D message~P fetched." cnt cnt)
    fetched))

(defun fetch-execute-list (config-list)
  (let ((cnt 0))
    (long-operation
      (dolist (conf config-list)
	(let ((fetched (fetch-execute conf)))
	  (incf cnt (length fetched))
	  (do-events))))
    (kamail3-message "~D message~P fetched." cnt cnt))
  (run-hooks '*fetch-execute-all-hook*))

(defun fetch-execute-select-menu (&optional title-p)
  (let ((menu (create-popup-menu))
	(cnt 0))
    (unless *fetch-config-list*
      (return-from fetch-execute-select-menu))
    (when title-p
      (add-menu-item menu
		     nil
		     "メール選択受信"
		     nil
		     #'(lambda () :disable))
      (add-menu-separator menu))
    (dolist (c *fetch-config-list*)
      (let* ((conf c)
	     (type (car c))
	     (opt (cdr c))
	     (func (fetch-name-func type))
	     name)
	(when func
	  (incf cnt)
	  (setq name (format nil "~D.~@[~A~]" cnt (funcall func opt)))
	  (add-menu-item menu
			 nil
			 name
			 #'(lambda ()
			     (interactive)
			     (fetch-execute-list (list conf)))
			 ))))
    menu))

(defun fetch-execute-select ()
      (interactive)
  (let ((menu (fetch-execute-select-menu t))
	(cnt 0))
    (when menu
      (track-popup-menu menu))))

(defun fetch-execute-all ()
  (interactive)
  (fetch-execute-list *fetch-config-list*))
