; xig-mode
; xyzzy interface for git
; author yosugi

(provide "xig/status")

(require "xig/common")
(require "xig/commit")
(require "xig/diff")

(in-package "xig")

(defconstant *xig-status-buffer-name* "*xig-status*" "status バッファ名")

(defconstant *xig-status-branch-message* "On branch")
(defconstant *xig-status-staged-message* "Changes to be committed:")
(defconstant *xig-status-unstaged-message* "Changed but not updated:")
(defconstant *xig-status-untracked-message* "Untracked files:")
(defconstant *xig-status-nofiles-message* "(no files)")

(defvar *xig-status-map* nil)
(unless *xig-status-map*
  (setq *xig-status-map* (copy-keymap *xig-common-map*))
  (define-key *xig-status-map* '#\u 'xig-status-toggle-stage-file)
  (define-key *xig-status-map* '#\! 'xig-status-revert-file)
  (define-key *xig-status-map* '#\C 'xig-commit-edit-message)
  (define-key *xig-status-map* '#\q 'xig-status-quit)
  (define-key *xig-status-map* '#\RET 'xig-status-diff)
  )

(defun xig-status-draw ()
  (xig-run-git "status --short --branch")
  (let (line status file branch-name
             (staged-lines nil)
             (unstaged-lines nil)
             (untracked-lines nil))
    (with-input-from-selected-buffer
      ;; ここでブランチ名取得
      (while (null branch-name)
        (setq line (read-line *standard-input* nil))
        (if (string-match "^##" line)
            (setq branch-name (substring line 3))))
      (while (setq line (read-line *standard-input* nil))
        (when (and (> (length line) 3)
                   (string-match "^[ MARCDU?!]\\{2\\} " line))
          (setq status (substring line 0 2))
          (setq file (substring line 3))
          ;; stage されてる場合
          (unless (or (eq (char status 0) #\SPC)
                      (eq (char status 0) #\?))
            (setq staged-lines
                  (concat staged-lines (format nil "~C   ~A~%" (char status 0) file))))
          ;; stage されない場合
          (unless (or (eq (char status 1) #\SPC)
                      (eq (char status 1) #\?))
            (setq unstaged-lines
                  (concat unstaged-lines (format nil "~C   ~A~%" (char status 1) file))))
          ;; untracked な場合
          (when (eq (char status 0) #\?)
            (setq untracked-lines
                  (concat untracked-lines (format nil "?   ~A~%" file)))))))
    (erase-buffer (selected-buffer))
    (with-output-to-selected-buffer
      (format t "~A ~A~%" *xig-status-branch-message* branch-name) ;; ブランチ名
      (format t "~A~%" *xig-status-staged-message*)
      (format t "~A" (or staged-lines (concat "  " *xig-status-nofiles-message* "\n"))) ;; staed
      (format t "~A~%" *xig-status-unstaged-message*)
      (format t "~A" (or unstaged-lines (concat "  " *xig-status-nofiles-message* "\n"))) ;; unstaged
      (format t "~A~%" *xig-status-untracked-message*)
      (format t "~A" (or untracked-lines (concat "  " *xig-status-nofiles-message* "\n")))) ;; untracked
    (goto-char (point-min))))

(defun xig-status-handle-area (func &optional (allow-nofile nil))
  (let ((line (xig-get-current-line))
        file)
    (when (null allow-nofile)
      (and (< (length line) 3) (return-from xig-status-handle-area))
      (and (eq (char line 0) #\SPC) (return-from xig-status-handle-area))
      (or (eq (char line 1) #\SPC) (return-from xig-status-handle-area))
      (setq file (substring line 4)))
    (if (and (>= (length line) 4)
             (string/= (string-trim " " line) *xig-status-nofiles-message*))
        (setq file (substring line 4)))
    (let (staged-line-number unstaged-line-number untracked-line-number)
      (save-excursion
        ;; 必ずブランチ名、staged エリア、unstaged エリア、untracked エリアの順なので
        ;; いちいち (goto-char (point-min)) しなくてよい
        (goto-char (point-min))
        (scan-buffer *xig-status-staged-message*)
        (setq staged-line-number (current-line-number))
        (scan-buffer *xig-status-unstaged-message*)
        (setq unstaged-line-number (current-line-number))
        (scan-buffer *xig-status-untracked-message*)
        (setq untracked-line-number (current-line-number)))
      (funcall func (char line 0) file staged-line-number unstaged-line-number untracked-line-number))))

(defun xig-status-show ()
  (interactive)
  (switch-to-buffer *xig-status-buffer-name*)
  (delete-other-windows)
  (setup-temp-buffer (selected-buffer))
  (kill-all-local-variables)
  (setq buffer-read-only nil)
  (xig-status-draw)
  (setq buffer-read-only t)
  (setq mode-name "xig-status")
  (use-keymap *xig-status-map*))

(defun xig-status-revert-file ()
  (interactive)
  (xig-status-handle-area
   #'(lambda (status file staged-line-number unstaged-line-number untracked-line-number)
       (cond
        ((find (current-line-number) '(staged-line-number unstaged-line-number untracked-line-number))
         (return-from xig-status-toggle-stage-file))
        ((xig-between (current-line-number) staged-line-number unstaged-line-number)
         (msgbox "Cannot revert changes to staged files"))
        ((xig-between (current-line-number) unstaged-line-number untracked-line-number)
         (when (yes-no-or-cancel-p "~A" "Are you sure you want to revert changes?")
           (setq buffer-read-only nil)
           (xig-run-git (format nil "checkout -- ~A" file))
           (xig-status-show)))
        ((>= (current-line-number) untracked-line-number)
         (msgbox "Cannot revert changes to untracked files"))))))

; toggle stage and unstage (or add/rm)
(defun xig-status-toggle-stage-file ()
  (interactive)
  (xig-status-handle-area
   #'(lambda (status file staged-line-number unstaged-line-number untracked-line-number)
       (cond
        ((find (current-line-number) '(staged-line-number unstaged-line-number untracked-line-number))
         (return-from xig-status-toggle-stage-file))
        ((xig-between (current-line-number) staged-line-number unstaged-line-number)
         (setq buffer-read-only nil)
         (xig-call-git (format nil "reset HEAD ~A" file))
         (xig-status-show)
         (scan-buffer file))
        ((>= (current-line-number) unstaged-line-number)
         (setq buffer-read-only nil)
         (if (eq status #\D)
             (xig-call-git (format nil "rm ~A" file))
           (xig-call-git (format nil "add ~A" file)))
         (xig-status-show)
         (scan-buffer file))))))

(defun xig-status-quit ()
  (interactive)
  (if (find-buffer *xig-status-buffer-name*)
      (delete-buffer (find-buffer *xig-status-buffer-name*))))

(defun xig-status-diff ()
  (interactive)
  (xig-status-handle-area
   #'(lambda (status file staged-line-number unstaged-line-number untracked-line-number)
       (cond
        ((eq (current-line-number) staged-line-number)
         (xig-diff-all :cached t))
        ((eq (current-line-number) unstaged-line-number)
         (xig-diff-all))
        ((>= (current-line-number) untracked-line-number)
         (return-from xig-status-diff))
        ((xig-between (current-line-number) staged-line-number unstaged-line-number)
         (if file (xig-diff-file file :cached t)))
        ((xig-between (current-line-number) unstaged-line-number untracked-line-number)
         (if file (xig-diff-file file)))))
   t))
