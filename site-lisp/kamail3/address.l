;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: address.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/address.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/address")

(in-package "kamail3")

(defvar *address-file*
  (merge-pathnames "address-book.dat" *data-directory*))

(defvar *address-list* nil)
(defvar *address-group-list* nil)
(defvar *address-group-default* "default")

(defstruct address
  email
  name
  group)

(defun address-load ()
  (when (file-exist-p *address-file*)
    (setq *address-list*
	  (junk::data-read-from-file *address-file*))
    (let (group-list)
      (dolist (a *address-list*)
	(pushnew (address-group a) group-list :test #'equal))
      (setq *address-group-list* (reverse group-list)))))

(defun address-save ()
  (junk::data-write-to-file *address-file* *address-list*))

(defun address-get (email)
  (car (member email *address-list*
	       :test #'equal
	       :key #'address-email)))

(defun address-complete (&optional prompt default)
  (completing-read (or prompt "Address: ")
		   (mapcar #'address-email *address-list*)
		   :default default
		   :must-match nil))

(defun address-group-complete ()
  (completing-read "Address Group: "
		   *address-group-list*
		   :must-match nil))

(defun address-push (email name group)
  (let ((address (make-address :email email
			       :name  name
			       :group group)))
    (setq *address-list*
	  (append *address-list* (list address)))
    (address-save)))

(defun address-delete (email)
  (setq *address-list*
	(delete email *address-list*
		:test #'equal
		:key #'address-email))
  (address-save))

(defun address-add (&optional email name group)
  (interactive)
  (unless email
    (setq email (read-string "E-Mail: ")))
  (unless name
    (setq name (read-string "Name: ")))
  (unless group
    (setq group (address-group-complete)))
  (address-push email name group))

(defun address-del (&optional email)
  (interactive)
  (unless email
    (setq email (address-complete)))
  (address-delete email))

(defvar *address-dialog-group-string* "■ ")
(defvar *address-dialog-result-list* nil)

(defun address-group-addresses (group)
  (let (address-list)
    (mapc #'(lambda (x)
	      (when (equal group (address-group x))
		(push x address-list)))
	  *address-list*)
    (nreverse address-list)))

(defun address-dialog ()
  (let (mail-list
	result-list)
    (setq *address-dialog-result-list* nil)
    (dolist (g *address-group-list*)
      (let ((addrs (address-group-addresses g)))
	(when addrs
	  ; グループ名
	  (push (list (concat *address-dialog-group-string* g) "") mail-list)
	  ; 中味
	  (dolist (a addrs)
	    (let ((name (address-name a))
		  (addr (address-email a)))
	      (push (list (or name "") addr) mail-list))))
	))
    ; ダイアログ作成
    (multiple-value-bind (result data)
	(dialog-box `(dialog 0 0 222 237
			     (:caption "アドレス帳")
			     (:font 9 "ＭＳ Ｐゴシック")
			     (:control
			      (:listbox name-list nil #x50b10911 7 7 206 202)
			      (:button IDOK "OK" #x50010001 106 216 50 14)
			      (:button IDCANCEL "ｷｬﾝｾﾙ" #x50010000 164 216 50 14)))
		    (list (cons 'name-list
				(mapcar #'(lambda (x) x) (reverse mail-list))))
		    '((name-list :column (24 40) :must-match t :enable (IDOK))))
      (when result
	(dolist (current (cdar data))
	  (let ((name (car current))
		(addr (cadr current)))
	    ; グループを選択した場合はグループの全アドレスを追加
	    (if (string-match (concat "^" *address-dialog-group-string*) name)
		(let ((group (substring name
					(length *address-dialog-group-string*)
					(length name))))
		  (setq result-list (address-group-addresses group)))
	      (unless (string= "" addr)
		(setq result-list (list (address-get addr)))))))))
    (setq *address-dialog-result-list* result-list)))
