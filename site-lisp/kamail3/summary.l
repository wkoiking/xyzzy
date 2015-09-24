;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; This file is not part of xyzzy.
;;;
; $Id: summary.l 737 2006-09-05 17:01:15Z torihat $
;
; kamail3/summary.l
;
; by HATTORI Masashi

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "kamail3/defs"))

(provide "kamail3/summary")

(in-package "kamail3")

(defvar *summary-print-thread* nil)

(defvar-local *summary-message-list* nil)
(defvar-local *summary-mark-list* nil)

(defvar *summary-mode-hook* nil)
(defvar *summary-archive-seen-messages-hook* nil)

(defvar *summary-datetime-format* "%y/%m/%d %H:%M ")

(defvar *summary-buffer-partial-temp*
  (concat *buffer-prefix* "Partial Temp*"))
(defvar *summary-message-partial-temp*
  (merge-pathnames "message-partial.eml" *data-directory*))

(defvar *summary-map* nil)
(setq *summary-map* (make-sparse-keymap))
(define-key *summary-map* #\g            'fetch-execute-all)
(define-key *summary-map* #\G            'fetch-execute-select)
(define-key *summary-map* #\SPC          'summary-view-mail-or-scroll)
(define-key *summary-map* #\TAB          'summary-view-next-unseen)
(define-key *summary-map* #\n            'summary-view-next)
(define-key *summary-map* #\p            'summary-view-previous)
(define-key *summary-map* #\;            'summary-popup-message-attrs)
(define-key *summary-map* #\s            'summary-message-save-attachment)
(define-key *summary-map* #\x            'summary-message-save-execute-attachment)
(define-key *summary-map* #\j            'summary-show-message-partial)
(define-key *summary-map* #\/            'query-search)
(define-key *summary-map* #\?            'query-search-detail)
(define-key *summary-map* #\Right        'query-search-next)
(define-key *summary-map* #\Left         'query-search-prev)
(define-key *summary-map* #\h            'summary-search-history-select)
(define-key *summary-map* '(#\C-x #\C-s) 'summary-save-as-folder)
(define-key *summary-map* '(#\l #\f)     'summary-search-by-from)
(define-key *summary-map* '(#\l #\F)     'summary-search-by-from-to-cc)
(define-key *summary-map* '(#\l #\s)     'summary-search-by-subject)
(define-key *summary-map* '(#\l #\m)     'summary-search-by-msgid)
(define-key *summary-map* '(#\l #\t)     'summary-search-by-thread)
(define-key *summary-map* '(#\m #\m)     'summary-mark-toggle)
(define-key *summary-map* '(#\m #\a)     'summary-mark-all)
(define-key *summary-map* '(#\m #\c)     'summary-mark-clear)
(define-key *summary-map* '(#\m #\/)     'summary-mark-with-condition)
(define-key *summary-map* #\c            'summary-draft-create)
(define-key *summary-map* #\r            'summary-message-reply)
(define-key *summary-map* #\R            'summary-message-reply-all)
(define-key *summary-map* #\f            'summary-message-forward)
(define-key *summary-map* #\F            'summary-message-forward-as-attachment)
(define-key *summary-map* '(#\t #\s)     'summary-tag-select)
(define-key *summary-map* '(#\t #\a)     'summary-tag-add)
(define-key *summary-map* '(#\t #\d)     'summary-tag-del-select)
(define-key *summary-map* '(#\t #\m)     'summary-marked-message-tag-add)
(define-key *summary-map* #\e            'folder-select)
(define-key *summary-map* #\a            'summary-archive-seen-messages)
(define-key *summary-map* #\I            'summary-move-to-inbox)
(define-key *summary-map* #\A            'summary-move-to-archive)
(define-key *summary-map* #\S            'summary-move-to-spam)
(define-key *summary-map* #\T            'summary-move-to-trash)
(define-key *summary-map* '(#\C-c #\s)   'summary-clear-spam)
(define-key *summary-map* '(#\C-c #\t)   'summary-clear-trash)
(define-key *summary-map* '(#\C-c #\E)   'folder-delete-select)
(define-key *summary-map* #\X            'summary-expire-execute)
(define-key *summary-map* '(#\C-x #\C-e) 'summary-save-as-expire-config)
(define-key *summary-map* '(#\C-c #\X)   'expire-config-delete-select)
(define-key *summary-map* '(#\C-c #\a)   'summary-address-add)
(define-key *summary-map* #\q            'summary-close)
(define-key *summary-map* #\F1           'show-help)
(define-key *summary-map* #\y            'summary-sync)
(define-key *summary-map* #\Q            'finish)
(define-key *summary-map* #\RBtnUp       'summary-mouse-right-press)

(defun summary-menu-create ()
  (let ((menu (create-popup-menu)))
    (add-menu-item menu
		   'fetch-execute-all
		   "全て受信"
		   'fetch-execute-all)
    (add-popup-menu menu
		    (fetch-execute-select-menu)
		    "選択受信")
    (add-menu-separator menu)
    (add-menu-item menu
		   'summary-view-mail-or-scroll
		   "メールを表示/スクロール"
		   'summary-view-mail-or-scroll)
    (add-menu-item menu
		   'summary-view-next-unseen
		   "次の未読メールを表示"
		   'summary-view-next-unseen)
    (add-menu-item menu
		   'summary-view-next
		   "次の行のメールを表示"
		   'summary-view-next)
    (add-menu-item menu
		   'summary-view-previous
		   "前の行のメールを表示"
		   'summary-view-previous)
    (add-menu-item menu
		   'summary-popup-message-attrs
		   "メールの属性を表示"
		   'summary-popup-message-attrs)
    (add-menu-item menu
		   'summary-message-save-attachment
		   "添付ファイルを選択して保存..."
		   'summary-message-save-attachment)
    (add-menu-item menu
		   'summary-message-save-execute-attachment
		   "添付ファイルを保存して実行..."
		   'summary-message-save-execute-attachment)
    (add-menu-item menu
		   'summary-show-message-partial
		   "partialメッセージを結合して表示"
		   'summary-show-message-partial)
    (add-menu-separator menu)
    (add-menu-item menu
		   'query-search
		   "通常検索..."
		   'query-search)
    (add-menu-item menu
		   'query-search-detail
		   "詳細検索..."
		   'query-search-detail)
    (add-menu-item menu
		   'query-search-next
		   "次のn件を表示"
		   'query-search-next)
    (add-menu-item menu
		   'query-search-prev
		   "前のn件を表示"
		   'query-search-prev)
    (add-popup-menu menu
		    (summary-search-history-select-menu)
		    "検索履歴")
    (add-menu-item menu
		   'summary-save-as-folder
		   "検索条件をフォルダとして登録..."
		   'summary-save-as-folder)
    (add-menu-item menu
		   'summary-search-by-from
		   "メールの送信者で検索"
		   'summary-search-by-from)
    (add-menu-item menu
		   'summary-search-by-from-to-cc
		   "メールの送信者でFrom,To,Ccを検索"
		   'summary-search-by-from-to-cc)
    (add-menu-item menu
		   'summary-search-by-subject
		   "メールの件名で検索..."
		   'summary-search-by-subject)
    (add-menu-item menu
		   'summary-search-by-thread
		   "スレッドを検索"
		   'summary-search-by-thread)
    (add-menu-item menu
		   'summary-search-by-msgid
		   "メールのIn-Reply-ToもしくはMessage-Idで検索"
		   'summary-search-by-msgid)
    (add-menu-separator menu)
    (add-menu-item menu
		   'summary-mark-toggle
		   "メールのマークをトグル"
		   'summary-mark-toggle)
    (add-menu-item menu
		   'summary-mark-all
		   "全てマークする"
		   'summary-mark-all)
    (add-menu-item menu
		   'summary-mark-clear
		   "全てマークを外す"
		   'summary-mark-clear)
    (add-menu-item menu
		   'summary-mark-with-condition
		   "ヘッダの条件を指定してマーク..."
		   'summary-mark-with-condition)
    (add-menu-separator menu)
    (add-menu-item menu
		   'summary-draft-create
		   "メールを新規作成"
		   'summary-draft-create)
    (add-menu-item menu
		   'summary-message-reply
		   "メールに返信"
		   'summary-message-reply)
    (add-menu-item menu
		   'summary-message-reply-all
		   "全員に返信"
		   'summary-message-reply-all)
    (add-menu-item menu
		   'summary-message-forward
		   "メールを転送"
		   'summary-message-forward)
    (add-menu-item menu
		   'summary-message-forward-as-attachment
		   "メールを添付ファイルとして転送"
		   'summary-message-forward-as-attachment)
    (add-menu-separator menu)
    (add-popup-menu menu
		    (summary-tag-select-menu)
		    "タグで検索")
    (add-menu-item menu
		   'summary-tag-add
		   "タグを入力してつける..."
		   'summary-tag-add)
    (add-menu-item menu
		   'summary-tag-del-select
		   "タグを外す"
		   'summary-tag-del-select)
    (add-menu-item menu
		   'summary-marked-message-tag-add
		   "マークしたメッセージにタグを付ける..."
		   'summary-marked-message-tag-add
		   #'(lambda ()
		       (unless *summary-mark-list*
			 :disable)))
    (add-menu-separator menu)
    (add-popup-menu menu
		    (folder-select-menu)
		    "フォルダ選択")
    (add-menu-item menu
		   'summary-archive-seen-messages
		   "既読メールをアーカイブ"
		   'summary-archive-seen-messages)
    (add-menu-item menu
		   'summary-move-to-archive
		   "メールをアーカイブ"
		   'summary-move-to-archive)
    (add-menu-item menu
		   'summary-move-to-inbox
		   "メールをinboxへ移動"
		   'summary-move-to-inbox)
    (add-menu-item menu
		   'summary-move-to-spam
		   "メールをspamへ移動"
		   'summary-move-to-spam)
    (add-menu-item menu
		   'summary-move-to-trash
		   "メールをtrashへ移動"
		   'summary-move-to-trash)
    (add-menu-item menu
		   'summary-clear-spam
		   "spamフォルダのメールを削除"
		   'summary-clear-spam)
    (add-menu-item menu
		   'summary-clear-trash
		   "trashフォルダのメールを削除"
		   'summary-clear-trash)
    (add-popup-menu menu
		    (folder-delete-select-menu)
		    "検索フォルダ削除")
    (add-menu-separator menu)
    (add-menu-item menu
		   'summary-expire-execute
		   "Expire実行"
		   'summary-expire-execute)
    (add-menu-item menu
		   'summary-save-as-expire-config
		   "検索条件をExpire設定として保存"
		   'summary-save-as-expire-config)
    (add-popup-menu menu
		    (expire-config-delete-select-menu)
		    "Expire設定削除")
    (add-menu-separator menu)
    (add-menu-item menu
		   'summary-address-add
		   "送信者をアドレス帳に登録..."
		   'summary-address-add)
    (add-menu-separator menu)
    (add-menu-item menu
		   'show-help
		   "readme.htmlを表示"
		   'show-help)
    (add-menu-item menu
		   'summary-sync
		   "疑似インデックスをマージ"
		   'summary-sync)
    (add-menu-item menu
		   'summary-close
		   "メッセージを閉じる/終了"
		   'summary-close)
    (add-menu-item menu
		   'finish
		   "終了"
		   'finish)
    menu))

(defun summary-mouse-right-press ()
  (interactive)
  (set-window *last-mouse-window*)
  (continue-pre-selection)
  (let ((menu (summary-menu-create)))
    (track-popup-menu menu)))

(defun summary-get-header (number)
  (assoc number *summary-message-list*))

(defun summary-set-header (number header)
  (setf (cdr (assoc number *summary-message-list*)) header))

(defun summary-buffer-set ()
  (set-buffer (get-buffer-create *buffer-summary*)))

(defun summary-window-set ()
  (summary-buffer-set)
  (delete-other-windows))

(defun summary-mode ()
  (interactive)
  (kill-all-local-variables)
  (make-local-variable 'mode-line-format)
  (make-local-variable 'indent-tabs-mode)
  (setq indent-tabs-mode nil)
  (setq *summary-mark-list* nil)
  (setq buffer-read-only t)
  (setq need-not-save t)
  (setq kept-undo-information nil)
  (setq auto-save nil)
  (toggle-ime nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-line-number* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-newline* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-eof* nil)
  (set-local-window-flags (selected-buffer)
			  *window-flag-cursor-line* t)
  (set-buffer-fold-width nil)
  (use-keymap *summary-map*)
  (title-mode-line)
  (run-hooks '*summary-mode-hook*))

(defun summary-delete-backward-to-column (col)
  (while (< col (current-column))
    (delete-backward-char))
  (indent-to col))

(defun summary-print ()
  (junk::modify-read-only-buffer
    (erase-buffer (selected-buffer))
    (if *summary-print-thread*
	(summary-print-thread *summary-message-list*)
      (dolist (msg *summary-message-list*)
	(let ((num (car msg))
	      (header (cdr msg)))
	  (summary-print-line num header))))
    (goto-char (point-min))))

(defun summary-reprint ()
  (let ((line (current-line-number)))
    (summary-print)
    (goto-line line)))

(defun summary-reload ()
  (interactive)
  (query-execute *query-last-query*
		 *query-last-folder*))

(setq *summary-column-mark*           0)
(setq *summary-column-status*         1)
(setq *summary-column-multipart-end*  4)
(setq *summary-column-from-end*      45)

(defun summary-print-line (num header &optional indent-string)
  (let* ((path (junk::mail-get-header-value "_lpath" header))
	 (fromstr (junk::mail-get-header-value "from" header))
	 (from fromstr)
	 from-string
	 (title (junk::mail-get-header-value "subject" header))
	 (datestr (junk::mail-get-header-value "date" header))
	 (date (and datestr (junk::rfc822-parse-date (if (listp datestr)
							 (car datestr)
						       datestr))))
	 (status (attr-status-get header))
	 (multipart-p (junk::mail-multipart-p header))
	 (marked (summary-message-marked num))
	 )
    (multiple-value-bind (email name)
	(parse-address from)
      (setq from-string (or name email from)))
    (apply #'set-text-attribute
	   (point)
	   (progn
	     (insert (summary-mark-string marked))
	     (point))
	   num
	   *attribute-summary-mark*)
    (apply #'set-text-attribute
	   (point)
	   (progn
	     (insert (summary-status-string status))
	     (point))
	   num
	   *attribute-summary-status*)
    (apply #'set-text-attribute
	   (point)
	   (progn
	     (insert (if multipart-p
			 *summary-status-multipart*
		       " "))
	     (insert " ")
	     (point))
	   num
	   *attribute-summary-multipart*)
    (apply #'set-text-attribute
	   (point)
	   (progn
	     (insert (format-date-string *summary-datetime-format* (or date 0)))
	     (point))
	   num
	   *attribute-summary-date*)
    (apply #'set-text-attribute
	   (point)
	   (progn
	     (insert (format nil "~@[~A~]" from-string))
	     (point))
	   num
	   *attribute-summary-from*)
    (summary-delete-backward-to-column *summary-column-from-end*) ; 適当に桁揃え
    (apply #'set-text-attribute
	   (point)
	   (progn
	     (insert " ")
	     (when indent-string
	       (insert indent-string))
	     (insert (format nil "~@[~A~]" title))
	     (point))
	   num
	   *attribute-summary-title*)
    (insert "\n")))

(defun summary-print-thread (messages)
  (when messages
    (long-operation
      (kamail3-message "parsing thread ...")
      (multiple-value-bind (parent-list response-list)
	  (summary-thread-parse messages)
	(progn
	  (when *debug-mode*
	    (msgbox "~S~%~%~S" parent-list response-list))
	  (kamail3-message "formatting thread ...")
	  (dolist (parent parent-list)
	    (summary-thread-print-recursive parent response-list "" ""))
	  (kamail3-message "formatting thread ... done.")
	  )))))

(defun summary-thread-parse (messages)
  (let (id-alist
	rp-alist
	response-list
	parent-list)
    (dolist (x messages)
      (let* ((num (car x))
	     (h (cdr x))
	     (message-id  (junk::mail-get-header-value "message-id" h))
	     (in-reply-to (junk::mail-get-header-value "in-reply-to" h))
	     (references  (junk::mail-get-header-value "references" h))
	     tmp
	     )
	(if (and (stringp in-reply-to)
		 (string-match "\\(<[^>]+>\\)" in-reply-to))
	    (setq in-reply-to (match-string 1))
	  (if (stringp references)
	      (progn
		(while (string-match "<[^>]+>" references)
		  (setq tmp (match-string 0))
		  (setq references (substring references (match-end 0) (length references))))
		(if tmp
		    (setq in-reply-to tmp)
		  (setq in-reply-to nil)))))
	(push (cons message-id num) id-alist)
	(push (cons num in-reply-to) rp-alist)))
    (dolist (x (reverse rp-alist))
      (let ((num (car x))
	    (in-reply-to (cdr x))
	    parent
	    )
	(if (and in-reply-to
		 (setq parent (cdr (assoc in-reply-to id-alist :test #'equal))))
	    (let ((childs (cdr (assoc parent response-list))))
	      (if childs
		  (progn
		    (pushnew num childs)
		    (rplacd (assoc parent response-list) childs))
		(push (cons parent (list num)) response-list)))
	  (push num parent-list))))
    (values (nreverse parent-list)
	    (nreverse response-list))))

(defun summary-thread-print-recursive (num response-list indent-prefix indent-string)
  (summary-print-line num
		      (cdr (summary-get-header num))
		      indent-string)
  (let ((childs (reverse (cdr (assoc num response-list))))
	cnt)
    (when childs
      (setq cnt (list-length childs))
      (dotimes (i cnt)
	(let* ((terminate-p (= i (1- cnt)))
	       (child (nth i childs))
	       (next-prefix (if terminate-p
				(concat indent-prefix "   ")
			      (concat indent-prefix "┃ ")))
	       (next-string (if terminate-p
				(concat indent-prefix "┗ ")
			      (concat indent-prefix "┣ "))))
	  (summary-thread-print-recursive child
					  response-list
					  next-prefix
					  next-string))))))

(defun summary-message-seen-if (msg)
  (let* ((num (car msg))
	 (header (cdr msg))
	 (status (attr-status-get header))
	 new-status
	 file)
    (unless (attr-status-seen status)
      (setq new-status (attr-status-add-seen status))
      (summary-set-header num (attr-status-set header new-status))
      (setq file (summary-message-file header))
      (when (attr-status-update file new-status)
	(summary-status-update new-status)))
    ))

(defun summary-status-string (status)
  (cond ((attr-status-forwarded status)
	 *summary-status-forwarded*)
	((attr-status-answered status)
	 *summary-status-answered*)
	((attr-status-seen status)
	 *summary-status-seen*)
	(t
	 *summary-status-unseen*)))

(defun summary-status-update (status)
  (let ((string (summary-status-string status)))
    (save-excursion
      (junk::modify-read-only-buffer
	(goto-column *summary-column-status*)
	(insert string)
	(delete-char 1)
	))))

(defun summary-mark-string (marked)
  (if marked
      *summary-mark-string-marked*
    *summary-mark-string-normal*))

(defun summary-message-marked (num)
  (member num *summary-mark-list*))

(defun summary-mark-update (marked)
  (let ((string (summary-mark-string marked)))
    (save-excursion
      (junk::modify-read-only-buffer
	(goto-column *summary-column-mark*)
	(insert string)
	(delete-char 1)
	))))

(defun summary-mark-add (num)
  (pushnew num *summary-mark-list*))

(defun summary-mark-del (num)
  (setq *summary-mark-list*
	(delete num *summary-mark-list*)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun summary-message-current ()
  (multiple-value-bind (from to tag foreground background bold underline strike-out prefix extend)
      (find-text-attribute-point (point))
    (when tag
      (summary-get-header tag))))

#|
(defun summary-message-file (header)
  (let ((real (junk::mail-get-header-value "_lreal" header)))
    (if real
	(map-backslash-to-slash real)
      real)))
|#

(defun summary-message-file (header)
  (let ((uri (junk::mail-get-header-value "@uri" header)))
    (if uri
	(he-uri-to-path uri)
      uri)))

(defun summary-recenter ()
  (recenter))

(defun summary-view-next (&optional (n 1))
  (interactive "p")
  (if (and (forward-line n)
	   (not (eobp)))
      (summary-view-mail-or-scroll)
    (when (message-window-get)
      (message-close))))

(defun summary-view-previous (&optional (n -1))
  (interactive "p")
  (summary-view-next n))

(defun summary-message-reply-all ()
  (interactive)
  (summary-message-reply t))

(defun summary-message-reply (&optional all)
  (interactive "p")
  (when (summary-view-mail)
    (message-reply all)))

(defun summary-message-forward-as-attachment ()
  (interactive)
  (summary-message-forward t))

(defun summary-message-forward (&optional as-attachment)
  (interactive "p")
  (when (summary-view-mail)
    (if as-attachment
	(message-forward-as-attachment)
      (message-forward))))

(defun summary-message-save-execute-attachment ()
  (interactive)
  (summary-message-save-attachment t))

(defun summary-message-save-attachment (&optional execute)
  (interactive "p")
  (when (summary-view-mail)
    (message-save-attachment-select execute)))

(defun summary-draft-create ()
  (interactive)
  (when (message-window-get)
    (message-window-set))
  (draft-create))

(defun summary-view-mail ()
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (file (summary-message-file header)))
    (unless file
      (return-from summary-view-mail))
    (if (and (equal file *message-file-current*)
	     (message-window-get))
	(message-window-set)
      (message-show file))
    t))

(defun summary-view-mail-or-scroll ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (file (summary-message-file header)))
    (when file
      (if (and (equal file *message-file-current*)
	       (message-window-get))
	  (let (scroll-end)
	    (message-window-set)
	    (setq scroll-end (message-scroll-forward))
	    (other-window 1)
	    (when scroll-end
	      (summary-view-next)))
	(progn
	  (unless (file-exist-p file)
	    (error "ファイルがありません: ~S" file))
	  (summary-recenter)
	  (when (message-show file)
	    (other-window 1)
	    (summary-message-seen-if msg)
	    ))))))

(defun summary-mark-toggle ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (marked (summary-message-marked num)))
    (when num
      (if marked
	  (progn
	    (summary-mark-del num)
	    (summary-mark-update (not marked)))
	(progn
	  (summary-mark-add num)
	  (summary-mark-update (not marked)))))))

(defun summary-mark-all ()
  (interactive)
  (dolist (msg *summary-message-list*)
    (let ((num (car msg)))
      (summary-mark-add num)))
  (summary-reprint))

(defun summary-mark-clear ()
  (interactive)
  (dolist (msg *summary-message-list*)
    (let ((num (car msg)))
      (summary-mark-del num)))
  (summary-reprint))

(defun summary-mark-with-condition ()
  (interactive)
  (let ((condition (read-string "Mark Condition: "))
	matched
	field
	regex)
    (when (stringp condition)
      (setq condition (string-trim " \t\n" condition)))
    (unless (string-match "^\\([-0-9a-zA-Z]+\\):\\(.+\\)$" condition)
      (kamail3-message "not condition: ~A" condition)
      (return-from summary-mark-with-condition))
    (setq field (match-string 1)
	  regex (match-string 2))
    (when (and field regex)
      (dolist (msg *summary-message-list*)
	(let* ((num (car msg))
	       (header (cdr msg))
	       (value (junk::mail-get-header-value field header)))
	  (when (and (stringp value)
		     (string-match regex value))
	    (summary-mark-add num)))))
    (summary-reprint)))

(defun summary-close ()
  (interactive)
  (if (message-window-get)
      (message-close)
    (finish)))

(defun summary-header-delete (num)
  (setq *summary-message-list*
	(delete num *summary-message-list* :key #'car)))

(defun summary-tag-add-1 (num header &optional tag)
  (let ((tags (attr-tags-get header))
	new-tag
	file)
    (when (and header
	       (setq file (summary-message-file header))
	       (setq tag (or tag
			     (attr-tag-input)))
	       (not (attr-tag-exist tag tags)))
      (setq tags (attr-tag-add tags tag))
      (setq new-tag (attr-tags-join tags))
      (setq header (attr-tag-set header new-tag))
      (summary-set-header num header)
      (when (attr-tag-update file new-tag)
	(attr-tag-regist tag)
	))))

(defun summary-marked-message-tag-add ()
  (interactive)
  (let ((tag (attr-tag-input)))
    (dolist (num *summary-mark-list*)
      (let ((header (cdr (summary-get-header num))))
	(when header
	  (summary-tag-add-1 num header tag))))))

(defun summary-tag-add ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg)))
    (when msg
      (summary-tag-add-1 num header))))

(defun summary-tag-del (num tag)
  (let* ((msg (summary-get-header num))
	 (header (cdr msg))
	 (tags (attr-tags-get header))
	 new-tag
	 file)
    (when (and msg
	       (attr-tag-exist tag tags)
	       (setq file (summary-message-file header)))
      (setq tags (attr-tag-del tags tag))
      (setq new-tag (attr-tags-join tags))
      (setq header (attr-tag-set header new-tag))
      (summary-set-header num header)
      (when (attr-tag-update file new-tag)
	
	))))

(defun summary-tag-select-menu ()
  (let((menu (create-popup-menu)))
    (dolist (tag *attr-tags-list*)
      (let ((name tag))
	(add-menu-item menu
		       name
		       name
		       #'(lambda ()
			   (interactive)
			   (summary-tag-open name)))))
    menu))

(defun summary-tag-select ()
  (interactive)
  (let* (menu)
    (when *attr-tags-list*
      (setq menu (summary-tag-select-menu))
      (track-popup-menu menu))))

(defun summary-tag-open (tag)
  (query-execute (make-query :tag tag
			     :order *folder-order-default*)
		 nil))

(defun summary-tag-del-select-menu (num tags)
  (let ((menu (create-popup-menu)))
    (dolist (tag tags)
      (let ((name tag))
	(add-menu-item menu
		       name
		       name
		       #'(lambda ()
			   (interactive)
			   (summary-tag-del num name)))))
    menu))

(defun summary-tag-del-select ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (tags (attr-tags-get header))
	 menu)
    (when tags
      (setq menu (summary-tag-del-select-menu num tags))
      (track-popup-menu menu))))

(defun summary-move-folder (new-folder)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (folder (attr-folder-get header))
	 file)
    (when (and msg
	       (not (equal folder new-folder))
	       (setq file (summary-message-file header)))
      (when (attr-folder-update file new-folder)
	(summary-header-delete num)
	(junk::modify-read-only-buffer
	    (delete-region (progn (goto-bol) (point))
			   (progn (or (forward-line) (goto-eol)) (point))))
	(summary-view-mail-or-scroll)
	))))

(defun summary-move-to-inbox ()
  (interactive)
  (summary-move-folder *folder-inbox*))

(defun summary-move-to-archive ()
  (interactive)
  (summary-move-folder *folder-archive*))

(defun summary-move-to-spam ()
  (interactive)
  (summary-move-folder *folder-spam*))

(defun summary-move-to-trash ()
  (interactive)
  (summary-move-folder *folder-trash*))

(defun summary-clear-folder (folder-name)
  (kamail3-message "deleting ~A ..." folder-name)
  (let ((deleted (folder-clear folder-name)))
    (summary-reload)
    (kamail3-message "~D message~P deleted." deleted deleted)))

(defun summary-clear-spam ()
  (interactive)
  (summary-clear-folder *folder-spam*))

(defun summary-clear-trash ()
  (interactive)
  (summary-clear-folder *folder-trash*))

(defun summary-archive-seen-messages ()
  (interactive)
  (let (file-list)
    (long-operation
      (kamail3-message "archiving seen messages ...")
      (dolist (msg *summary-message-list*)
	(let* ((num (car msg))
	       (header (cdr msg))
	       (status (attr-status-get header))
	       (folder (attr-folder-get header))
	       (file (summary-message-file header)))
	  (when (and (or (equal folder *folder-inbox*)
			 (not folder)) ; inboxからのみ
		     (attr-status-seen status)
		     file)
	    (push (cons file *folder-archive*) file-list))))
      (when file-list
	(attr-folder-update-batch (reverse file-list)))
      (kamail3-message "archiving seen messages ... done."))
    (run-hooks '*summary-archive-seen-messages-hook*)
    (when file-list
      (folder-open *folder-inbox*))))

(defun summary-popup-message-attrs ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (folder (attr-folder-get header))
	 (status (attr-status-get header))
	 (tags (attr-tags-get header)))
    (when (or folder status tags)
      (popup-string (format nil "Folder:~@[ ~A~]~%Status:~@[ ~A~]~%Tags:~@[ ~A~]~%"
			    folder
			    status
			    tags)
		    (point)))))

(defun summary-view-next-unseen ()
  (interactive)
  (let (line)
    (save-excursion
      (goto-char (point-min))
      (loop
	(goto-column *summary-column-status*)
	(when (equal (format nil "~C" (char-after (point)))
		     *summary-status-unseen*)
	  (setq line (current-line-number))
	  (return))
	(or (forward-line) (return))))
    (if line
	(progn
	  (goto-line line)
	  (summary-view-mail-or-scroll))
      (kamail3-message "未読はありません。"))))

(defun summary-search-history-select-menu ()
  (let ((menu (create-popup-menu)))
    (dolist (q *query-search-history*)
      (let ((query q)
	    (name (query-folder-name-default q)))
	(when name
	  (add-menu-item menu
			 nil
			 (abbreviate-menu-string name)
			 #'(lambda ()
			     (interactive)
			     (query-execute query))))))
    menu))

(defun summary-search-history-select ()
  (interactive)
  (when *query-search-history*
    (let ((menu (summary-search-history-select-menu)))
      (track-popup-menu menu))))

(defun summary-save-as-folder ()
  (interactive)
  (when *query-last-folder*
    (kamail3-error "フォルダは保存できません。"))
  (let* ((prompt "Folder name: ")
	 (query *query-last-query*)
	 (name (read-string prompt
			    :default (query-folder-name-default query))))
    (while (or (not (query-string-or-null name))
	       (folder-get name))
      (setq name (read-string prompt :default name)))
    (folder-add name query)))

(defun summary-save-as-expire-config ()
  (interactive)
  (when *query-last-folder*
    (kamail3-error "フォルダは保存できません。"))
  (expire-config-add *query-last-query*))

(defun summary-expire-execute ()
  (interactive)
  (let ((cnt (expire-execute)))
    (summary-reload)
    (kamail3-message "Expired ~D message~P." cnt cnt)))

(defun summary-address-add ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (from (junk::mail-get-header-value "from" header)))
    (when from
      (let ((from-list (parse-addresses from)))
	(dolist (f from-list)
	  (let ((email (car f))
		(name (cdr f)))
	    (address-add email name)))))))

(defun summary-search-by-from ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (from (junk::mail-get-header-value "from" header)))
    (when from
      (multiple-value-bind (email name)
	  (parse-address from)
	(when email
	  (query-execute (make-query :string (concat "from:" email))))))))

(defun summary-search-by-from-to-cc ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (from (junk::mail-get-header-value "from" header)))
    (when from
      (multiple-value-bind (email name)
	  (parse-address from)
	(when email
	  (query-execute (make-query :string (concat "from,to,cc:" email))))))))

(defun summary-search-by-subject ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (subject (junk::mail-get-header-value "subject" header)))
    (when subject
      (let ((str (read-string "Subject: "
			      :default subject)))
	(when str
	  (query-execute (make-query :string (concat "subject:" str))))))))

(defun summary-search-by-msgid ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (message-id (junk::mail-get-header-value "message-id" header))
	 (in-reply-to (junk::mail-get-header-value "in-reply-to" header))
	 ids)
    (when (or in-reply-to message-id)
      (setq ids (junk::rfc822-parse-msgid (or in-reply-to message-id)))
      (when ids
	(let ((fields "message-id,references,in-reply-to"))
	  (query-execute (make-query
			  :string (concat fields ":" (car ids)))))))))

(defun summary-search-by-thread ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 (message-id (junk::mail-get-header-value "message-id" header))
	 (references (junk::mail-get-header-value "references" header))
	 (in-reply-to (junk::mail-get-header-value "in-reply-to" header))
	 ids)
    (when (or message-id references in-reply-to)
      (dolist (f (list message-id references in-reply-to))
	(when f
	  (let ((tmp (junk::rfc822-parse-msgid f)))
	    (dolist (id tmp)
	      (pushnew id ids :test #'equal)))))
      (when ids
	(let ((fields "message-id,references,in-reply-to")
	      query)
	  (if (< 1 (list-length ids))
	      (let ((regexp (concat "("
				    (junk::string-join (mapcar #'(lambda (x)
								   (regexp-quote x))
							       ids)
						       "|")
				    ")")))
		(setq query
		      (make-query
		       :string (concat fields "~" regexp))))
	    (setq query
		  (make-query
		   :string (concat fields ":" (car ids)))))
	  (query-execute query)
	  )))))

(defun summary-parse-partial-message (msg)
  (let* ((num (car msg))
	 (header (cdr msg))
	 (content-type (junk::mail-get-header-content "content-type" header)))
    (when content-type
      (let ((message (junk::mail-get-header-option-value "message" content-type))
	    (id (junk::mail-get-header-option-value "id" content-type))
	    (number (junk::mail-get-header-option-value "number" content-type))
	    (total (junk::mail-get-header-option-value "total" content-type)))
	(when (and (equalp message "partial")
		   id
		   number
		   total)
	  (values id
		  (parse-integer number)
		  (parse-integer total)))))))

(defun summary-search-partial-messages (partial-id)
  (let (msgs)
    (dolist (msg *summary-message-list*)
      (multiple-value-bind (id number total)
	  (summary-parse-partial-message msg)
	(when (and id
		   (equal partial-id id))
	  (push (cons number msg) msgs))))
    (sort msgs #'< :key #'car)))

(defun summary-show-message-partial ()
  (interactive)
  (let* ((msg (summary-message-current))
	 (num (car msg))
	 (header (cdr msg))
	 msgs)
    (multiple-value-bind (id number total)
	(summary-parse-partial-message msg)
      (when id
	(let ((msgs (summary-search-partial-messages id)))
	  (unless (= (length msgs) total)
	    (kamail3-error "total number mismatch: ~D != ~D"
			   total
			   (length msgs)
			   ))
	  (save-excursion
	    (set-buffer (get-buffer-create *summary-buffer-partial-temp*))
	    (erase-buffer (selected-buffer))
	    (dolist (m msgs)
	      (let* ((header (cdr (cdr m)))
		     (file (summary-message-file header))
		     (beg (point))
		     end)
		(insert-file-noconv file)
		(when (junk::mail-goto-body-beg)
		(delete-region beg (point)))
		(goto-char (point-max))
		(loop
		  (setq end (point))
		  (or (forward-line -1) (return))
		  (or (eolp) (return)))
		(delete-region end (point-max))))
	    (write-region (point-min)
			  (point-max)
			  *summary-message-partial-temp*))
	  (delete-buffer *summary-buffer-partial-temp*)
	  (message-show *summary-message-partial-temp*)
	  )))))

(defun summary-sync ()
  (interactive)
  (let ((cmd (he-cmd-gather-draft *draft-directory*))
	res)
    (long-operation
      (kamail3-message "syncing pseudo index ...")
      (when (he-execute cmd)
	(kamail3-message "deleting draft files ...")
	(let ((files (directory *draft-directory*
				:wild "*.est"
				:absolute t)))
	  (dolist (file files)
	    (delete-file file)
	    (do-events))))
      (kamail3-message "syncing pseudo index ... done.")
      )))
