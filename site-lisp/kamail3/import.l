;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: import.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/import.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/import")

(in-package "kamail3")

(defvar *import-mh-ignore-folders* "draft trash spam queue")

(defvar *import-mh-folder* *folder-archive*)
(defvar *import-mh-tags*   nil)
(defvar *import-mh-status* *attr-status-seen*)

(defun user::import-mh-folder (dir ignore)
  (interactive "DFolder: \nsIgnore Folders: \n"
    :default0 (si:system-root)
    :default1 *import-mh-ignore-folders*)
  (let ((ignore-folders (mapcar #'(lambda (x)
				    (concat x "/"))
				(split-string ignore " ")))
	folders)
    (dolist (folder (directory dir
			       :absolute nil
			       :directory-only t))
      (unless (member-if #'(lambda (x)
			     (string-matchp x folder)) ignore-folders)
	(push folder folders)))
    (long-operation
      (dolist (folder (reverse folders))
	(kamail3-message "importing ~A ..." folder)
	(import-mh-folder-1 (merge-pathnames folder dir))))
    (kamail3-message "done.")))

(defun import-mh-folder-1 (folder)
  (let ((opt (make-fetchmh :dir folder
			   :folder *import-mh-folder*
			   :tags   *import-mh-tags*
			   :status *import-mh-status*)))
    (fetch-mh-execute opt t)))
