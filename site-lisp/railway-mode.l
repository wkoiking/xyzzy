(provide "railway")

(in-package "editor")
(export '(railway-mode))

(defpackage "railway-mode"
  (:nicknames "rail")
  (:use "lisp" "editor"))

(require "indent-to-token+")

(in-package "rail")
(export '(*railway-mode-map*
          *railway-mode-hook*
          *railway-use-setext-style*
          *h1-style* *h2-style* *h3-style* *h4-style* *h5-style* *h6-style*
          *italic-style* *bold-style* *italic-bold-style*
          *listmarker-style* *code-style*
          ))

(defparameter *h1-style* '(:color 8 10 :line :bold))
(defparameter *h2-style* '(:color 8 11 :line :bold))
(defparameter *h3-style* '(:color 8 0 :underline :bold))
(defparameter *h4-style* '(:color 8 0 :underline))
(defparameter *h5-style* '(:color 8 0))
(defparameter *h6-style* '(:color 8 0))
(defparameter *italic-style* '(:keyword 2))

(defun make-regexp-keyword-list ()
  (compile-regexp-keyword-list
   `(("^# .+" nil ,*h1-style*)
     ("^## .+" nil ,*h2-style*)
     ("^### .+" nil ,*h3-style*)
     ("^#### .+" nil ,*h4-style*)
     ("^##### .+" nil ,*h5-style*)
     ("^###### .+" nil ,*h6-style*)
     ("\\*[^\n*]+\\*" nil ,*italic-style*)
     )))

(defvar *railway-prefix-key* '(#\C-c))

(defvar *railway-mode-map* nil)
(defun setup-keymap ()
  (unless *railway-mode-map*
    (let ((m (setq *railway-mode-map* (make-sparse-keymap))))
      (flet ((pk (c) `(,@*railway-prefix-key* ,c)))
        (define-key m #\RET 'railway-newline)
        (define-key m #\RET 'railway-newline-and-indent) ;
        (define-key m (pk #\>) 'railway-shift-region)
        (define-key m (pk #\<) 'railway-unshift-region)
        (define-key m #\C-e 'align-table)
        ; 		(define-key m #\TAB 'railway-tab-indent)
        )))) ;

; (defvar *railway-mode-hook* nil)
(add-hook
 '*railway-mode-hook*
 #'(lambda ()
     (ed::set-buffer-local 'indent-tabs-mode nil)))

(defun railway-header-p ()
  (save-excursion
    (goto-bol)
    (railway-quote-skip)
    (when (looking-at "\\(#\\{1,6\\}\\)[ \t]*[^#]+[ \t]*#*$")
      (length (match-string 1)))))

(defun railway-search-preceding-header ()
  (save-excursion
    (while (forward-line -1)
      (let ((p (railway-header-p)))
        (and p (return p))))))

(defun railway-tab-indentation-exists-p ()
  (save-excursion
    (goto-char (point-min))
    (scan-buffer "^\t+[^ \t]" :regexp t)))

(defun railway-shift-region (start end &optional (num 1))
  (interactive "*r\np")
  (when (eq *prefix-args* 'universal-argument)
    (setq num -1))
  (let ((col (* num (if (railway-tab-indentation-exists-p)
                        (tab-columns)
                      4))))
    (shift-region start end col)))

(defun railway-unshift-region (start end &optional (num -1))
  (interactive "*r\np")
  (when (> num 0)
    (setq num (- num)))
  (railway-shift-region start end num))

(defun railway-search-header (&optional (dir 1))
  (save-excursion
    (while (forward-line dir)
      (when (railway-header-p)
        (return (point))))))

(defun railway-next-header ()
  (interactive)
  (let ((p (railway-search-header)))
    (when p
      (goto-char p))))

(defun railway-previous-header ()
  (interactive)
  (let ((p (railway-search-header -1)))
    (when p
      (goto-char p))))

;;; mode definition

(defvar-local ed::*railway-mode* nil)

(defun ed::railway-mode (&optional (arg nil sv))
  (interactive "p")
  (setup-keymap)
  (use-keymap *railway-mode-map*)
  (unless (local-variable-p '#0=regexp-keyword-list)
    (make-local-variable '#0#))
  (setq #0# (append #0# (make-regexp-keyword-list))
        buffer-mode 'ed::railway-mode
        mode-name "Railway")
  (run-hooks '*railway-mode-hook*)
  t)

;; 付け足し
(defun align-table ()
  (interactive)
  (indent-to-token+ "\\(\\w\\|[-_.<>#]\\)+\\|([0-9]+ *, *[0-9]+ *)")
  )

; offside-modeのコピー
;; ここでは使っていないけどね（普通にTABを使いたいため）
(defun calc-offside-indent ()
  (if (eq (current-line-number) 1)
      0
    (save-excursion
      (previous-line)
      (goto-bol)
      (skip-chars-forward " \t")
      (let ((is-eol (eolp)))
        (if is-eol
            0
          (current-column)
          )
        )
      )
    ))

(defun railway-indent-line ()
  (interactive "*")
  (let ((column (calc-offside-indent)))
    (smart-indentation column)))

(defun railway-tab-indent ()
  (interactive "*")
  (let ((column (calc-offside-indent)))
    (cond
     ((< (current-column) column)
      (smart-indentation column))
     ((= (current-column) column)
      (smart-indentation (+ column 4)))
     ((> (current-column) column)
      (smart-indentation column)
      (goto-bol)
      (skip-chars-forward " \t")
      ))))

(defun railway-newline-and-indent (&optional (arg 1))
  (interactive "*p")
  (delete-trailing-spaces)
  (insert #\LFD arg)
  (railway-indent-line))
