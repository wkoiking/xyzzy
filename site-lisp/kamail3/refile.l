;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: refile.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/refile.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/refile")

(in-package "kamail3")

(defvar *refile-rule-list* nil)

(defstruct rule
  conditions
  folder
  tag
  status)

#|
(defun *refile-rule-file*
  (merge-pathnames "refile-rule.dat" *data-directory*))

(defun refile-rule-load ()
  (when (file-exist-p *refile-rule-file*)
    (setq *refile-rule-list*
	  (junk::data-read-from-file *refile-rule-file*))))

(defun refile-rule-save ()
  (junk::data-write-to-file *refile-rule-file* *refile-rule-list*))
|#

(defun refile-rule-check-conditions (header rule)
  (unless (rule-conditions rule)
    (return-from refile-rule-check-conditions))
  (dolist (cond (rule-conditions rule))
    (let* ((field (car cond))
	   (regex (cdr cond))
	   (value (junk::mail-get-header-value field header)))
      (unless (and (stringp value)
		   (string-match regex value))
	(return-from refile-rule-check-conditions))))
  t)

(defun refile-rule-check (header)
  (let (rules)
    (dolist (rule *refile-rule-list*)
      (when (refile-rule-check-conditions header rule)
	;(msgbox "~S~%~%~{~S~%~}" rule header)
	(push rule rules)))
    (when *debug-mode*
      (kamail3-msgbox "~{~S~%~}~%~{~S~%~}" header (reverse rules)))
    (nreverse rules)))
