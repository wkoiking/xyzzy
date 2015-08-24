; xig-mode
; xyzzy interface for git
; author yosugi

(provide "xig/xig")

(defpackage "xig"
  (:use "lisp" "editor"))

(require "xig/common")
(require "xig/diff")
(require "xig/status")
(require "xig/commit")

(defun xig-mode ()
  (interactive)
  (xig::xig-start))

(in-package "xig")

(export '(xig-mode *xig-git-path*))

(defconstant *xig-mode-version* "0.1.0" "�o�[�W����")
(defconstant *xig-mode-name* "xig" "���̃��[�h�̖��O")
(defconstant *xig-pframe-name* *xig-mode-name* "�t���[���̖��O")
(defconstant *xig-main-buffer-name* "*xig-main*" "main �o�b�t�@��")

(defvar *xig-git-path* "git" "git �̃p�X")

(defvar *xig-shortlog-has-graph* nil "shortlog �ɃO���t��\�����Ă��邩")
(defvar *xig-git-shortlog-command* "log --pretty=format:\"%h %ci %an %d %s\"" "shortlog ��\�����鎞�̃R�}���h")
(defvar *xig-git-shortlog-graph-command* (concat *xig-git-shortlog-command* " " "--graph"))

(defvar *xig-main-map* nil)
(unless *xig-main-map*
  (setq *xig-main-map* (copy-keymap *xig-common-map*))
  (define-key *xig-main-map* '#\R 'xig-show-shortlog)
  (define-key *xig-main-map* '#\g 'xig-toggle-shortlog-graph)
  (define-key *xig-main-map* '#\RET 'xig-main-diff)
  (define-key *xig-main-map* '#\S 'xig-status-show)
  (define-key *xig-main-map* '#\q 'xig-quit-mode)
  )

(defun xig-show-version ()
  (interactive)
  (xig-run-git "--version"))

(defun xig-show-shortlog ()
  (interactive)
  (setq *xig-shortlog-has-graph* t)
  (xig-toggle-shortlog-graph))

(defun xig-toggle-shortlog-graph ()
  (interactive)
  (setq buffer-read-only nil)
  (cond
   ((null *xig-shortlog-has-graph*)
    (xig-run-git *xig-git-shortlog-graph-command*)
    (setq *xig-shortlog-has-graph* t))
   (t
    (xig-run-git *xig-git-shortlog-command*)
    (setq *xig-shortlog-has-graph* nil)))
  (save-excursion
    (goto-char (point-max))
    (with-output-to-selected-buffer (format t "~%"))))

(defun xig-main-diff ()
  (interactive)
  (let ((line (xig-get-current-line)) commit)
    (or (string-match "[0-9a-f]+" line)
        (return-from xig-main-diff))
    (setq commit (match-string 0))
    (xig-diff-commit commit)))

(defun xig-start ()
  (interactive)
  (select-pseudo-frame
   (or (find-pseudo-frame *xig-pframe-name*)
       (new-pseudo-frame *xig-pframe-name*)))
  (switch-to-buffer *xig-main-buffer-name*)
  (kill-all-local-variables)
  (setup-temp-buffer (selected-buffer))
  (set-buffer-modified-p nil)
  (setq auto-save nil)
  (setq need-not-save t)
  (setq mode-name "xig-main")
  (setq buffer-mode 'xig-mode)
  (use-keymap *xig-main-map*)
  (xig-show-shortlog)
  (setq buffer-read-only t))

(defun xig-quit-mode ()
  (interactive)
  (when (find-buffer *xig-diff-buffer-name*)
    (xig-diff-quit)
    (return-from xig-quit-mode))
  (delete-pseudo-frame)
  (mapcar #'(lambda (buffer-name)
              (if (find-buffer buffer-name)
                  (delete-buffer (find-buffer buffer-name))))
          (list *xig-main-buffer-name*
                *xig-diff-buffer-name*
                *xig-status-buffer-name*
                *xig-commit-buffer-name*)))