;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: fetch-pop3.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/fetch-pop3.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(require "kamail3/fetch")

(provide "kamail3/fetch-pop3")

(in-package "kamail3")

(defvar *fetch-pop3-type* 'pop3)

(defvar *fetch-pop3-buffer-temp*
  (concat *buffer-prefix* "Fetch POP3*"))

(defstruct fetchpop3
  host
  port
  user
  pass
  auth
  keep)

(defun fetch-pop3-name (pop3)
  (format nil "~S:~A@~A"
          *fetch-pop3-type*
          (fetchpop3-user pop3)
          (fetchpop3-host pop3)))

(defun fetch-pop3-uidl-file (pop3)
  (let ((filename (format nil "pop3-uidl-~A.dat"
                          (si:www-url-encode
                           (format nil "~@[~A~]@~@[~A~]"
                                   (fetchpop3-user pop3)
                                   (fetchpop3-host pop3))
                           nil
                           "0-9a-zA-Z"))))
    (merge-pathnames filename *data-directory*)))

(defun fetch-pop3-uidl-load (pop3)
  (let ((file (fetch-pop3-uidl-file pop3)))
    (when (file-exist-p file)
      (junk::data-read-from-file file))))

(defun fetch-pop3-uidl-save (pop3 uidl-list)
  (let ((file (fetch-pop3-uidl-file pop3)))
    (junk::data-write-to-file file uidl-list)))

(defun fetch-pop3-buffer-set ()
  (set-buffer (get-buffer-create *fetch-pop3-buffer-temp*))
  (erase-buffer (selected-buffer))
  (setq need-not-save t)
  (setq kept-undo-information nil)
  (setq auto-save nil)
  (set-local-window-flags (selected-buffer)
                          *window-flag-line-number* nil)
  (set-local-window-flags (selected-buffer)
                          *window-flag-newline* nil)
  (set-local-window-flags (selected-buffer)
                          *window-flag-eof* nil)
  (set-local-window-flags (selected-buffer)
                          *window-flag-cursor-line* t)
  (set-buffer-fold-width nil))

(defun fetch-pop3-insert-line (num file header)
  (push (cons "_lreal" (map-slash-to-backslash file)) header)
  (summary-print-line num header)
  (refresh-screen)
  (unless (pos-visible-in-window-p (point))
    (recenter -1)))

(defun fetch-pop3-execute (opt)
  (let* ((host (fetchpop3-host opt))
         (port (fetchpop3-port opt))
         (user (fetchpop3-user opt))
         (pass (fetchpop3-pass opt))
         (keep (fetchpop3-keep opt))
         (auth (fetchpop3-auth opt))
         (uidl-old (fetch-pop3-uidl-load opt))
         uidl-list
         uidl-new
         stream
         apop-key
         return
         del-list
         (del-count 0)
         keep-timestamp
         fetched
         )
    (when (numberp keep)
      (setq keep-timestamp (- (get-universal-time) (* keep 24 60 60))))
    (multiple-value-setq (stream apop-key)
      (junk::pop3-open host port))
    (handler-case
        (unwind-protect
            (progn
              (setq return (cond ((equal auth "apop")
                                  (junk::pop3-apop stream apop-key user pass))
                                 ((equal auth "cram-md5")
                                  (junk::pop3-auth-cram-md5 stream user pass))
                                 (t
                                  (and (junk::pop3-user stream user)
                                       (junk::pop3-pass stream pass)))))
              (unless return
                (kamail3-error "login ~S error." auth))
              (setq uidl-list (junk::pop3-uidl stream))
              (when uidl-list
                (save-excursion
                  (fetch-pop3-buffer-set)
                  (dolist (u uidl-list)
                    (let ((num  (car u))
                          (uidl (cdr u))
                          timestamp)
                      (if (setq timestamp (cdr (assoc uidl uidl-old :test #'equal)))
                          (push (cons uidl timestamp) uidl-new)
                        (progn
                          (multiple-value-bind (file header ts)
                              (fetch-pop3-get stream num)
                            (when file
                              (let ((ut (get-universal-time)))
                                (setq timestamp (if (and ts
                                                         (< 0 ts))
                                                    (min ts ut) ; ñ¢óàÇÃDateÇÕåªç›
                                                  ut)) ; DateÇ™Ç»Ç¢èÍçáÇÕåªç›
                                (push (cons file header) fetched)
                                (push (cons uidl timestamp) uidl-new)
                                (fetch-pop3-insert-line num file header)
                                )))))
                      (if keep-timestamp
                          (when (< timestamp keep-timestamp)
                            (push num del-list))
                        (unless keep
                          (push num del-list)))
                      (fetch-pop3-uidl-save opt uidl-new)
                      ))
                  (when del-list
                    (dolist (num (sort del-list #'>)) ; ëÂÇ´Ç¢ÇŸÇ§Ç©ÇÁè¡Ç∑
                      (when (junk::pop3-dele stream num)
                        (incf del-count)))
                    (kamail3-message "~D message~P deleted." del-count del-count))
                  )
                (delete-buffer *fetch-pop3-buffer-temp*)
                ))
          (junk::pop3-quit stream))
      (error (c)
        (kamail3-msgbox "fetch error: ~@[~A~]"
                        (si:*condition-string c))))
    (when fetched
      (fetch-pop3-sync (reverse fetched)))
    ;    (when fetched
    ;      (fetch-sync (mapcar #'car (reverse fetched)))
    ;      (fetch-pop3-refile (reverse fetched)))
    (mapcar #'car (reverse fetched))))

(defun fetch-pop3-get (stream num)
  (let (header file date timestamp)
    (save-excursion
      (fetch-buffer-set)
      (when (junk::pop3-retr-to-buffer stream num)
        (setq file (fetched-file-store))
        (goto-char (point-min))
        (junk::mail-decode-mime-header)
        (setq header (junk::mail-parse-header))
        (setq date (junk::mail-get-header-value "date" header))
        (when date
          (when (listp date)
            (setq date (car date)))
          (setq timestamp (junk::rfc822-parse-date date)))))
    (values file header timestamp)))

(defun fetch-pop3-sync (fetched)
  (let (fetched-files)
    (dolist (f fetched)
      (let* ((file (car f))
             (header (cdr f))
             (rules (refile-rule-check header))
             folder
             tags
             status)
        (dolist (rule rules)
          (let ((rfolder (rule-folder rule))
                (rtag    (rule-tag rule))
                (rstatus (rule-status rule)))
            (when rfolder
              (setq folder (attr-folder-string rfolder)))
            (when rtag
              (pushnew rtag tags :test #'equal))
            (when rstatus
              (setq status rstatus))))
        (push (list file
                    (or folder (attr-folder-string *folder-inbox*))
                    (if tags (attr-tags-join tags) "")
                    (or status *attr-status-unseen*)) fetched-files)
        ))
    (fetch-sync (reverse fetched-files))
    ))

(defun fetch-pop3-refile (fetched)
  (let (cmd-list)
    (dolist (f fetched)
      (let* ((file (car f))
             (header (cdr f))
             (rules (refile-rule-check header)))
        (dolist (rule rules)
          (let ((folder (rule-folder rule))
                (tag    (rule-tag rule))
                (status (rule-status rule)))
            (when folder
              (push (attr-folder-update-cmd file folder) cmd-list))
            (when tag
              (push (attr-tag-update-cmd file tag) cmd-list))
            (when status
              (push (attr-status-update-cmd file tag) cmd-list))
            ))))
    (when cmd-list
      (kamail3-message "applying refile rules ...")
      (he-execute-batch (reverse cmd-list))
      (kamail3-message "applying refile rules ... done."))
    ))

;;; regist fetch type
(fetch-type-register
 (make-fetchconf :type *fetch-pop3-type*
                 :name 'fetch-pop3-name
                 :execute 'fetch-pop3-execute
                 ))
