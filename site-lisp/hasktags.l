;;; -*- Mode: lisp -*-
; hasktags.l
;
; hasktags.exe を使用してタグジャンプを行う機能を提供
; hasktags 日本語対応版 http://hp.vector.co.jp/authors/VA025040/hasktags/
;
; by MATSUOKA Hiroshi <matsuoka@fun.sci.fukuoka-u.ac.jp>
;
; .xyzzy sample
; (load-library "hasktags")
; ; hasktags.exe へのパス
; (setf *hasktags-command-path* (merge-pathnames "etc/hasktags.exe" (si:system-root)))
; ; hasktags.exe へのその他のオプション
; ;(setf *hasktags-command-option* "")
; (global-set-key #\M-. 'hasktags-jump-tag)
; (global-set-key #\M-\, 'hasktags-back-tag-jump)
; (global-set-key #\M-/ 'hasktags-make-tags-file)
; ;(global-set-key #\M-/ 'hasktags-make-tags-file-recursive)
;
; [2005/09/28] matsuoka@fun.sci.fukuoka-u.ac.jp
; 作成。
; [2007/12/02] matsuoka@fun.sci.fukuoka-u.ac.jp
; ハッシュベースから隠しバッファでの検索に切り替え
; [2007/12/08] matsuoka@fun.sci.fukuoka-u.ac.jp
; NANRI さんの指摘を受けて tags で / がクォートされてるのをはずす
;
(provide "hasktags")
(in-package "editor")
(export '(*hasktags-command*
          *hasktags-tags-buffer-name*
          *hasktags-tags-stack*
          hasktags-make-tags-file
          hasktags-jump-tag
          hasktags-back-tag-jump
          hasktags-load-tags-file
          ))

; hasktags.exe 
(defvar *hasktags-command* "hasktags.exe")
; tags を読み込むバッファ名のプレフィクス
(defvar *hasktags-tags-buffer-prefix* " *hasktags ")
; バッファローカルなタグスタック
(defvar *hasktags-tags-stack* nil)
; tags として使用するバッファ名
(defvar *hasktags-tags-buffer-name* nil)

; tags ファイルの作成して、バッファに読み込む
(defun hasktags-make-tags-file (dir)
  (interactive "DMakeTagsTo: " :default01 (if (get-buffer-file-name) (pathname-directory (get-buffer-file-name)) ""))
  (if (call-process (concat *hasktags-command* " " "-c" " " (map-slash-to-backslash dir)) :exec-directory dir :wait t :show :minimize)
      (let* ((file-path (merge-pathnames "tags" dir))
             (buf-name (concat " " file-path)))
        (progn
          (setf *hasktags-tags-buffer-name* buf-name)
          (get-buffer-create buf-name)
          (erase-buffer buf-name)
          (save-excursion
            (switch-to-buffer buf-name)
            (make-local-variable 'need-not-save)
            (setf need-not-save t)
            (make-local-variable 'auto-save)
            (setf auto-save nil)
            (insert-file file-path)
            (message "loaded tags"))
          t))
    (progn
      (message "failed to loading tags")
      nil)))

; バッファの現在行と行頭行末のポイントを返す
(defun hasktags-get-current-line ()
  (let ((end (progn (goto-eol) (point)))
        (begin (progn (goto-bol) (point))))
    (cons (buffer-substring begin end) (cons begin end))))

; tagsバッファを読んで一致するタグのリストを返す
(defun hasktags-search-tag (word)
  (save-excursion
    (switch-to-buffer *hasktags-tags-buffer-name*)
    (goto-char 0)
    (let ((tags nil)
          (regexp (compile-regexp (format nil "^~A\t\\([^\t]+\\)\t\\([1-9][0-9]*\\)\n" (regexp-quote word)))))
      (while (scan-buffer regexp :no-dup t :regexp t)
        (push (cons (match-string 1) (match-string 2)) tags))
      tags)))

; タグジャンプ
(defun hasktags-jump-tag ()
  (interactive)
  (if *hasktags-tags-buffer-name*
    (let* ((word (hasktags-get-current-identifier))
           (tags (hasktags-search-tag word)))
      (case (length tags)
        (0 (message "unknown identifier ~A" word))
        (1 (hasktags-goto-tag (car tags)))
        (2 (hasktags-goto-tag (car tags))) ;
        (t (hasktags-select-next-tag tags))))
    (message "no tags buffer")))

; バックタグジャンプ
(defun hasktags-back-tag-jump ()
  (interactive)
  (if (null *hasktags-tags-stack*)
      (message "no stack for back tag jump")
    (hasktags-goto-tag1 (pop *hasktags-tags-stack*))))

; 任意のタグに移動(スタックを積む)
(defun hasktags-goto-tag (tag)
  (hasktags-push-current-marker)
  (hasktags-goto-tag1 tag))

; 任意のタグに移動
(defun hasktags-goto-tag1 (tag)
  (let ((file (car tag))
        (marker (cdr tag)))
    (find-file file)
    (goto-char 0)
    (cond
     ((integerp marker)
      (goto-char marker))
     ((string-match "^[1-9][0-9]*$" marker)
      (goto-line (parse-integer marker)))
     (t
      (message "invalid tag")))))

; 複数のタグから次のジャンプ先を決定する
(defun hasktags-select-next-tag (tags)
  (hasktags-goto-tag (hasktags-select-tag tags)))

; 複数のタグよりミニバッファでジャンプ先を選択する。tag を返却
(defun hasktags-select-tag (tags)
  (let ((l nil) (i 0) (dir (pathname-directory *hasktags-tags-buffer-name*)))
    (save-excursion
      (switch-to-buffer *hasktags-tags-buffer-name*)
      (dolist (x tags)
        (push (format nil "~2A:\"$~A\": ~A"
                      i
                      (substring (car x) (+ 1 (length dir)))
                      (if (stringp (cdr x)) (string-trim " 	/^$" (cdr x)) (cdr x)))
              l)
        (setf i (+ i 1))))
    (let ((selected
           (completing-read
            "Select Tag: " l :must-match t :default (if (= (length l) 1) (car l) nil) :history nil)))
      (nth (parse-integer (string-trim " " (car (split-string selected ":")))) tags))))

; 現在位置をスタックに積む
(defun hasktags-push-current-marker ()
  (push (cons (get-buffer-file-name) (point))
        *hasktags-tags-stack*))

; カーソル位置の識別子を xyzzy の syntax に合わせて取得
(defun hasktags-get-current-identifier ()
  (save-excursion
    (buffer-substring
     (progn
       (skip-syntax-spec-forward "w_")
       (point))
     (progn
       (skip-syntax-spec-backward "w_")
       (point)))))
