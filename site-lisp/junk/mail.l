;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: mail.l 769 2007-12-14 19:18:39Z torihat $
;
; mail.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "junk/defs")
  (require "junk/macro"))

(require "encdec")

(require "junk/lib")
(require "junk/rfc822")
(require "junk/rfc2045")

(provide "junk/mail")

(in-package "junk")

(require "foreign")

(c:define-dll-entry c:int MultiByteToWideChar
  (c:int c:int (c:void *) c:int (c:void *) c:int)
  "kernel32")

(c:define-dll-entry c:int WideCharToMultiByte
  (c:int c:int (c:void *) c:int (c:void *) c:int c:int c:int)
  "kernel32")

(c:define-dll-entry c:int fopen
  ((c:char *) (c:char *))
  "msvcrt")

(c:define-dll-entry c:int fseek
  (c:int c:int c:int)
  "msvcrt")

(c:define-dll-entry c:int ftell
  (c:int)
  "msvcrt")

(c:define-dll-entry c:int fread
  ((c:void *) c:int c:int c:int)
  "msvcrt")

(c:define-dll-entry c:int fwrite
  ((c:void *) c:int c:int c:int)
  "msvcrt")

(c:define-dll-entry c:int fclose
  (c:int)
  "msvcrt")

(defun mail-convert-file-cp-to-utf8 (in-file out-file cp)
  (let* ((in-path (si:make-string-chunk (map-slash-to-backslash in-file)))
	 (out-path (si:make-string-chunk (map-slash-to-backslash out-file)))
	 (mode-rb (si:make-string-chunk "rb"))
	 (mode-wb (si:make-string-chunk "wb"))
	 (fp (fopen in-path mode-rb)))
    (when (zerop fp)
      (return-from mail-convert-file-cp-to-utf8 nil))
    (let (src read-bytes)
      (unwind-protect
	  (progn
	    (fseek fp 0 2)
	    (let ((size (ftell fp)))
	      (fseek fp 0 0)
	      (when (<= size 0)
		(let ((out-fp (fopen out-path mode-wb)))
		  (unless (zerop out-fp)
		    (fclose out-fp)
		    (return-from mail-convert-file-cp-to-utf8 t)))
		(return-from mail-convert-file-cp-to-utf8 nil))
	      (setq src (si:make-chunk nil size))
	      (setq read-bytes (fread src 1 size fp))))
	(fclose fp))
      (when (and src (plusp read-bytes))
	(let ((wlen (MultiByteToWideChar cp 0 src read-bytes 0 0)))
	  (when (plusp wlen)
	    (let* ((dst (si:make-chunk nil (* wlen 2)))
		   (actual (MultiByteToWideChar cp 0 src read-bytes dst wlen))
		   (u8len (WideCharToMultiByte 65001 0 dst actual 0 0 0 0)))
	      (when (plusp u8len)
		(let* ((out (si:make-chunk nil u8len))
		       (u8actual (WideCharToMultiByte 65001 0 dst actual out u8len 0 0))
		       (out-fp (fopen out-path mode-wb)))
		  (unless (zerop out-fp)
		    (unwind-protect
			(fwrite out 1 u8actual out-fp)
		      (fclose out-fp))
		    (return-from mail-convert-file-cp-to-utf8 t))))))))
      nil)))

(defvar *mail-windows-codepage-alist*
  '(("gb2312"         . 936)
    ("csgb2312"       . 936)
    ("gbk"            . 936)
    ("cp936"          . 936)
    ("windows-936"    . 936)
    ("gb18030"        . 54936)
    ("euc-kr"         . 949)
    ("cseuckr"        . 949)
    ("ks_c_5601-1987" . 949)
    ("ksc5601"        . 949)
    ("korean"         . 949)
    ("cp949"          . 949)
    ("windows-949"    . 949)
    ("uhc"            . 949)
    ("big5"           . 950)
    ("cp950"          . 950)))

