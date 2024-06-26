;;; -*- Mode: Lisp; Package: EDITOR -*-
;;;
;;; Original file was part of xyzzy-0.2.0.129
;;;
;;; へなちょこ拡張 by HATTORI Masashi<gyo@he.mirai.ne.jp>
;;;
;;; Up   ヒストリーを戻る
;;; Down ヒストリーを進む

;;; 勝手に改変 Fujinaga Nao

(provide "shell")

(in-package "editor")

(export '(*shell-mode-hook* *shell-prompt-regexp* *shell-mode-map* shell-mode
          *shell-ret* *shell-echo* shell shell-send-input shell-send-interrupt shellwrap
          shell-alternate-send-input))

(defvar *shell-mode-hook* nil)
(defvar *shell-prompt-regexp* "^[^#$%>?\n]*[#$%>?] *")
(defvar-local *shell-ret* "\n")
(defvar-local *shell-echo* nil)

(defvar *shell-mode-map* nil)
(unless *shell-mode-map*
  (setq *shell-mode-map* (make-sparse-keymap))
  (define-key *shell-mode-map* #\RET 'shell-send-input)
  (define-key *shell-mode-map* #\C-j 'shell-alternate-send-input)
  (define-key *shell-mode-map* '(#\C-c #\C-c) 'shell-send-interrupt)
  (define-key *shell-mode-map* #\Home 'shell-home)
  (define-key *shell-mode-map* #\S-Home 'shell-selection-beginning-of-line)
  (define-key *shell-mode-map* #\TAB 'shell-dabbrev))

(defun shell-mode ()
  (setq mode-name "Shell")
  (setq buffer-mode 'shell-mode)
  (use-keymap *shell-mode-map*)
  (setq need-not-save t)
  (setq auto-save nil)
  (setq kept-undo-information nil)
  (set-buffer-fold-width 80)
  (cond ((string-matchp *eshell* "command.com$")
         (setq *shell-ret* "\r" *shell-echo* t))
        ((string-matchp *eshell* "cmd.exe$")
         (setq *shell-ret* "\n" *shell-echo* t))
        (t
         (setq *shell-ret* "\n" *shell-echo* nil)))
  (run-hooks '*shell-mode-hook*))

(defun shell ()
  (interactive)
  (set-buffer (get-buffer-create "*Shell*"))
  (let ((proc (buffer-process (selected-buffer))))
    (and proc (eq (process-status proc) ':run)
         (return-from shell t)))
  (goto-char (point-max))
  (shell-mode)
  (make-process *eshell* :output (selected-buffer)))

;;;
;;; This code is loosely based on version 0.1 by Yutaka Oiwa <oiwa@is.s.u-tokyo.ac.jp>.
;;;

(defun shell-send-input ()
  (interactive)
  (let ((process (buffer-process (selected-buffer)))
        start end prompt)
    (when (and process
               (eq (process-status process) :run))
      (cond ((>= (point) (marker-point (process-marker process)))
             (setq start (marker-point (process-marker process)))
             (setq end (progn (goto-eol) (point))))
            ((save-excursion
               (goto-bol)
               (looking-at *shell-prompt-regexp*))
             (setq start (match-end 0))
             (setq end (progn (goto-eol) (point)))
             (setq prompt (match-string 0)))
            (t
             (return-from shell-send-input nil)))
      (let ((cmd (buffer-substring start end)))
        (cond ((eobp)
               (if *shell-echo*
                   (delete-region start end)
                 (insert "\n")))
              (t
               (goto-char (point-max))
               (or (bolp)
                   (insert "\n"))
               (and prompt (insert prompt))
               (unless *shell-echo*
                 (insert cmd "\n"))))
        (set-marker (process-marker process))
;;; add history
        (when (and cmd
                   (< 0 (length cmd)))
          (shell-history-save)
          (shell-history-backward-push cmd))
        (process-send-string process (concatenate 'string cmd *shell-ret*))))))

(defun shell-alternate-send-input ()
  (interactive)
  (let ((*shell-echo* (if (equal *shell-echo* "\n") "\r" "\n")))
    (declare (special *shell-echo*))
    (shell-send-input)))

(setf (symbol-function 'shell-send-interrupt) #'kill-subprocess)



;;;
;;; added by HATTORI Masashi<gyo@he.mirai.ne.jp>
;;;
(define-key *shell-mode-map* #\TAB 'shell-complete)
(define-key *shell-mode-map* #\Up 'shell-history-previous)
(define-key *shell-mode-map* #\C-p 'shell-history-previous)
(define-key *shell-mode-map* #\Down 'shell-history-next)
(define-key *shell-mode-map* #\C-n 'shell-history-next)
(define-key *shell-mode-map* #\C-a 'shell-goto-bol)
(define-key *shell-mode-map* #\C-r 'shell-history-search-backward)

(defvar *shell-prompt-regexp2* "^\\([^#$%>?\n]*\\)[#$%>?]")

(defun shell-complete ()
  (interactive)
  (let ((curp (point))
        dir
        files
        str
        start)
    (save-excursion
      (skip-chars-backward "^ #$%>?");<=いいかげん
      (setq start (point))
      (setq str (buffer-substring start curp))
      (goto-bol)
      (when (looking-at *shell-prompt-regexp2*)
        (let ((match (string-trim " " (match-string 1))))
          ; bashの場合（へなちょこ）
          (if (and (string-matchp "bash" *eshell*)
                   (string-match "^//\\([a-zA-Z]\\)/" match))
              (let ((drive (substring match 2 3)))
                (setq dir (concat drive ":" (substring match 3 (length match)))))
            (setq dir (map-backslash-to-slash match))))
        (when (file-directory-p dir)
          (setq files
                (mapcar #'(lambda (d) (string-right-trim "/" d))
                        (directory dir
                                   :wild (concat str "*")))
                )))
      );save-excursion
    (when files
      (do-completion start (point) :list-ignore-case files))))

(defvar *shell-history-backward* nil)
(defvar *shell-history-current* nil)
(defvar *shell-history-forward* nil)

(defun shell-history-save ()
  (let (command)
    (when *shell-history-current*
      (shell-history-backward-push *shell-history-current*)
      (setq *shell-history-current* nil)
      )
    (while (setq command (shell-history-forward-pop))
      (shell-history-backward-push command)
      )
    ))

(defun shell-history-backward-push (command)
  (push (list command) *shell-history-backward*))

(defun shell-history-backward-pop ()
  (car (pop *shell-history-backward*)))

(defun shell-history-forward-push (command)
  (push (list command) *shell-history-forward*))

(defun shell-history-forward-pop ()
  (car (pop *shell-history-forward*)))

(defun shell-history-previous ()
  (interactive)
  (let ((command (shell-history-backward-pop)))
    (shell-prompt-reset)
    (when *shell-history-current*
      (shell-history-forward-push *shell-history-current*))
    (setq *shell-history-current* command)
    (if command
        (insert command)
      (message "No previous history.")
      )
    ))

(defun shell-history-next ()
  (interactive)
  (let ((command (shell-history-forward-pop)))
    (shell-prompt-reset)
    (when *shell-history-current*
      (shell-history-backward-push *shell-history-current*))
    (setq *shell-history-current* command)
    (if command
        (insert command)
      (message "No next history.")
      )
    ))

(defun shell-history-search-backward ()
  (interactive)
  (let ((curp (point))
        beg)
    (save-excursion
      (shell-goto-bol)
      (setq beg (point))
      (when (= beg curp)
        (return-from shell-history-search-backward))
      )
    (do-completion beg curp :list *shell-history-backward*)
    ))

(defun shell-prompt-reset ()
  (let ((curp (point)))
    (save-excursion
      (shell-goto-bol)
      (when (< (point) curp)
        (delete-region (point) curp))
      )))

(defun shell-goto-bol ()
  (interactive)
  (let ((curp (point)))
    (goto-bol)
    (if (looking-at *shell-prompt-regexp2*)
        (goto-char (match-end 0))
      (goto-char curp)
      )))

(defvar *shell-prompt-regexp* "^[^#$%>?\n]*[#$%>?]*")

(defun shell-home ()
  (interactive)
  (if (save-excursion (goto-bol) (looking-at *shell-prompt-regexp*))
      (goto-char (match-end 0))
    (goto-bol)))

(defun shell-selection-beginning-of-line ()
  (interactive "p")
  (begin-selection)
  (shell-home))
