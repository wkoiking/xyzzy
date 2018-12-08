;; -*- mode:lisp; package:markdown-mode -*-

;; markdown.l ( http://www.geocities.jp/kiaswebsite/xyzzy/markdown.html )
;; Rev: 227 ‚ðŒ³‚É‰ü•Ï
;;
;; License
;; =======
;;
;; Copyright (c) 2011 Yousuke Ushiki <citrus.yubeshi@gmail.com>
;; Copyright (c) 2005,2006 kia
;;
;; Permission is hereby granted, free of charge, to any person obtaining a
;; copy of this software and associated documentation files (the "Software"),
;; to deal in the Software without restriction, including without
;; limitation the rights to use, copy, modify, merge, publish, distribute,
;; sublicense, and/or sell copies of the Software, and to permit persons to
;; whom the Software is furnished to do so, subject to the following
;; conditions:
;;
;; The above copyright notice and this permission notice shall be included
;; in all copies or substantial portions of the Software.
;;
;; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
;; OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
;; MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
;; IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
;; CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
;; TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
;; SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
;;
;;

(provide "markdown")

(in-package "editor")
(export '(markdown-mode))

(defpackage "markdown-mode"
  (:nicknames "md")
  (:use "lisp" "editor"))

(require "indent-to-token+")

(in-package "md")
(export '(*markdown-mode-map*
          *markdown-mode-hook*
          *markdown-use-setext-style*
          *markdown-mode-syntax-table*
          *h1-style* *h2-style* *h3-style* *h4-style* *h5-style* *h6-style*
          *italic-style* *bold-style* *italic-bold-style*
          *listmarker-style* *code-style*
          *markdown-command*
          *markdown-window-height*
          ))

(defvar *markdown-mode-hook* nil)
(defvar *markdown-keyword-hash-table* nil)
(defvar *markdown-keyword-file* "Markdown")

(defvar *markdown-mode-syntax-table* nil)
(unless *markdown-mode-syntax-table*
  (setq *markdown-mode-syntax-table* (make-syntax-table))
  (set-syntax-match *markdown-mode-syntax-table* #\( #\))
  (set-syntax-match *markdown-mode-syntax-table* #\[ #\])
  (set-syntax-match *markdown-mode-syntax-table* #\{ #\})
  (set-syntax-start-multi-comment *markdown-mode-syntax-table* "<!")
  (set-syntax-end-multi-comment *markdown-mode-syntax-table* "->")
  (set-syntax-start-c++-comment *markdown-mode-syntax-table* #\>)
  (set-syntax-end-c++-comment *markdown-mode-syntax-table* #\LFD)
  )

(defvar *markdown-mode-map* nil)
(unless *markdown-mode-map*
  (setq *markdown-mode-map* (make-sparse-keymap))
  (define-key *markdown-mode-map* '(#\C-c #\C-c) 'pandoc-compile-to-html)
  (define-key *markdown-mode-map* '(#\C-c #\C-s) 'pandoc-compile-to-simple-html)
;   (define-key *markdown-mode-map* '(#\C-c #\e) 'pandoc-compile-to-pdf-e)
;   (define-key *markdown-mode-map* '(#\C-c #\j) 'pandoc-compile-to-pdf-j)
  (define-key *markdown-mode-map* '(#\C-c #\d) 'pandoc-compile-to-docx)
  (define-key *markdown-mode-map* '(#\C-c #\b) 'pandoc-compile-to-blog)
  (define-key *markdown-mode-map* #\C-e 'align-table)
  (define-key *markdown-mode-map* #\C-- 'make-line)
  (define-key *markdown-mode-map* #\C-= 'make-double-line)
  (define-key *markdown-mode-map* #\TAB 'markdown-tab-indent-toggle)
  (define-key *markdown-mode-map* #\RET 'markdown-newline-and-indent))

(defparameter *h1-style* '(:color 8 10 :line :bold))
(defparameter *h2-style* '(:color 8 11 :line :bold))
(defparameter *h3-style* '(:color 8 0 :underline :bold))
(defparameter *h4-style* '(:color 8 0 :underline))
(defparameter *h5-style* '(:color 8 0))
(defparameter *h6-style* '(:color 8 0))
(defparameter *deletion-style* '(:color 6 0))
(defparameter *addition-style* '(:color 2 0))
(defparameter *normal-text-style* '(:color 1 0))
(defparameter *listmarker-style* '(:keyword 0 :bold))
(defparameter *italic-style* '(:keyword 1 :bold))
(defparameter *bold-style* '(:keyword 1 :bold))
(defparameter *italic-bold-style* '(:keyword 1 :bold))
(defparameter *code-style* '(:keyword 2))

(defvar *markdown-regexp-keyword-list* nil)
(unless *markdown-regexp-keyword-list*
  (setq *markdown-regexp-keyword-list*
        (compile-regexp-keyword-list
         `(("^# .+" nil ,*h1-style*)
           ("^## .+" nil ,*h2-style*)
           ("^### .+" nil ,*h3-style*)
           ("^#### .+" nil ,*h4-style*)
           ("^##### .+" nil ,*h5-style*)
           ("^###### .+" nil ,*h6-style*)
           
           ("^\\+.+" nil ,*addition-style*)
           ("^\\-\\-.+" nil ,*normal-text-style*)
           ("^\\-.+" nil ,*deletion-style*)
           
           ("^[ \t]*[*+-] " nil ,*listmarker-style*)
           
           ("^```[^`]+\n```$" nil ,*code-style*)
           ("`[^\n`]+`" nil ,*code-style*)
           
           ("\\*\\*\\*[^\n*]+\\*\\*\\*" nil ,*italic-bold-style*)
           ("\\*\\*[^\n*]+\\*\\*" nil ,*bold-style*)
           ("\\*[^\n*]+\\*" nil ,*italic-style*)
           
           ("[\n\t ]___[^\n_]+___[\n\t ]" nil ,*italic-bold-style*)
           ("[\n\t ]__[^\n_]+__[\n\t ]" nil ,*bold-style*)
           ("[\n\t ]_[^\n_]+_[\n\t ]" nil ,*italic-style*)
           ))))

(defvar *markdown-command* (concat (user-homedir-pathname) "bin/pandoc.exe"))
(defvar *pandoc-crossref-command* (concat (user-homedir-pathname) "bin/pandoc-crossref.exe"))
  
(defvar *markdown-window-height* 20)

(defvar *markdown-default-horizontal-rule* "*****")
(defvar *markdown-quote-re* "\\(?:[ \t]*>\\)+[ \t]*")
(defvar *markdown-use-setext-style* nil)

(defun pandoc-compile-to-html ()
  (interactive)
  (save-buffer-dialog)
  (if (= (count-windows nil) 1)
      (split-window (- (screen-height) *markdown-window-height* 2)))
  (execute-subprocess (concat *markdown-command*
                              " "
                              "--filter " *pandoc-crossref-command* " "
                              "-o \"" (string-right-trim "md" (get-buffer-file-name)) "html" "\" "
                              "--webtex -s -S -N --toc --toc-depth=5 -H " (user-homedir-pathname) "css/markdown.css "
                              "\"" (buffer-name (selected-buffer)) "\"")))

(defun pandoc-compile-to-simple-html ()
  (interactive)
  (save-buffer-dialog)
  (if (= (count-windows nil) 1)
      (split-window (- (screen-height) *markdown-window-height* 2)))
  (execute-subprocess (concat *markdown-command*
                              "--filter " *pandoc-crossref-command* " "
                              "-o \"" (string-right-trim "md" (get-buffer-file-name)) "html" "\" "
                              "--webtex -s -S -N -H " (user-homedir-pathname) "css/markdown.css "
                              "\"" (buffer-name (selected-buffer)) "\"")))

(defun pandoc-compile-to-blog ()
  (interactive)
  (save-buffer-dialog)
  (if (= (count-windows nil) 1)
      (split-window (- (screen-height) *markdown-window-height* 2)))
  (execute-subprocess (concat *markdown-command*
                              "--filter " *pandoc-crossref-command* " "
                              "-o \"" (string-right-trim "md" (get-buffer-file-name)) "html" "\" "
                              "--webtex -S "
                              "\"" (buffer-name (selected-buffer)) "\"")))

(defun pandoc-compile-to-docx ()
  (interactive)
  (save-buffer-dialog)
  (if (= (count-windows nil) 1)
      (split-window (- (screen-height) *markdown-window-height* 2)))
  (execute-subprocess (concat *markdown-command*
                              " "
                              "--filter " *pandoc-crossref-command* " "
                              "-o \"" (string-right-trim "md" (get-buffer-file-name)) "docx" "\" "
                              "--toc -N -f markdown -t docx "
                              "\"" (buffer-name (selected-buffer)) "\"")))

; (defun pandoc-compile-to-pdf-e ()
;   (interactive)
;   (save-buffer-dialog)
;   (if (= (count-windows nil) 1)
;       (split-window (- (screen-height) *markdown-window-height* 2)))
;   (execute-subprocess (concat *markdown-command*
;                               "-o \"" (string-right-trim "md" (get-buffer-file-name)) "pdf" "\" "
;                               "-s -S -N --toc "
;                               "\"" (buffer-name (selected-buffer)) "\"")))
;
; (defun pandoc-compile-to-pdf-j ()
;   (interactive)
;   (save-buffer-dialog)
;   (if (= (count-windows nil) 1)
;       (split-window (- (screen-height) *markdown-window-height* 2)))
;   (execute-subprocess (concat *markdown-command*
;                               "-o \"" (string-right-trim "md" (get-buffer-file-name)) "pdf" "\" "
;                               "-s -S -N --toc -V documentclass=ltjsarticle --latex-engine=lualatex "
;                               "\"" (buffer-name (selected-buffer)) "\"")))

(defun align-table ()
  (interactive)
  (indent-to-token+ "|"))

(defun make-line ()
  (interactive)
  (save-excursion
    (save-restriction
      (narrow-to-region (save-excursion (goto-bol) (point))
                        (save-excursion (goto-eol) (point)))
      (goto-char (point-min))
      (replace-regexp " " "-" t)
      (goto-char (point-min))
      (replace-regexp "|" "+" t)
      (widen))))

(defun make-double-line ()
  (interactive)
  (save-excursion
    (save-restriction
      (narrow-to-region (save-excursion (goto-bol) (point))
                        (save-excursion (goto-eol) (point)))
      (goto-char (point-min))
      (replace-regexp " " "=" t)
      (goto-char (point-min))
      (replace-regexp "|" "+" t)
      (widen))))

; offsideŒn
(defun offside ()
  (save-excursion
    (goto-bol)
    (skip-chars-forward " \t")
    (current-column)))

(defun previous-offside ()
  (if (eq (current-line-number) 1)
      0
    (save-excursion
      (previous-line)
      (offside))))

(defun markdown-indent-line ()
  (interactive "*")
  (smart-indentation (previous-offside)))

(defun markdown-tab-indent-toggle ()
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

(defun markdown-newline-and-indent (&optional (arg 1))
  (interactive "*p")
  (delete-trailing-spaces)
  (insert #\LFD arg)
  (markdown-indent-line))

; mode definition
(defun markdown-mode ()
  (interactive)
  (kill-all-local-variables)
  (setq mode-name "Markdown")
  (setq buffer-mode 'markdown-mode)
  (use-syntax-table *markdown-mode-syntax-table*)
  (use-keymap *markdown-mode-map*)
  (and *markdown-keyword-file* (null *markdown-keyword-hash-table*)
       (setq *markdown-keyword-hash-table*
             (load-keyword-file *markdown-keyword-file*)))
  (when *markdown-keyword-hash-table*
    (make-local-variable 'keyword-hash-table)
    (setq keyword-hash-table *markdown-keyword-hash-table*))
  (when *markdown-regexp-keyword-list*
    (make-local-variable 'regexp-keyword-list)
    (setq regexp-keyword-list *markdown-regexp-keyword-list*))
  (run-hooks '*markdown-mode-hook*))

(add-hook
 '*markdown-mode-hook*
 #'(lambda ()
     (ed::set-buffer-local 'indent-tabs-mode nil)))

;;; markdown-mode.l ends here.
