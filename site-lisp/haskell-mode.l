;;; haskell mode

(provide "haskell-mode")
(in-package "editor")

(require "indent-to-token+")

(export '(haskell-mode
          *haskell-keyword-file* *haskell-mode-hook*
          *haskell-mode-syntax-table*
          *haskell-regexp-keyword-list*
          *haskell-mode-map*))

(defvar *haskell-mode-hook* nil)
(defvar *haskell-keyword-hash-table* nil)
(defvar *haskell-keyword-file* "Haskell")
(defparameter *function-style* '(:color 0))
(defparameter *constractor-style* '(:keyword 2))
(defparameter *comment-style* '(:keyword :comment))
(defparameter *symbol-style* '(:keyword 1))
(defparameter *char-style* '(:keyword :string))
(defparameter *paren-style* '(:color 0))
(defvar *haskell-window-height* nil)

(defvar *haskell-regexp-keyword-list* nil)
(unless *haskell-regexp-keyword-list*
  (setq *haskell-regexp-keyword-list*
        (compile-regexp-keyword-list
         `(("\\([A-Z]\\(\\w\\|_\\)*'*\\.\\)*[a-z]\\(\\w\\|_\\)*'*" nil ,*function-style*)
           ("\\([A-Z]\\(\\w\\|_\\)*'*\\.\\)*[A-Z]\\(\\w\\|_\\)*'*" nil ,*constractor-style*)
           ("--" nil ,*comment-style*)
           ("\\_b[-!^=;|<>~@?:+/$#&*.]*" nil ,*symbol-style*)
           ("'\\(\\\\[ntr'\\]\\|[^\\]\\)'" nil ,*char-style*)
           ("[][(){},]" nil ,*paren-style*)
           ))))

(defvar *haskell-skip-keyword-regex* "\\(where\\|let\\)")

(defvar *haskell-mode-syntax-table* nil)
(unless *haskell-mode-syntax-table*
  (setq *haskell-mode-syntax-table* (make-syntax-table))
  (set-syntax-string *haskell-mode-syntax-table* #\")

  (set-syntax-escape *haskell-mode-syntax-table* #\\)

  (set-syntax-whitespace *haskell-mode-syntax-table* #\SPC)
  (set-syntax-whitespace *haskell-mode-syntax-table* #\TAB)
  (set-syntax-whitespace *haskell-mode-syntax-table* #\RET)
  
  (set-syntax-symbol *haskell-mode-syntax-table* #\!)
  (set-syntax-symbol *haskell-mode-syntax-table* #\=)
  (set-syntax-symbol *haskell-mode-syntax-table* #\:)
  (set-syntax-symbol *haskell-mode-syntax-table* #\|)
  (set-syntax-symbol *haskell-mode-syntax-table* #\>)
  (set-syntax-symbol *haskell-mode-syntax-table* #\<)
  (set-syntax-symbol *haskell-mode-syntax-table* #\+)
  (set-syntax-symbol *haskell-mode-syntax-table* #\-)
  (set-syntax-symbol *haskell-mode-syntax-table* #\/)
  (set-syntax-symbol *haskell-mode-syntax-table* #\^)
  (set-syntax-symbol *haskell-mode-syntax-table* #\$)
  (set-syntax-symbol *haskell-mode-syntax-table* #\#)
  (set-syntax-symbol *haskell-mode-syntax-table* #\&)
  (set-syntax-symbol *haskell-mode-syntax-table* #\*)
  (set-syntax-symbol *haskell-mode-syntax-table* #\.)
  (set-syntax-symbol *haskell-mode-syntax-table* #\?)
  (set-syntax-symbol *haskell-mode-syntax-table* #\@)  
  (set-syntax-match *haskell-mode-syntax-table* #\( #\))
  (set-syntax-match *haskell-mode-syntax-table* #\[ #\])
  (set-syntax-match *haskell-mode-syntax-table* #\{ #\})
  
  (set-syntax-start-multi-comment *haskell-mode-syntax-table* "{-")
  (set-syntax-end-multi-comment *haskell-mode-syntax-table* "-}")
  
  (set-syntax-start-c++-comment *haskell-mode-syntax-table* #\-)
  
  (set-syntax-end-c++-comment *haskell-mode-syntax-table* #\LFD)
  )

(defvar *haskell-mode-map* nil)
(unless *haskell-mode-map*
  (setq *haskell-mode-map* (make-sparse-keymap))
  (define-key *haskell-mode-map* #\TAB 'haskell-tab-indent-toggle)
  (define-key *haskell-mode-map* #\RET 'haskell-newline-and-indent)
  (define-key *haskell-mode-map* #\C-e 'align-equation)
  (define-key *haskell-mode-map* '(#\C-c #\C-c) 'runhaskell))

; offside系
(defun offside ()
  (save-excursion
    (goto-bol)
    (skip-chars-forward " \t")
    (when (looking-at *haskell-skip-keyword-regex*)
        (goto-char (match-end 0))
      (skip-chars-forward " \t"))
    (current-column)))

(defun previous-offside ()
  (if (eq (current-line-number) 1)
      0
    (save-excursion
      (previous-line)
      (offside))))

(defun haskell-indent-line ()
  (interactive "*")
  (smart-indentation (previous-offside)))

(defun haskell-tab-indent-toggle ()
  (interactive "*")
  (let ((prev-off (previous-offside))
        (current-off (offside))
        (current (current-column)))
    (cond ((= current current-off)
           (cond
            ((< current prev-off)
             (smart-indentation prev-off))
            ((= current prev-off)
             (smart-indentation (+ prev-off 4)))
            ((> current prev-off)
             (smart-indentation (- prev-off 4))
             (goto-bol)
             (skip-chars-forward " \t"))))
          (t (smart-indentation prev-off)
             (goto-bol)
             (skip-chars-forward " \t")))))

(defun haskell-newline-and-indent (&optional (arg 1))
  (interactive "*p")
  (delete-trailing-spaces)
  (insert #\LFD arg)
  (haskell-indent-line))

; mode definition
(defun haskell-mode ()
  (interactive)
  (kill-all-local-variables)
  (setq mode-name "Haskell")
  (setq buffer-mode 'haskell-mode)
  (use-syntax-table *haskell-mode-syntax-table*)
  (use-keymap *haskell-mode-map*)
  (and *haskell-keyword-file* (null *haskell-keyword-hash-table*)
       (setq *haskell-keyword-hash-table*
             (load-keyword-file *haskell-keyword-file*)))
  (when *haskell-keyword-hash-table*
    (make-local-variable 'keyword-hash-table)
    (setq keyword-hash-table *haskell-keyword-hash-table*))
  (when *haskell-regexp-keyword-list*
    (make-local-variable 'regexp-keyword-list)
    (setq regexp-keyword-list *haskell-regexp-keyword-list*))
  (run-hooks '*haskell-mode-hook*))

(add-hook
 '*haskell-mode-hook*
 #'(lambda ()
     (ed::set-buffer-local 'indent-tabs-mode nil)))

(defun align-equation ()
  (interactive)
  (indent-to-token+ " = "))

(defun runhaskell ()
  (interactive)
  (save-buffer-dialog)
  (if (= (count-windows nil) 1)
        (split-window))
  (execute-subprocess (concat "runhaskell " "\"" (get-buffer-file-name) "\"")))

; 今はキーバインドしていない

(defun ghc-compile ()
  (interactive)
  (save-buffer-dialog)
  (if (= (count-windows nil) 1)
      (split-window (- (screen-height) *haskell-window-height* 2)))
  (execute-subprocess (concat "ghc --make -O2 " "\"" (get-buffer-file-name) "\"")))
