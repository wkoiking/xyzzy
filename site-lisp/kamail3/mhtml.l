;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: mhtml.l 721 2006-08-29 15:09:35Z torihat $
;
; kamail3/mhtml.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/mhtml")

(in-package "kamail3")

(defvar *mhtml-browser-position* bx::XPIS_BOTTOM)
(defvar *mhtml-browser-height* 400)

(defvar *mhtml-temp-file*
  (merge-pathnames "mhtml-temp.mht" *data-directory*))

(define-key *summary-map* #\,            'summary-view-html-mail-or-scroll)
(define-key *summary-map* #\q            'mhtml-close-browser-if)

(defvar *mhtml-browser* nil)

(defvar *mhtml-temp-directory*
  (merge-pathnames "mhtml-temp" *data-directory*))

(defun mhtml-extract-parts (structure)
  (let ((header (junk::mailstructure-headers structure))
	html-part
	cid-parts)
    (if (junk::mail-multipart-p header)
	(dolist (child (junk::mailstructure-structure structure))
	  (multiple-value-bind (h c) (mhtml-extract-parts child)
	    (when (and h (null html-part))
	      (setq html-part h))
	    (setq cid-parts (nconc cid-parts c))))
      (let* ((content-type (junk::mail-get-header-content "content-type" header))
	     (mime-type (junk::mail-get-content-mime-type content-type))
	     (cid (junk::mail-get-header-value "content-id" header)))
	(cond ((equalp mime-type "text/html")
	       (setq html-part structure))
	      (cid
	       (push (cons (string-trim " \t\r\n<>" (if (listp cid) (car cid) cid))
			   structure)
		     cid-parts)))))
    (values html-part cid-parts)))

(defun mhtml-part-extension (header)
  (let* ((filename (junk::mail-get-filename header))
	 (content-type (junk::mail-get-header-content "content-type" header))
	 (mime-type (junk::mail-get-content-mime-type content-type)))
    (cond ((and (stringp filename)
		(string-match "\\(\\.[a-zA-Z0-9]+\\)$" filename))
	   (match-string 1))
	  ((equalp mime-type "image/png") ".png")
	  ((or (equalp mime-type "image/jpeg") (equalp mime-type "image/jpg")) ".jpg")
	  ((equalp mime-type "image/gif") ".gif")
	  ((equalp mime-type "image/webp") ".webp")
	  ((equalp mime-type "image/svg+xml") ".svg")
	  (t ".bin"))))

(defun mhtml-save-part-raw (structure file)
  (let* ((header (junk::mailstructure-headers structure))
	 (beg (junk::mailstructure-beg structure))
	 (end (junk::mailstructure-end structure))
	 (encoding (junk::mail-get-transfer-encoding header)))
    (save-excursion
      (source-buffer-set)
      (cond ((equalp encoding "quoted-printable")
	     (quoted-printable-decode-region-to-file file beg end))
	    ((equalp encoding "base64")
	     (base64-decode-region-to-file file beg end))
	    (t
	     (write-region beg end file nil *encoding-binary* *eol-lf*))))))

(defun mhtml-build-html (file)
  (let (structure)
    (save-excursion
      (source-buffer-set)
      (erase-buffer (selected-buffer))
      (insert-file-noconv file)
      (setq structure (junk::mail-parse)))
    (multiple-value-bind (html-part cid-parts)
	(mhtml-extract-parts structure)
      (unless html-part
	(return-from mhtml-build-html nil))
      (create-directory-if *mhtml-temp-directory*)
      (dolist (old (directory *mhtml-temp-directory* :file-only t :absolute t))
	(delete-file old))
      (let ((html-file (merge-pathnames "message.html" *mhtml-temp-directory*))
	    (cid-map nil)
	    (idx 0)
	    (charset (junk::mail-get-content-charset
		      (junk::mail-get-header-content "content-type"
			(junk::mailstructure-headers html-part)))))
	(dolist (pair cid-parts)
	  (let* ((cid (car pair))
		 (part (cdr pair))
		 (ext (mhtml-part-extension (junk::mailstructure-headers part)))
		 (fname (format nil "cid_~D~A" idx ext))
		 (fpath (merge-pathnames fname *mhtml-temp-directory*)))
	    (mhtml-save-part-raw part fpath)
	    (push (cons cid fname) cid-map)
	    (incf idx)))
	(mhtml-save-part-raw html-part html-file)
	(let ((tmp (create-new-buffer " *kamail3-mhtml-tmp*")))
	  (unwind-protect
	      (save-excursion
		(set-buffer tmp)
		(let ((ed::*expected-fileio-encoding* *encoding-binary*))
		  (declare (special ed::*expected-fileio-encoding*))
		  (insert-file-contents html-file))
		(dolist (pair cid-map)
		  (goto-char (point-min))
		  (replace-buffer (concat "cid:" (car pair)) (cdr pair) :case-fold t))
		(when (and charset (plusp (length charset)))
		  (goto-char (point-min))
		  (unless (scan-buffer "charset=" :case-fold t)
		    (insert (format nil "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=~A\">\n" charset))))
		(write-region (point-min) (point-max) html-file nil *encoding-binary* *eol-lf*))
	    (delete-buffer tmp)))
	html-file))))

(defun mhtml-view (file &optional shell-exec)
  (let ((html-file (mhtml-build-html file)))
    (if html-file
	(if shell-exec
	    (shell-execute (map-slash-to-backslash html-file))
	  (mhtml-view-html-mail html-file))
      (kamail3-message "HTML part not found."))))

(defun mhtml-find-browser ()
  (or *mhtml-browser*
      (find-if #'file-exist-p
	       (list "C:/Program Files/Google/Chrome/Application/chrome.exe"
		     "C:/Program Files (x86)/Google/Chrome/Application/chrome.exe"
		     (merge-pathnames "Google/Chrome/Application/chrome.exe"
				      (or (si:getenv "LOCALAPPDATA") ""))
		     "C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe"
		     "C:/Program Files/Microsoft/Edge/Application/msedge.exe"))))

(defun summary-view-html-mail-or-scroll (&optional shell-exec)
  (interactive "p")
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (file (summary-message-file header)))
    (unless file
      (return-from summary-view-html-mail-or-scroll))
    (mhtml-view file shell-exec)))

; (defun summary-view-html-mail-or-scroll (&optional shell-exec)
;   (interactive "p")
;   (let* ((msg (summary-message-current))
; 	 (num (car msg))
; 	 (header (cdr msg))
; 	 (file (summary-message-file header)))
;     (unless file
;       (return-from summary-view-html-mail-or-scroll))
;     (when (message-window-get)
;       (message-close))
;     (if (mhtml-browser-open-p)
; 	(bx:browserex-scroll-down)
;       (mhtml-view file shell-exec))))

(defun mhtml-browser-open-p ()
  (bx::browser-open-p))

(defun mhtml-view-html-mail (file)
  (let ((browser (mhtml-find-browser))
	(path (map-slash-to-backslash file)))
    (if browser
	(shell-execute (map-slash-to-backslash browser) nil (format nil "\"~A\"" path))
      (shell-execute path nil))))

; (defun mhtml-view-html-mail (file)
;   (bx::navigate file
; 		:position *mhtml-browser-position*
; 		:size *mhtml-browser-height*))

(defun mhtml-close-browser-if ()
  (interactive)
  (if (mhtml-browser-open-p)
      (mhtml-close-browser)
    (summary-close)))

(defun mhtml-close-browser ()
  (bx::close-browser))
