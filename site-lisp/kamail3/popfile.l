;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: popfile.l 722 2006-08-30 16:45:53Z torihat $
;
; kamail3/popfile.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs")
  (require "xmlrpcm/popfile"))

(provide "kamail3/popfile")

(in-package "kamail3")

(defvar *popfile-url* "http://localhost:8081/RPC2")
(defvar *popfile-username* "admin")
(defvar *popfile-password* "")

(defvar *popfile-header-classification* "x-text-classification")

(defvar *popfile-last-bucket* nil)

(define-key *summary-map* #\z  'summary-popfile-select-bucket)

(defun summary-popfile-header-update-cmd (file bucket)
  (he-cmd-edit file
	       *popfile-header-classification*
	       bucket))

(defun summary-popfile-header-update (file bucket)
  (let ((draft (fetch-draft-file (fetch-file-number file))))
    (if (file-exist-p draft)
	(attr-draft-update draft *popfile-header-classification* bucket)
      (he-execute (summary-popfile-header-update-cmd file bucket)))))

(defun popfile-get-session-key ()
  (xmlrpcm::popfile-get-session-key *popfile-url*
				    *popfile-username*
				    *popfile-password*))

(defun summary-popfile-select-bucket ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (bucket-current (junk::mail-get-header-value *popfile-header-classification*
						      header))
	 (file (summary-message-file header)))
    (when file
      (setq *popfile-last-bucket* nil)
      (let ((sesskey (popfile-get-session-key))
	    buckets)
	(unless sesskey
	  (kamail3-error "Session Keyを取得できません。"))
	(unwind-protect
	    (when (setq buckets (xmlrpcm::popfile-get-buckets *popfile-url*
							      sesskey))
	      (let ((menu (create-popup-menu)))
		(dolist (b buckets)
		  (let ((bucket b)
			(n num)
			(filename file)
			(cur-p (equal b bucket-current)))
		    (add-menu-item menu
				   bucket
				   bucket
				   #'(lambda ()
				       (interactive)
				       (summary-popfile-add-message-to-bucket n
									      sesskey
									      bucket
									      filename
									      bucket-current))
				   #'(lambda ()
				       (when cur-p :check)))))
		(track-popup-menu menu)))
	  (xmlrpcm::popfile-release-session-key *popfile-url*
						sesskey)))
      (when *popfile-last-bucket*
	(message "Bucket set: ~A" *popfile-last-bucket*))
      )))

(defun summary-popfile-add-message-to-bucket (num sesskey bucket filename &optional bucket-current)
  (interactive)
  (when (equal bucket bucket-current)
    (return-from summary-popfile-add-message-to-bucket))
  (let ((filepath (map-slash-to-backslash filename))
	result)
    (unless sesskey
      (kamail3-error "Session Keyを取得できません。"))
    (setq result
	  (progn
	    (and bucket-current
		 (xmlrpcm::popfile-remove-message-from-bucket *popfile-url*
							      sesskey
							      bucket
							      filepath))
	    (xmlrpcm::popfile-add-message-to-bucket *popfile-url*
						    sesskey
						    bucket
						    filepath)))
    (if result
	(let* ((msg (summary-get-header num))
	       (header (cdr msg)))
	  (setq header (attr-set-value header *popfile-header-classification* bucket))
	  (summary-set-header num header)
	  (summary-popfile-header-update filename bucket)
	  (setq *popfile-last-bucket* bucket))
      (kamail3-error "エラーが発生しました。"))))
