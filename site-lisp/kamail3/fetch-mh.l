;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: fetch-mh.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/fetch-mh.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(require "kamail3/fetch")

(provide "kamail3/fetch-mh")

(in-package "kamail3")

(defvar *fetch-mh-type* 'mh)

(defstruct fetchmh
  dir
  folder
  tags
  status)

(defun fetch-mh-name (opt)
  (let ((dir (fetchmh-dir opt)))
    (format nil "~S:~A" *fetch-mh-type* dir)))

(defun fetch-mh-execute (opt &optional copy-p)
  (let ((dir (fetchmh-dir opt))
	(folder (fetchmh-folder opt))
	(tags (fetchmh-tags opt))
	(status (fetchmh-status opt))
	files
	fetched)
    (unless (and dir
		 (file-exist-p dir)
		 (file-directory-p dir))
      (kamail3-error "directory not exist: ~S" dir))
    ;;; listup files
    (kamail3-message "parsing ~A ..." dir)
    (setq files (fetch-mh-listup-files dir))
    (kamail3-message "parsing ~A ... done." dir)
    (unless files
      (kamail3-message "no file to fetch.")
      (return-from fetch-mh-execute))
    ;;; fetch files
    (kamail3-message "fetching ...")
    (dolist (file files)
      (let ((newfile (fetch-mh-file file copy-p)))
	(when newfile
	  (push newfile fetched)))
      (do-events))
    (kamail3-message "fetching ... done.")
    (when fetched
      (fetch-sync (mapcar #'(lambda (x)
			      (list x
				    (or folder (attr-folder-string *folder-inbox*))
				    (if tags (attr-tags-join tags) "")
				    (or status *attr-status-unseen*)))
			  (reverse fetched))))
    (nreverse fetched)))

(defun fetch-mh-listup-files (dir)
  (let (files)
    (dolist (info (sort (directory dir
				   :absolute t
				   :file-info t)
			#'(lambda (a b)
			    (> (caddr a) (caddr b)))))
      (let ((file (car info)))
	(when (and file
		   (string-match "/[0-9]+$" file))
	  (push file files))))
    files))

(defun fetch-mh-file (file &optional copy-p)
  (let ((newfile (fetch-file-prepare-next)))
    (and (if copy-p
	     (copy-file file newfile)
	   (rename-file file newfile))
	 newfile)))

;;; regist fetch type
(fetch-type-register
 (make-fetchconf :type *fetch-mh-type*
		 :name 'fetch-mh-name
		 :execute 'fetch-mh-execute
		 ))
