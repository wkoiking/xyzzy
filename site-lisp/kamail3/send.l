;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: send.l 793 2008-02-08 20:49:27Z torihat $
;
; kamail3/send.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/send")

(in-package "kamail3")

(defstruct smtpconf
  server
  port
  auth
  user
  pass
  realm)

(defvar *smtp-config-current* nil)

(defvar *send-show-sending-message* t)

(defvar *send-message-save* nil)

(defun send-smtp-select ()
  (unless *smtp-config-list*
    (return-from send-smtp-select))
  (if (= 1 (length *smtp-config-list*))
      (car *smtp-config-list*)
    (let ((server (completing-read "SMTP Server: "
				   (mapcar #'(lambda (x)
					       (smtpconf-server x))
					   *smtp-config-list*)
				   :default (and (smtpconf-p *smtp-config-current*)
						 (smtpconf-server *smtp-config-current*)))))
      (when server
	(smtp-config-get server))
      )))

(defun smtp-config-get (server)
  (car (member server *smtp-config-list*
	       :test #'equal
	       :key  #'smtpconf-server)))

(defun send-buffer-set ()
  (set-buffer (get-buffer-create *buffer-send*))
  (setq need-not-save t))

(defun send-buffer-send ()
  (let* ((smtpconf (send-smtp-select)))
    (unless (smtpconf-p smtpconf)
      (kamail3-error "SMTPİ’è‚ªæ“¾‚Å‚«‚Ü‚¹‚ñB"))
    (junk::smtp-send-buffer (smtpconf-server smtpconf)
			    :port (smtpconf-port smtpconf)
			    :msgid-add t
			    :auth (smtpconf-auth smtpconf)
			    :user (smtpconf-user smtpconf)
			    :pass (smtpconf-pass smtpconf)
			    :realm (smtpconf-realm smtpconf)
			    :show *send-show-sending-message*)
    ))

(defun send-buffer-save-and-clear ()
  (let ((file (fetched-file-store)))
    (when (and file
	       *send-message-save*)
      (fetch-sync (list (list file
			      (attr-folder-string *folder-sent*)
			      nil
			      *attr-status-seen*))))
    (delete-buffer *buffer-send*)))

(defun send-buffer-create (draft)
  (let* ((header (draft-header draft))
	 (body (draft-body draft))
	 (attachments (draft-attachments draft))
	 (content-type (junk::mail-get-header-content "content-type" header))
	 (charset (junk::mail-get-content-charset content-type)))
    (send-buffer-set)
    (erase-buffer (selected-buffer))
    (send-print-header (draft-header draft) charset)
    (if attachments
	(let ((body-header (draft-body-header draft))
	      (boundary (draft-boundary draft)))
	  (send-print-body-part body-header body boundary)
	  (dolist (attachment attachments)
	    (let ((filename (car attachment))
		  (part-header (cdr attachment)))
	      (send-print-attachment part-header filename boundary)))
	  (send-print-boundary boundary t))
      (send-print-body header body))
    ))

(defun send-insert-attachment (file)
  (let ((in (open file :encoding :binary)))
    (unwind-protect
	(insert (si:base64-encode in))
      (close in))))

#|
(defun send-insert-attachment (file)
  (insert-file-noconv file))
|#

(defun send-print-boundary (boundary &optional end-p)
  (insert (format nil "--~A~A~%"
		  boundary
		  (if end-p "--" ""))))

(defun send-print-attachment (header file boundary)
  (let* ((content-type (junk::mail-get-header-content "content-type" header))
	 (charset (junk::mail-get-content-charset content-type))
	 beg)
    (send-print-boundary boundary)
    (send-print-header header charset)
    (setq beg (point))
    (send-insert-attachment file)
    ;(junk::mail-encode-encoding header beg)
    (goto-char (point-max))
    (or (bolp) (insert "\n"))))

(defun send-print-body-part (header body boundary)
  (let* ((content-type (junk::mail-get-header-content "content-type" header))
	 (charset (junk::mail-get-content-charset content-type)))
    (send-print-boundary boundary)
    (send-print-header header charset)
    (send-print-body header body)))

(defun send-print-body (header body)
  (let ((beg (point)))
    (insert body)
    (junk::mail-encode-encoding header beg)
    (goto-char (point-max))
    (or (bolp) (insert "\n"))))

(defun send-print-header (header charset)
  (let ((beg (point)))
    (dolist (h header)
      (let ((field (car h))
	    (value (cdr h)))
	(when value
	  (insert (format nil "~A: ~A~%"
			  (string-capitalize field)
			  (if (listp value)
			      (junk::string-join value ",\n ")
			    value))))))
    (insert "\n")
    (goto-char beg)
    (junk::mail-encode-mime-header charset)
    (goto-char (point-max))))