(defun mail-windows-codepage (charset)
  (and (stringp charset)
       (cdr (assoc (string-downcase (string-trim " \t\"" charset))
		   *mail-windows-codepage-alist*
		   :test #'string=))))

; (defun mail-cp-bytes-to-string (bytes cp)
;   (let ((len (length bytes)))
;     (when (zerop len)
;       (return-from mail-cp-bytes-to-string ""))
;     (let ((src (si:make-chunk nil len)))
;       (dotimes (i len)
; 	(si:pack-uint8 src i (aref bytes i)))
;       (let ((wlen (MultiByteToWideChar cp 0 src len 0 0)))
; 	(if (<= wlen 0)
; 	    ""
; 	  (let ((dst (si:make-chunk nil (* wlen 2))))
; 	    (let ((actual (MultiByteToWideChar cp 0 src len dst wlen)))
; 	      (with-output-to-string (s)
; 		(dotimes (i actual)
; 		  (let* ((u (si:unpack-uint16 dst (* i 2)))
; 			 (c (unicode-char u)))
; 		    (write-char (or c #\?) s)))))))))))

(defun mail-cp-bytes-to-string (bytes cp)
  (let ((len (length bytes)))
    (when (zerop len)
      (return-from mail-cp-bytes-to-string ""))
    (let ((src (si:make-chunk nil len)))
      (dotimes (i len)
	(si:pack-uint8 src i (aref bytes i)))
      (let ((wlen (MultiByteToWideChar cp 0 src len 0 0)))
	(if (<= wlen 0)
	    ""
	  (let ((dst (si:make-chunk nil (* wlen 2))))
	    (let ((actual (MultiByteToWideChar cp 0 src len dst wlen)))
	      (with-output-to-string (s)
		(dotimes (i actual)
		  (let* ((u (si:unpack-uint16 dst (* i 2)))
			 (c (unicode-char u)))
		    (cond ((characterp c) (write-char c s))
			  ((stringp c) (princ c s))
			  (t (write-char #\? s)))))))))))))

(defun mail-decode-b-to-bytes (str)
  (let ((table "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")
	(buf 0)
	(bits 0)
	(out nil))
    (dotimes (i (length str))
      (let* ((ch (char str i))
	     (pos (position ch table)))
	(when pos
	  (setq buf (logior (ash buf 6) pos))
	  (incf bits 6)
	  (when (>= bits 8)
	    (decf bits 8)
	    (push (logand (ash buf (- bits)) #xff) out)))))
    (coerce (nreverse out) 'vector)))

(defun mail-decode-q-to-bytes (str)
  (let ((len (length str))
	(i 0)
	(out nil))
    (while (< i len)
      (let ((ch (char str i)))
	(cond ((char= ch #\_)
	       (push 32 out)
	       (incf i))
	      ((and (char= ch #\=)
		    (<= (+ i 3) len)
		    (string-match "^[0-9a-fA-F]\\{2\\}$" (subseq str (1+ i) (+ i 3))))
	       (push (parse-integer str :start (1+ i) :end (+ i 3) :radix 16) out)
	       (incf i 3))
	      (t
	       (push (logand (char-code ch) #xff) out)
	       (incf i)))))
    (coerce (nreverse out) 'vector)))

(defun mail-decode-cp-mime-words ()
  (goto-char (point-min))
  (let ((re "=\\?\\([^? \t\r\n]+\\)\\?\\([BbQq]\\)\\?\\([^? \t\r\n]*\\)\\?="))
    (while (scan-buffer re :regexp t)
      (let* ((beg (match-beginning 0))
	     (end (match-end 0))
	     (cp (mail-windows-codepage (match-string 1)))
	     (enc (string-upcase (match-string 2)))
	     (payload (match-string 3)))
	(if (not cp)
	    (goto-char end)
	  (let ((chunks (list (if (string= enc "B")
				  (mail-decode-b-to-bytes payload)
				(mail-decode-q-to-bytes payload)))))
	    (goto-char end)
	    ;; 同じコードページの連続する encoded-word はマルチバイト分断防止のため結合してから変換
	    (while (and (looking-at re)
			(eql (mail-windows-codepage (match-string 1)) cp))
	      (let ((next-enc (string-upcase (match-string 2)))
		    (next-payload (match-string 3)))
		(push (if (string= next-enc "B")
			  (mail-decode-b-to-bytes next-payload)
			(mail-decode-q-to-bytes next-payload))
		      chunks)
		(setq end (match-end 0))
		(goto-char end)))
	    (let* ((all-bytes (apply #'concatenate 'vector (nreverse chunks)))
		   (decoded (mail-cp-bytes-to-string all-bytes cp)))
	      (delete-region beg end)
	      (goto-char beg)
	      (insert decoded))))))))

(defstruct mailstructure
  number
  headers
  beg
  end
  structure)

(defmacro mail-get-header (key headers)
  `(assoc ,key ,headers :test #'string=))

(defmacro mail-get-header-value (key headers)
  `(cdr (assoc ,key ,headers :test #'string=)))

(defmacro mail-get-header-option-value (key opts)
  `(cdr (assoc ,key ,opts :test #'equalp)))

(defun mail-decode-mime-header ()
  (save-excursion
    (while (or (looking-for "From ")
	       (looking-for ">From ")
	       (looking-for "X-KaMail-Status"))
      (or (forward-line 1) (return)))
    (save-excursion
      (when (scan-buffer "^X-Mailer :" :regexp t :tail t)
	(delete-region (- (point) 2) (- (point) 1))))
    ;; 追加：ヘッダ領域内の連続する encoded-word 間の空白・改行を除去 (RFC 2047)
    (save-excursion
      (save-restriction
	(let ((hbeg (point))
	      (hend (save-excursion
		      (if (scan-buffer "^$" :regexp t)
			  (point)
			(point-max)))))
	  (narrow-to-region hbeg hend)
	  (goto-char (point-min))
	  (replace-buffer "\\?=\\(?:[ \t]*\r?\n[ \t]+\\|[ \t]+\\)=\\?" "?==?" :regexp t)
	  (mail-decode-cp-mime-words)))) ; 追加
    (decode-mime-header)))

; (defun mail-decode-mime-header ()
;   (save-excursion
;     (while (or (looking-for "From ")
; 	       (looking-for ">From ")
; 	       (looking-for "X-KaMail-Status"))
;       (or (forward-line 1) (return)))
;     (save-excursion
;       (when (scan-buffer "^X-Mailer :" :regexp t :tail t)
; 	(delete-region (- (point) 2) (- (point) 1))))
;     (decode-mime-header)))

(defun mail-encode-mime-header (&optional charset)
  (setq charset (or charset *encode-mime-charset-name*))
  (let ((*encode-mime-charset-name* charset))
    (declare (special *encode-mime-charset-name*))
    (encode-mime-header)))

(defun mail-multipart-p (headers)
  (let((content-type
	(mime-parse-content
	 (or (mail-get-header-value "content-type" headers) ""))))
    (and (mail-get-header-option-value "multipart" content-type)
	 (mail-get-header-option-value "boundary" content-type))))

; (defvar *nkf-command* (concat (user-homedir-pathname) "nkfwin/vc2005/win32(98,Me,NT,2000,XP,Vista,7)Windows-31J/nkf32"))
(defvar *nkf-command* (map-slash-to-backslash (concat (user-homedir-pathname) "bin/nkf32.exe")))
(defvar *pandoc-command* (map-slash-to-backslash (concat (user-homedir-pathname) "bin/pandoc.exe")))

(defvar *mail-header-fields-only-one*
  '("date"
    "from"
    "sender"
    "reply-to"
    "to"
    "cc"
    "bcc"
    "message-id"
    "in-reply-to"
    "references"
    "subject"))

(defun mail-parse-header ()
  (let (headers
	key
	val)
    (save-excursion
      (goto-char (point-min))
      (while (not (looking-at "^$"))
	(if (looking-at "^\\([^\000-\040:\177-\377]+\\):[ \t]*\\(.*\\)$")
	    (let (old)
	      (setq key (string-downcase (match-string 1)))
	      (setq val (match-string 2))
	      (while (and (forward-line 1)
			  (looking-at "^[ \t]+\\(.*\\)$"))
		(setq val (concat val " " (match-string 1))))
	      (if (setq old (mail-get-header-value key headers))
		  (if (member key *mail-header-fields-only-one* :test #'equal)
		      (setf (mail-get-header-value key headers) val)
		    (setf (mail-get-header-value key headers)
			  (if (listp old)
			      (append old (list val))
			    (list old val))))
		(push (cons key val) headers)))
	  (or (forward-line 1) (return)))))
    (nreverse headers)))

(defun mail-goto-body-beg ()
  (unless (scan-buffer "^$" :regexp t)
    (return-from mail-goto-body-beg))
  (or (forward-line) (return-from mail-goto-body-beg))
  t)

(defun mail-get-header-content (key headers)
  (let ((value (mail-get-header-value key headers)))
    (when value
      (cond ((equal key "content-type")
	     (mime-parse-content value))
	    ((equal key "content-disposition")
	     (mime-parse-disposition value))
	    ((equal key "content-transfer-encoding")
	     (mime-parse-encoding value))
	    (t
	     value)))))

(defun mail-get-content-mime-type (content-type)
  (when content-type
    (format nil "~@[~A~]/~@[~A~]"
	    (car (car content-type))
	    (cdr (car content-type)))))

(defun mail-get-content-charset (content-type)
  (mail-get-header-option-value "charset" content-type))

(defun mail-inline-p (headers)
  (let* ((content-type
	  (mail-get-header-content "content-type" headers))
	 (mime-type (mail-get-content-mime-type content-type))
	 (content-disposition
	  (mail-get-header-content "content-disposition" headers))
	 (content-transfer-encoding
	  (mail-get-header-content "content-transfer-encoding" headers)))
    (or (not mime-type)
	(and (equalp "text/plain" mime-type)
	     (not (equalp "attachment" (car content-disposition)))))
    ))

; (defun mail-inline-p (headers) ; 追加しました
;   (let* ((content-type              (mail-get-header-content "content-type" headers))
;          (mime-type                 (mail-get-content-mime-type content-type))
;          (content-disposition       (mail-get-header-content "content-disposition" headers))
;          (content-transfer-encoding (mail-get-header-content "content-transfer-encoding" headers)))
;     (or (not mime-type)
;         (and (or (equalp "text/plain" mime-type)
; ;                  (equalp "message/rfc822" mime-type)
; ;                  (equalp "text/html" mime-type)
;                  )
;              (not (equalp "attachment" (car content-disposition)))))
;     ))

(defun mail-get-filename (headers)
  (let* ((content-type
	  (mail-get-header-content "content-type" headers))
	 (mime-type (mail-get-content-mime-type content-type))
	 (content-disposition
	  (mail-get-header-content "content-disposition" headers)))
    (or (mail-get-header-option-value "filename" content-disposition)
	(mail-get-header-option-value "name" content-type))))

(defun mail-parse-multipart (headers &optional prefix)
  (let ((boundary (mail-multipart-p headers))
	structures
	body-start
	(content-type (mail-get-header-value "content-type" headers))
	(cnt 0))
    (unless boundary
      (return-from mail-parse-multipart))
    (save-excursion
      (unless (mail-goto-body-beg)
	(return-from mail-parse-multipart))
      (setq body-start (point))	; body 開始位置 - 不要？
      (let ((boundary-regexp (concat "^--" (regexp-quote boundary) "\\(--\\)?$"))
	    beg
	    end)
	(unless (scan-buffer boundary-regexp :regexp t :tail nil)
	  ;(error "Missing boundary.")
	  (return-from mail-parse-multipart))
	(or (forward-line) (return-from mail-parse-multipart))
	(setq beg (point))
	(while (scan-buffer boundary-regexp :regexp t :tail nil)
	  (setq end (point))
	  (incf cnt)
	  (let ((end-p (match-beginning 1))
		(part-number (format nil "~@[~A.~]~D" prefix cnt))
		part-headers
		part-beg
		part-end
		part-body
		part-structures)
	    (save-excursion
	      (save-restriction
		(narrow-to-region beg end)
		(goto-char beg)
		(mail-decode-mime-header)
		(setq part-headers (mail-parse-header))
		;; RFC 2046 - 5.1.1
		;; If no Content-Type field is present it is assumed to be "message/rfc822" in a
		;; "multipart/digest" and "text/plain" otherwise.
		(unless (mail-get-header "content-type" part-headers)
		  (if (string-match "multipart/digest" content-type)
		      (push (cons "content-type" "message/rfc822") part-headers)
		    (push (cons "content-type" "text/plain") part-headers)))
		(mail-goto-body-beg)
		(setq part-beg (point))
		(setq part-end (point-max))
		(when (mail-multipart-p part-headers)
		  (goto-char beg)
		  (setq part-structures
			(mail-parse-multipart part-headers (format nil "~D" cnt))))
		(push (make-mailstructure :number part-number
					  :headers part-headers
					  :body part-body
					  :beg part-beg
					  :end part-end
					  :structure part-structures)
		      structures)))
	    (and end-p (return))
	    (goto-char (1+ beg))
	    (or (scan-buffer boundary-regexp :regexp t) (return))
	    (or (forward-line) (return))
	    (setq beg (point))
	    ))
	))
    (nreverse structures)))

(defun mail-decode-encoding (headers &optional beg end)
  (let* ((charset (mail-get-content-type-charset headers))
         (encoding (mail-get-transfer-encoding headers))
         (content-type (mail-get-header-content "content-type" headers))
         (raw-charset (mail-get-content-charset content-type))
         (cp (mail-windows-codepage raw-charset))
         (mime-type (mail-get-content-mime-type content-type)))
    (setq beg (or beg (point)))
    (setq end (or end (point-max)))
    (save-restriction
      (narrow-to-region beg end)
      (when (equalp encoding "quoted-printable")
	(quoted-printable-decode-region beg end))
      (setq end (point-max))
      (when (equalp encoding "base64")
	(base64-decode-region beg end))
      (setq end (point-max))
      (if cp
	  (let ((tmp-in (make-temp-file-name "km3in"))
		(tmp-out (make-temp-file-name "km3out")))
	    (unwind-protect
		(progn
		  (write-region beg end tmp-in nil *encoding-binary* *eol-lf*)
		  (when (mail-convert-file-cp-to-utf8 tmp-in tmp-out cp)
		    (delete-region beg end)
		    (goto-char beg)
		    (insert-file-noconv tmp-out)
		    (map-utf-8-region beg (point-max))
		    (goto-char beg)
		    (replace-buffer "\r\n" "\n")))
	      (when (file-exist-p tmp-in) (delete-file tmp-in))
	      (when (file-exist-p tmp-out) (delete-file tmp-out))))
	(if (eq charset *encoding-auto*)
	    (map-char-encoding-region (or beg (point))
				      (or end (point-max)))
	  (map-char-encoding-region (or beg (point))
				    (or end (point-max))
				    charset)))
;       (if cp
; 	  (let* ((raw (buffer-substring beg end))
; 		 (len (length raw))
; 		 (bytes (make-array len)))
; 	    (dotimes (i len)
; 	      (setf (aref bytes i) (logand (char-code (char raw i)) #xff)))
; 	    (delete-region beg end)
; 	    (goto-char beg)
; 	    (insert (substitute-string (mail-cp-bytes-to-string bytes cp) "\r\n" "\n")))
; 	(if (eq charset *encoding-auto*)
; 	    (map-char-encoding-region (or beg (point))
; 				      (or end (point-max)))
; 	  (map-char-encoding-region (or beg (point))
; 				    (or end (point-max))
; 				    charset)))
      (setq end (point-max))
      (when (equalp mime-type "text/html")
        (filter-buffer (concat *nkf-command* " -w | " *pandoc-command* " -f html -t plain")))
      )
    encoding))

; (defun mail-decode-encoding (headers &optional beg end)
;   (let ((charset (mail-get-content-type-charset headers))
; 	(encoding (mail-get-transfer-encoding headers)))
;     (setq beg (or beg (point)))
;     (setq end (or end (point-max)))
;     (save-restriction
;       (narrow-to-region beg end)
;       (when (equalp encoding "quoted-printable")
; 	(quoted-printable-decode-region beg end))
;       (setq end (point-max))
;       (when (equalp encoding "base64")
; 	(base64-decode-region beg end))
;       (setq end (point-max))
;       (if (eq charset *encoding-auto*)
; 	  (map-char-encoding-region (or beg (point))
; 				    (or end (point-max)))
; 	(map-char-encoding-region (or beg (point))
; 				  (or end (point-max))
; 				  charset)))
;     encoding))

(defun mail-encode-encoding (headers &optional beg end)
  (let ((charset (mail-get-content-type-charset headers))
	(encoding (mail-get-transfer-encoding headers)))
    (setq beg (or beg (point)))
    (setq end (or end (point-max)))
    (save-restriction
      (narrow-to-region beg end)
      (goto-char beg)
      (unless (eq charset *encoding-auto*)
	(when (eq charset *encoding-utf8*)
	  (setq charset *encoding-utf8n*)) ; UTF-8はBOMなしで
	(insert (convert-encoding-from-internal charset
						(buffer-substring beg end)))
	(delete-region (point) (point-max))
	(setq end (point-max)))
      (when (equalp encoding "quoted-printable")
	(goto-char beg)
	(insert (si:quoted-printable-encode
		 (buffer-substring beg end)))
	(delete-region (point) (point-max)))
      (setq end (point-max))
      (when (equalp encoding "base64")
	(goto-char beg)
	(insert (si:base64-encode
		 (buffer-substring beg end)))
	(delete-region (point) (point-max))))
    encoding))

(defun mail-get-mime-charset (charset)
  (or (gethash charset *mime-charset-name-hash-table*)
      *encoding-auto*))

(defun mail-get-content-type-charset (headers)
  (let* ((ctype (mail-get-header-content "content-type" headers))
	 (charset (mail-get-header-option-value "charset" ctype)))
    (if charset
	(mail-get-mime-charset charset)
      *encoding-auto*
      )))

(defun mail-get-transfer-encoding (headers)
  (mail-get-header-content "content-transfer-encoding" headers))

(defun mail-parse ()
  (let (headers
	beg
	end
	structure)
    (mail-decode-mime-header)
    (setq headers (mail-parse-header))
    (if (mail-multipart-p headers)
	(setq structure (mail-parse-multipart headers))
      (setq beg (progn
		  (mail-goto-body-beg)
		  (point))
	    end (point-max)))
    (make-mailstructure :headers headers
			:beg beg
			:end end
			:structure structure)))

(defun test-print-structure (s structure)
;   (mail-inline-p (mailstructure-headers structure))
  (format s "~S~%~{~S~%~}~S~%~S~%----------~%"
	  (mailstructure-number structure)
	  (mailstructure-headers structure)
	  (mailstructure-beg structure)
	  (mailstructure-end structure))
  (dolist (part (mailstructure-structure structure))
    (test-print-structure s part)))

(defun user::test-mail-parse ()
  (interactive)
  (setq structure (mail-parse))
  (message-box
   (with-output-to-string (s)
     (test-print-structure s structure))))
