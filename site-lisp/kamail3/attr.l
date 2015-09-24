;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: attr.l 722 2006-08-30 16:45:53Z torihat $
;
; kamail3/attr.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/attr")

(in-package "kamail3")

(defun attr-get-value (header key)
  (junk::mail-get-header-value key header))

(defun attr-set-value (header key val)
  (setq val (or val ""))
  (if (attr-get-value header key)
      (setf (junk::mail-get-header-value key header) val)
    (push (cons key val) header))
  header)

;;;
;;; Update Draft
;;;
(defun attr-draft-update (draft key value)
  (let ((result nil)
	(ed::*expected-fileio-encoding* *encoding-sjis*))
    (declare (special ed::*expected-fileio-encoding*))
    (save-excursion
      (fetch-buffer-temp-set)
      (insert-file-contents draft)
      (map-utf-8-region (point-min) (point-max))
      (goto-char (point-min))
      (unless (scan-buffer "^$" :regexp t)
	(goto-char (point-max)))
      (save-restriction
	(narrow-to-region (point-min) (point))
	(goto-char (point-min))
	(when (scan-buffer (format nil "^~A=" key)
			   :regexp t
			   :tail t)
	  (and value (insert value))
	  (delete-region (point)
			 (progn
			   (goto-eol)
			   (point)))
	  (setq result t)))
      (when result
	(write-region (point-min) (point-max) draft nil *encoding-utf8n* *eol-lf*)))
    (fetch-buffer-temp-delete)
    result))

;;; status

(defun attr-status-update-cmd (file status)
  (he-cmd-edit file
	       *header-status*
	       status))

(defun attr-status-update (file status)
  (let ((draft (fetch-draft-file (fetch-file-number file))))
    (if (file-exist-p draft)
	(attr-draft-update draft *header-status* status)
      (he-execute (attr-status-update-cmd file status)))))

(defun attr-status-get (header)
  (attr-get-value header *header-status*))

(defun attr-status-set (header status)
  (attr-set-value header *header-status* status))

(defun attr-status-exist (status test)
  (if status
      (string-match test status)
    nil))

(defun attr-status-seen (status)
  (attr-status-exist status *attr-status-seen*))

(defun attr-status-forwarded (status)
  (attr-status-exist status *attr-status-forwarded*))

(defun attr-status-answered (status)
  (attr-status-exist status *attr-status-answered*))

(defun attr-status-add (status add)
  (unless (attr-status-exist status add)
    (concat status add)))

(defun attr-status-add-seen (status)
  (attr-status-add status *attr-status-seen*))

(defun attr-status-add-forwarded (status)
  (attr-status-add status *attr-status-forwarded*))

(defun attr-status-add-answered (status)
  (attr-status-add status *attr-status-answered*))

(defun attr-status-delete (status delete)
  (if status
      (substitute-string status delete "")
    status))

(defun attr-status-delete-seen (status)
  (attr-status-delete status *attr-status-seen*))

(defun attr-status-delete-forwarded (status)
  (attr-status-delete status *attr-status-forwarded*))

(defun attr-status-delete-answered (status)
  (attr-status-delete status *attr-status-answered*))

;;; tag

(defconstant *attr-tag-separator* "|")

(defun attr-tag-update-cmd (file tag)
  (he-cmd-edit file
	       *header-tag*
	       tag))

(defun attr-tag-input (&optional tag prompt)
  (completing-read (or prompt "Tag: ")
		   *attr-tags-list*
		   :default tag))

(defun attr-tag-update (file tag)
  (let ((draft (fetch-draft-file (fetch-file-number file))))
    (if (file-exist-p draft)
	(attr-draft-update draft *header-tag* tag)
      (he-execute (attr-tag-update-cmd file tag)))))

(defun attr-tags-load ()
  (when (file-exist-p *attr-tags-list-file*)
    (setq *attr-tags-list*
	  (junk::data-read-from-file *attr-tags-list-file*))))

(defun attr-tags-save ()
  (junk::data-write-to-file *attr-tags-list-file* *attr-tags-list*))

(defun attr-tags-get (header)
  (let ((tag (attr-get-value header *header-tag*)))
    (if tag
	(attr-tags-split tag)
      nil)))

(defun attr-tags-set (header tags)
  (let ((tag (attr-tags-join tags)))
    (attr-tag-set header tag)))

(defun attr-tags-join (tags)
  (let ((tmp (junk::string-join tags *attr-tag-separator*)))
    (when tmp
      (concat *attr-tag-separator*
	      tmp
	      *attr-tag-separator*))))

(defun attr-tags-split (tag)
  (split-string tag *attr-tag-separator*))

(defun attr-tag-set (header tag)
  (attr-set-value header *header-tag* tag))

(defun attr-tag-exist (tag tags)
  (member tag tags :test #'equal))

(defun attr-tag-regist (tag)
  (unless (attr-tag-exist tag *attr-tags-list*)
    (setq *attr-tags-list*
	  (attr-tag-add *attr-tags-list* tag))
    (attr-tags-save)))

(defun attr-tag-add (tags tag)
  (pushnew tag tags :test #'equal))

(defun attr-tag-del (tags tag)
  (delete tag tags :test #'equal))

(defun attr-tag-query (tag)
  (format nil "~A STRINC ~A~A~A"
	  *header-tag*
	  *attr-tag-separator*
	  tag
	  *attr-tag-separator*))

;;; folder

(defvar *attr-folder-prefix-string* "km3:")

(defun attr-folder-update-cmd (file folder)
  (he-cmd-edit file
	       *header-folder*
	       (attr-folder-string folder)))

(defun attr-folder-update-batch (file-list)
  (let (cmd-list)
    (dolist (f file-list)
      (let* ((file (car f))
	     (folder (cdr f))
	     (draft (fetch-draft-file (fetch-file-number file))))
	(if (file-exist-p draft)
	    (attr-draft-update draft *header-folder* (attr-folder-string folder))
	  (push (attr-folder-update-cmd file folder) cmd-list))
	(do-events)))
    (when cmd-list
      (he-execute-batch cmd-list))
    ))

(defun attr-folder-update (file folder)
  (let ((draft (fetch-draft-file (fetch-file-number file))))
    (if (file-exist-p draft)
	(attr-draft-update draft *header-folder* (attr-folder-string folder))
      (he-execute (attr-folder-update-cmd file folder)))))

(defun attr-folder-string (folder)
  (if (or (not (stringp folder))
	  (= (length folder) 0))
      nil
    (concat *attr-folder-prefix-string* folder)))

(defun attr-folder-name (str)
  (when (and (stringp str)
	     (string-match (concat "^" *attr-folder-prefix-string*) str))
    (substring str (match-end 0) (length str))))

(defun attr-folder-get (header)
  (let ((str (attr-get-value header *header-folder*)))
    (attr-folder-name str)))

(defun attr-folder-set (header folder)
  (let ((str (attr-folder-string folder)))
    (attr-set-value header *header-folder* str)))
