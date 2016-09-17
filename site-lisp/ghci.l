;;;  ghci.l -- GHCi (Glasgow Haskell Compiler interpreter) interface for xyzzy
;;;  Copyright (C) 2006 IMAI Takeo <usitukai@osk.3web.ne.jp>

;;; Shell3.lを参考にヒストリーとかその他色々付け加えてます
;;; 2012 Nao Fujinaga

(provide "ghci")
(in-package "editor")

(export '(ghci-mode
          ghci-shell-mode
          *ghci-mode-map*
          *ghci-window-height*
          *ghci-shell-mode-map*))

;;; ghci-mode（マイナーモード）
(defvar-local ghci-mode nil)
(defvar *ghci-command* "stack ghci")
(defvar *ghci-buffer-name* "*GHCi*")
(defvar *ghci-buffer* nil)
(defvar *ghci-process* nil)
(defvar *ghci-window-height* 35)

(defvar *ghci-mode-map*
  (let ((keymap (make-sparse-keymap)))
    (define-key keymap '(#\C-c #\C-i) 'ghci-load-file)
    (define-key keymap '(#\C-c #\C-q) 'ghci-quit)
    keymap))

(defun ghci-mode (&optional (arg nil sv))
  (interactive "p")
  (toggle-mode 'ghci-mode arg sv)
  (if ghci-mode
      (set-minor-mode-map *ghci-mode-map*)
    (unset-minor-mode-map *ghci-mode-map*))
  (update-mode-line t))

; Minor mode に登録
(pushnew '(ghci-mode . "GHCi") *minor-mode-alist* :key #'car)

; バッファがなければ作成、あれば返す
(defun ghci-get-buffer ()
  (setq *ghci-buffer* (get-buffer-create *ghci-buffer-name*)))

; ghciにバッファをロードする
(defun ghci-load-file (&optional f)
  (interactive)
  (let ((buf (ghci-get-buffer))                      ; 出力バッファ
        (win (get-buffer-window *ghci-buffer-name*)) ; 出力ウィンドウ
        (file (get-buffer-file-name))                ; バッファの関連付けらたファイル（フルパス）
        (bufname (buffer-name (selected-buffer)))
        proc)
    (save-buffer-dialog)
    (if (= (count-windows nil) 1)
        (split-window))
    (switch-to-buffer-other-window buf)
    (setq proc (buffer-process buf))
    (if (and proc (eq (process-status proc) ':run))
        proc
      (progn
        (erase-buffer buf)
        (setq proc (make-process *ghci-command*
                                 :output buf))
        (set-process-outcode proc *encoding-utf8n*))) ;;
    (when file
      (end-of-buffer)
      (ghci-send-string ":r")
;       (ghci-send-string (concat ":cd " (string-right-trim bufname file)))
;       (ghci-send-string (concat ":l " bufname))
      )
    (ghci-shell-mode)
    (end-of-buffer)))

; ghciのプロセスを返す
(defun ghci-get-process ()
  (if (deleted-buffer-p *ghci-buffer*)
      nil
    (buffer-process *ghci-buffer*)))

; ghciに文字列を送る
(defun ghci-send-string (str)
  (let ((proc (ghci-get-process)))
    (if proc
        (process-send-string proc (concat str "\n")) nil)))

; ghciバッファを消す
(defun ghci-delete-buffer ()
  (when (not (deleted-buffer-p *ghci-buffer*))
    (let ((proc (buffer-process *ghci-buffer*)))
      (when proc
        (kill-process proc)))
    (delete-buffer *ghci-buffer*)))

; ghciのプロセスが生きてるか否か
(defun ghci-alive-p ()
  (and
   (not (deleted-buffer-p *ghci-buffer*))
   (buffer-process *ghci-buffer*)))


; ghciをやめる
(defun ghci-quit ()
  (interactive)
  ; (when (ghci-alive-p)
  ; (ghci-send-string ":q"))
  (sleep-for 1)
  (ghci-delete-buffer)
  (delete-other-windows))


;;; ghci-shell-mode (GHCi とのインタラクティブバッファ用）
(defvar *ghci-shell-mode-hook* nil)
(defvar *ghci-shell-keyword-hash-table* nil)
(defvar *ghci-shell-keyword-file* "Haskell")                   ; Haskellのキーワードファイルに設定中
(defvar *ghci-shell-mode-syntax-table* *haskell-mode-syntax-table*) ; Haskellのシンタックステーブルに設定中

(defvar *ghci-shell-mode-map* nil)
(unless *ghci-shell-mode-map*
  (setq *ghci-shell-mode-map* (make-sparse-keymap))
  (define-key *ghci-shell-mode-map* #\RET 'ghci-shell-send-input)
  (define-key *ghci-shell-mode-map* '(#\C-c #\C-c) 'ghci-shell-send-interrupt)
  (define-key *ghci-shell-mode-map* #\Up 'ghci-shell-up)
  (define-key *ghci-shell-mode-map* #\Down 'ghci-shell-down)
  (define-key *ghci-shell-mode-map* #\Home 'ghci-shell-home)
  (define-key *ghci-shell-mode-map* #\S-Home 'ghci-shell-selection-beginning-of-line)
  (define-key *ghci-shell-mode-map* #\TAB 'ghci-dabbrev)
  (define-key *ghci-shell-mode-map* #\F10 'ghci-shell-first-error)
  )

(defvar *ghci-shell-prompt-regex* "^\\*?\\([A-Z]\\(\\w\\|_\\)*'*\\.\\)*[A-Z]\\(\\w\\|_\\)*'*\\( \\([A-Z]\\(\\w\\|_\\)*'*\\.\\)*[A-Z]\\(\\w\\|_\\)*'*\\)*>")
(defvar-local *ghci-shell-ret* "\n")
(defvar-local *ghci-shell-echo* nil)
(defvar *ghci-shell-history-backward* nil)
(defvar *ghci-shell-history-current* nil)
(defvar *ghci-shell-history-forward* nil)

; ghci-
(defun ghci-shell-first-error ()
  (interactive)
  (exchange-point-and-mark)
  (first-error)
  )

(defun ghci-shell-selection-beginning-of-line ()
  (interactive "p")
  (begin-selection)
  (ghci-shell-home))

(defun ghci-dabbrev ()
  (interactive)
  (if (and (> (point) (save-excursion (ghci-shell-home) (point)))
           (= (save-excursion (goto-eol) (point)) (point-max)))
      (dabbrev-expand)))

(defun ghci-shell-home ()
  (interactive)
  (if (save-excursion (goto-bol) (looking-at *ghci-shell-prompt-regex*))
      (goto-char (+ (match-end 0) 1))
    (goto-bol)))

(defun ghci-shell-up ()
  (interactive)
  (if (and (>= (point) (save-excursion (ghci-shell-home) (point)))
           (= (save-excursion
                (goto-eol)
                (point))
              (point-max)))
      (ghci-shell-history-previous)
    (previous-virtual-line)))

(defun ghci-shell-down ()
  (interactive)
  (if (and (>= (point) (save-excursion (ghci-shell-home) (point)))
           (= (save-excursion (goto-eol) (point)) (point-max)))
      (ghci-shell-history-next)
    (next-virtual-line)))

(defun ghci-shell-history-previous ()
  (interactive)
  (let ((command (ghci-shell-history-backward-pop)))
    (ghci-shell-prompt-reset)
    (when *ghci-shell-history-current*
      (ghci-shell-history-forward-push *ghci-shell-history-current*))
    (setq *ghci-shell-history-current* command)
    (if command
        (insert command)
      (message "No previous history.")
      )
    ))

(defun ghci-shell-history-next ()
  (interactive)
  (let ((command (ghci-shell-history-forward-pop)))
    (ghci-shell-prompt-reset)
    (when *ghci-shell-history-current*
      (ghci-shell-history-backward-push *ghci-shell-history-current*))
    (setq *ghci-shell-history-current* command)
    (if command
        (insert command)
      (message "No next history.")
      )
    ))

(defun ghci-shell-history-search-backward ()
  (interactive)
  (let ((curp (point))
        beg)
    (save-excursion
      (ghci-shell-goto-bol)
      (setq beg (point))
      (when (= beg curp)
        (return-from ghci-shell-history-search-backward))
      )
    (do-completion beg curp :list *ghci-shell-history-backward*)
    ))

(defun ghci-shell-prompt-reset ()
  (interactive)
  (ghci-shell-home)
  (delete-region (point) (save-excursion (goto-eol) (point))))

(defun ghci-shell ()
  (interactive)
  (set-buffer (get-buffer-create "*ghci-shell*"))
  (let ((proc (buffer-process (selected-buffer))))
    (and proc (eq (process-status proc) ':run)
         (return-from ghci-shell t)))
  (goto-char (point-max))
  (ghci-shell-mode)
  (make-process *eghci-shell* :output (selected-buffer)))

(defun ghci-shell-send-input ()
  (interactive)
  (let ((process (buffer-process (selected-buffer)))
        start end prompt)
    (when (and process
               (eq (process-status process) :run))
      (cond
       ((>= (point) (marker-point (process-marker process)))
        (setq start (marker-point (process-marker process)))
        (setq end (save-excursion (goto-eol) (point))))
       (t
        (return-from ghci-shell-send-input nil)))
      (let ((cmd (buffer-substring start end)))
        (cond ((eobp)
               (if *ghci-shell-echo*
                   (delete-region start end)
                 (insert "\n")))
              (t
               (goto-char (point-max))
               (or (bolp)
                   (insert "\n"))
               (and prompt (insert prompt))
               (unless *ghci-shell-echo*
                 (insert cmd "\n"))))
        (set-marker (process-marker process))
        ;; add history
        (when (and cmd
                   (< 0 (length cmd)))
          (ghci-shell-history-save)
          (ghci-shell-history-backward-push cmd))
        (process-send-string process (concatenate 'string cmd *ghci-shell-ret*))))))

(defun ghci-shell-alternate-send-input ()
  (interactive)
  (let ((*ghci-shell-echo* (if (equal *ghci-shell-echo* "\n") "\r" "\n")))
    (declare (special *ghci-shell-echo*))
    (ghci-shell-send-input)))

(setf (symbol-function 'ghci-shell-send-interrupt) #'kill-subprocess)

(defun ghci-shell-history-save ()
  (let (command)
    (when *ghci-shell-history-current*
      (ghci-shell-history-backward-push *ghci-shell-history-current*)
      (setq *ghci-shell-history-current* nil)
      )
    (while (setq command (ghci-shell-history-forward-pop))
      (ghci-shell-history-backward-push command)
      )
    ))

(defun ghci-shell-history-backward-push (command)
  (push (list command) *ghci-shell-history-backward*))

(defun ghci-shell-history-backward-pop ()
  (car (pop *ghci-shell-history-backward*)))

(defun ghci-shell-history-forward-push (command)
  (push (list command) *ghci-shell-history-forward*))

(defun ghci-shell-history-forward-pop ()
  (car (pop *ghci-shell-history-forward*)))

; mode definition
(defun ghci-shell-mode ()
  (interactive)
  (kill-all-local-variables)
  (setq mode-name "ghci-shell")
  (setq buffer-mode 'ghci-shell-mode)
  (use-keymap *ghci-shell-mode-map*)
  (use-syntax-table *ghci-shell-mode-syntax-table*)
  (and *ghci-shell-keyword-file* (null *ghci-shell-keyword-hash-table*)
       (setq *ghci-shell-keyword-hash-table*
             (load-keyword-file *ghci-shell-keyword-file*)))
  (when *ghci-shell-keyword-hash-table*
    (make-local-variable 'keyword-hash-table)
    (setq keyword-hash-table *ghci-shell-keyword-hash-table*))
  (setq need-not-save t)
  (setq auto-save nil)
  (setq kept-undo-information nil)
  (run-hooks '*ghci-shell-mode-hook*))
