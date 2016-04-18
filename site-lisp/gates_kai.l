;;; gates_kai.l
(provide "gates_kai.l")

(in-package "editor")

;; winkey
(defvar *default-selection-keymap*
  (let ((keymap (make-sparse-keymap)))
    (define-key keymap #\C-c 'copy-selection-to-clipboard)
    (define-key keymap #\C-x 'kill-selection-to-clipboard)
    keymap))

(setq-default selection-keymap *default-selection-keymap*)
(global-set-key #\C-z 'undo)
(global-set-key #\C-v 'paste-from-clipboard)
(define-key esc-map #\C-x #'ctl-x-prefix)
(define-key esc-map #\C-c #'specific-command-prefix)

; files
; (global-set-key #\C-n 'new-file)
; (global-set-key #\C-o 'open-file-dialog)
(global-set-key #\C-o 'open-filer)
(global-set-key #\C-s 'save-buffer-dialog)
; (global-set-key #\C-p 'print-selected-buffer-dialog)
(global-set-key #\M-F4 'kill-xyzzy)      ; Alt+F4
(global-set-key #\C-b 'select-buffer)

; edit
(global-set-key #\C-y 'redo)
(global-set-key #\C-a 'selection-whole-buffer)
(global-set-key #\C-w 'kill-selected-buffer)
; (global-set-key #\C-g 'goto-line)

; search
(global-set-key #\C-f 'search-dialog)
(global-set-key #\C-r 'replace-dialog)

; move
; chars
(global-set-key #\S-Left 'selection-backward-char)
(global-set-key #\S-Right 'selection-forward-char)
(global-set-key #\S-C-Up 'selection-previous-virtual-line)
(global-set-key #\S-C-Down 'selection-next-virtual-line)
; lines
(global-set-key #\S-Up 'selection-previous-virtual-line)
(global-set-key #\S-Down 'selection-next-virtual-line)
; bol/eol
(global-set-key #\Home 'beginning-of-line)
(global-set-key #\End 'end-of-line)
(global-set-key #\S-Home 'selection-beginning-of-line)
(global-set-key #\S-End 'selection-end-of-line)
; words
; (global-set-key #\C-Left 'backward-word)
(global-set-key #\C-Left 'my-backward-word)
; (global-set-key #\C-Right 'forward-word)
(global-set-key #\C-Right 'my-forward-word)
; (global-set-key #\S-C-Left 'selection-backward-word)
(global-set-key #\S-C-Left 'my-selection-backward-word)
; (global-set-key #\S-C-Right 'selection-forward-word)
(global-set-key #\S-C-Right 'my-selection-forward-word)
; page
(global-set-key #\S-PageUp 'selection-previous-page)
(global-set-key #\S-PageDown 'selection-next-page)
(global-set-key #\C-PageUp 'previous-buffer)       ; Shift+Ctrl+Tab
(global-set-key #\C-PageDown 'next-buffer)             ; Ctrl+Tab

; bob/eob
(global-set-key #\C-Home 'beginning-of-buffer)
(global-set-key #\C-End 'end-of-buffer)
(global-set-key #\S-C-Home 'selection-beginning-of-buffer)
(global-set-key #\S-C-End 'selection-end-of-buffer)
; buffers
(global-set-key #\C-F4 'close-selected-buffer)
(global-set-key #\S-C-F24 'previous-buffer)       ; Shift+Ctrl+Tab
(global-set-key #\C-F24 'next-buffer)             ; Ctrl+Tab
(set-extended-key-translate-table exkey-S-C-tab #\S-C-F24)
(set-extended-key-translate-table exkey-C-tab #\C-F24)

; ファンクションキー
(global-set-key #\F1 'describe-bindings)
(global-set-key #\F2 'repeat-backward-search)
(global-set-key #\F3 'repeat-forward-search)
(global-set-key #\F7 'ispell-buffer)
; (global-set-key #\F8 'open-file-dialog)
(global-set-key #\F8 'open-filer)
(global-set-key #\F12 'save-as-dialog)

; word単位で移動（改良版）
(defun my-forward-word ()
  (interactive "p")
  (let ((x (following-char)))
	(cond
	 ((alpha-num-p x) (while (and (alpha-num-p (following-char)) (< (point) (point-max)))
						(forward-char)))
	 ((syntax-whitespace-p x) (while (and (syntax-whitespace-p (following-char)) (< (point) (point-max)))
								(forward-char)))
	 ((syntax-symbol-p x) (while (and (syntax-symbol-p (following-char)) (< (point) (point-max)))
							(forward-char)))
	 ((open-paren-p x) (goto-matched-parenthesis) (forward-char))
	 (t (forward-char)))))

(defun my-backward-word ()
  (interactive "p")
  (let ((x (preceding-char)))
	(cond
	 ((alpha-num-p x) (while (and (alpha-num-p (preceding-char)) (> (point) (point-min)))
						(backward-char)))
	 ((syntax-whitespace-p x) (while (and (syntax-whitespace-p (preceding-char)) (> (point) (point-min)))
								(backward-char)))
	 ((syntax-symbol-p x) (while (and (syntax-symbol-p (preceding-char)) (> (point) (point-min)))
							(backward-char)))
	 ((close-paren-p x) (backward-char) (goto-matched-parenthesis))
	 (t (backward-char)))))

(defun alpha-num-p (x)
  (or (alpha-char-p x) (digit-char-p x) (equal #\' x)))

(defun open-paren-p (x)
  (or (equal x #\() (equal x #\{) (equal x #\[)))

(defun close-paren-p (x)
  (or (equal x #\)) (equal x #\}) (equal x #\])))

(defun my-selection-forward-word ()
  (interactive "p")
  (begin-selection)
  (my-forward-word))

(defun my-selection-backward-word (&optional (arg 1))
  (interactive "p")
  (begin-selection)
  (my-backward-word))

;;; end of gate_kai.l
 