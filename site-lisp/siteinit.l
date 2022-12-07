; セッションの保存と回復
(setq *marked-session-file* (concat (user-homedir-pathname) "marked-session.ssn"))
(setq *temp-session-file* (concat (user-homedir-pathname) "temp-session.ssn"))
(defun mark-session ()
  (interactive)
  (save-session *marked-session-file* t)
  (message "Session Marked"))
(defun exchange-session ()
  (interactive)
  (save-session *temp-session-file* t)
  (load-session *marked-session-file* nil) ; tにするとghci-modeが何故か読まれないことがあるのでnilにした
  (rename-file *temp-session-file* *marked-session-file* :if-exists :overwrite)
  (message "Session Exchanged"))

(global-set-key '(#\C-x #\SPC) 'mark-session)
(global-set-key '(#\C-x #\x) 'exchange-session)

; Hexl バイナリエディタ
(require "hexl/hexl")

; Ispell
(setq *ispell-command* (concat (user-homedir-pathname) "ispell/ispell.exe"))

; デフォルトファイルエンコーディングをUTF8Nにする
(setq *default-fileio-encoding* *encoding-utf8n*)
(setq *default-eol-code* *eol-lf*)

;; diff
(setq ed::*diff-command-name*
      (map-slash-to-backslash (concat (user-homedir-pathname) "diffutils-2.8.7-1-bin/bin/diff.exe")))

;; C-x gでGit Bashを起動する
; 別途 git for windows をインストールする必要あり
; (setf *git-bash* (map-slash-to-backslash "\"C:/Program Files/Git/bin/sh.exe\" --login -i"))
(setf *git-bash* (map-slash-to-backslash (concat (user-homedir-pathname) "bin/cmder/Cmder.exe")))

(define-key filer-keymap '(#\C-x #\g)
            #'(lambda ()
                (interactive)
                (call-process (or *git-bash*) :no-std-handles t :show :show
                              :exec-directory (filer-get-directory))))

(define-key ctl-x-map #\g 'run-git-bash)
(defun run-git-bash ()
  (interactive)
  (launch-application (or *git-bash*)))

; HaskellでTAG Jumpする
; hasktagsをインストールしてパスを通す必要あり
(load-library "hasktags")
(global-set-key #\F4 'hasktags-jump-tag)
(global-set-key #\S-F4 'hasktags-back-tag-jump)
; (global-set-key #\M-/ 'hasktags-make-tags-file)
; (global-set-key #\M-/ 'hasktags-load-tags-file)

; HTMLブラウザ
(require "win-window")
(require "browserex")
; ファイラ設定
;; ファイラのフォントサイズを設定

(add-hook '*init-app-menus-hook*
          #'(lambda ()
              (set-filer-font :face "MSゴシック" :size 10)))

(setq *filer-use-recycle-bin* t)
(setq *filer-primary-directory* "C:/Users/wanag")
(setq *filer-secondary-directory* "C:/Users/wanag")
(setq *filer-directories*
      `(
        ("Desktop" . ,(get-special-folder-location :desktop))
        ("Home" . "C:/Users/wanag/Home/")
        ("VDU" . "C:/Users/wanag/VDU/")
        ("work" . "C:/Users/wanag/Home/work/")
        ("Code" . "C:/Users/wanag/MyCode/")
        ("Google Drive" . "C:/Users/wanag/Google ドライブ/")
        ("Bin" . "C:/Users/wanag/AppData/Roaming/local/bin/")
        ("Delhi（ローカル）" . "C:/Users/wanag/Home/DelhiMetro8/")
        ("Sao Paulo（ローカル）" . "C:/Users/wanag/Home/SP6/")
        ("Delhi Office Server" . "//192.168.1.10/Fileserver/Common/01_Technical")
        ("Xyzzyhome" . ,(user-homedir-pathname))
        ("Documents" . ,(get-special-folder-location :personal))
        ("System-root" . ,(si:system-root))))

(setq *filer-primary-file-mask* `("*[^~]"))
(setq *filer-secondary-file-mask* `("*[^~]"))

(setq *filer-path-masks*
      `((" " "*[^~]")
        ("All" "*")
        ("bak" "*~" "*.bak")
        ("Haskell" "*.hs" "*.lhs")
        ("TXT" "*.txt")
        ("xls" "*.xls" "*.xlsx")
        ("doc" "*.doc" "*.docx")
        ("PDF" "*.pdf")
        ("PPT" "*.ppt" "*.pptx")
        ("Markdown" "*.md" "*.markdown")
        ("CSV" "*.csv")
        ("SVG" "*.svg")
        ("SAI" "*.sai" "*.sai")
        ("ZIP" "*.zip" "*.7z")
        ("EXE" "*.exe")
        ("email" "*.eml")
        ("Image" "*.bmp" "*.jpg" "*.jpeg" "*.png" "*.tif" "*.gif")
        ("PNG" "*.png")
        ("BMP" "*.bmp")
        ("JPG" "*.jpg" "*.jpeg")
        ("HTML" "*.html" "*.htm")
        ("Lisp" "*.l")
        ("lc" "*.lc")
        ("TIFF" "*.tif")
        ("Tex" "*.tex")
        ("PureScript" "*.purs")
        ("Haskell Dump" "*.dump-hi" "*.hie")
        ("CPP" "*.cpp")
        ("H" "*.h")
        ("DWG" "*.dwg" ".dxf")
        ("GIF" "*.gif")
        ("vsd" "*.vsd" "*.vsdx" "*.vdx")
        ("Link" "*.lnk")
        ("MOV" "*.MOV")))

;; ファイラでファイル名をコピーする
(defvar *get-filenamestring-menu* nil)

(define-key filer-keymap '(#\C-c) #'(lambda () (filer-get-filenames 1 "\n" "" "" "" "")))

(define-key filer-keymap #\C-Insert
            #'(lambda () (interactive)
                (unless *get-filenamestring-menu*
                  (setq *get-filenamestring-menu*
                        (define-popup-menu
                          (:item nil "ファイル名のみコピー(&F)"
                           #'(lambda () (interactive) (filer-get-filenames 1 "\n" "" "" "" "")))
                          (:item nil "フルパス(/)でコピー(&S)"
                           #'(lambda () (interactive) (filer-get-filenames 2 "\n" "" "" "" "")))
                          (:item nil "フルパス(\\)でコピー(&E)"
                           #'(lambda () (interactive) (filer-get-filenames 3 "\n" "" "" "" "")))
                          (:item nil "URL表記でコピー(&U)"
                           #'(lambda () (interactive) (filer-get-filenames 4 "\n" "" "" "" "")))
                          (:item nil "添付形式でコピー(&A)"
                           #'(lambda () (interactive) (filer-get-attach-format)))
                          (:item nil "Haskellリスト表記でコピー(&H)"
                           #'(lambda () (interactive) (filer-get-filenames 1 ", " "[" "]" "\"" "\"")))
                          (:item nil "pandocのimgとしてコピー1(&I)"
                           #'(lambda () (interactive) (filer-get-pandoc-image)))
                          (:item nil "pandocのimgとしてコピー2(&P)"
                           #'(lambda () (interactive) (filer-get-pandoc-image2)))
                          (:item nil "pandocのimgとしてコピー3(&N)"
                           #'(lambda () (interactive) (filer-get-pandoc-image3)))
                          )))
                (track-popup-menu *get-filenamestring-menu*)))

(defun filer-get-filenames (type sep open-bracket close-bracket open-quote close-quote)
  (interactive)
  (let* ((fullpaths (or (filer-get-mark-files)
                             (list (filer-get-current-file))))
         (file-strs (mapcar #'(lambda (filename) (filer-get-filenamestring filename type)) fullpaths)))
    (copy-to-clipboard (filer-make-clipboard-string file-strs sep open-bracket close-bracket open-quote close-quote))
    (message "ファイル名をコピーしました")))

(defun filer-get-filenamestring (fullpath type)
  (interactive)
  (let (str)
    (cond
     ((= type 1) (setq str (file-namestring fullpath)))
     ((= type 2) (setq str fullpath))
     ((= type 3) (setq str (map-slash-to-backslash fullpath)))
     ((= type 4) (setq str (concat "file:///" fullpath))))
    str))

(defun filer-make-clipboard-string (lst sep open-bracket close-bracket open-quote close-quote)
  (let* ((lst2 (mapcar #'(lambda (x) (concat open-quote x close-quote)) lst))
         (x (car lst2))
         (xs (cdr lst2)))
    (concat open-bracket (apply #'concat (cons x (mapcar #'(lambda (x) (concat sep x)) xs))) close-bracket)))

(defun string-left-match-number (str1 str2)
  (if (or (= 0 (length str1)) (= 0 (length str2)))
      0
    (if (char-equal (elt str1 0) (elt str2 0))
        (+ 1 (string-left-match-number (subseq str1 1) (subseq str2 1)))
      0)))

(defun filer-get-pandoc-image ()
  (interactive)
  (let* ((fullpaths (or (filer-get-mark-files)
                            (list (filer-get-current-file))))
         (filenames (mapcar #'(lambda (filename) (pathname-name filename)) fullpaths))
         (working-dir (get-buffer-file-name))
         (image-strs (mapcar #'(lambda (filename fullpath)
                                 (concat "![" (substitute-string filename "_" " ") "](./" (subseq fullpath (string-left-match-number working-dir fullpath)) "){#fig:"
                                         (string-downcase (substitute-string (substitute-string filename "_" "-") " " "-")) "}"))
                                 filenames fullpaths)))
    (copy-to-clipboard (filer-make-clipboard-string image-strs
                        "\n" "" "" "" ""))
    (message "ファイル名をコピーしました")))

(defun filer-get-pandoc-image2 ()
  (interactive)
  (let* ((fullpaths (or (filer-get-mark-files)
                            (list (filer-get-current-file))))
         (filenames (mapcar #'(lambda (filename) (filer-get-filenamestring filename 1)) fullpaths))
         (working-dir (get-buffer-file-name))
         (image-strs (mapcar #'(lambda (filename fullpath)
                                 (concat "[" (pathname-name filename) "]: ./"
                                         (subseq fullpath (string-left-match-number working-dir fullpath))))
                             filenames fullpaths)))
    (copy-to-clipboard (filer-make-clipboard-string image-strs
                        "\n" "" "" "" ""))
    (message "ファイル名をコピーしました")))

(defun filer-get-pandoc-image3 ()
  (interactive)
  (let* ((fullpaths (or (filer-get-mark-files)
                            (list (filer-get-current-file))))
         (filenames (mapcar #'(lambda (filename) (pathname-name filename)) fullpaths))
         (working-dir (get-buffer-file-name))
         (image-strs (mapcar #'(lambda (filename fullpath)
                                 (concat "![](./" (subseq fullpath (string-left-match-number working-dir fullpath)) ")"))
                                 filenames fullpaths)))
    (copy-to-clipboard (filer-make-clipboard-string image-strs
                        "\n" "" "" "" ""))
    (message "ファイル名をコピーしました")))

(defun filer-get-attach-format ()
  (interactive)
  (let* ((selected-files (or (filer-get-mark-files)
                            (list (filer-get-current-file))))
         (dirname (map-slash-to-backslash (filer-get-directory)))
         (filenames (mapcar #'(lambda (filename) (concat "    " (filer-get-filenamestring filename 1))) selected-files)))
    (copy-to-clipboard (concat dirname "\nファイル名:\n" (filer-make-clipboard-string filenames "\n" "" "" "" "")))
    (message "ファイル名をコピーしました")))

; filer Graphics Magick
(defvar *graphics-magick-convert-menu* nil)

(define-key filer-keymap #\C-End
            #'(lambda () (interactive)
                (unless *graphics-magick-convert-menu*
                  (setq *graphics-magick-convert-menu*
                        (define-popup-menu
                          (:item nil "印刷用(L)(&L)"
                           #'(lambda () (interactive) (graphics-magick-convert-files 1)))
                          (:item nil "印刷用(M)(&M)"
                           #'(lambda () (interactive) (graphics-magick-convert-files 2)))
                          (:item nil "印刷用(S)(&S)"
                           #'(lambda () (interactive) (graphics-magick-convert-files 3)))
                          (:item nil "HTML用(L)(&B)"
                           #'(lambda () (interactive) (graphics-magick-convert-files 4)))
                          (:item nil "HTML用(M)(&N)"
                           #'(lambda () (interactive) (graphics-magick-convert-files 5)))
                          (:item nil "HTML用(S)(&T)"
                           #'(lambda () (interactive) (graphics-magick-convert-files 6)))
                          (:item nil "詳細表示(D)(&D)"
                           #'(lambda () (interactive) (graphics-magick-convert-files 7)))
                          )))
                (track-popup-menu *graphics-magick-convert-menu*)))

(defun graphics-magick-convert-files (type)
  (interactive)
  (message "graphics-magick-convert-files")
  (let ((selected-files (or (filer-get-mark-files)
                            (list (filer-get-current-file)))))
    (dolist (file selected-files) (graphics-magick-convert (filer-get-filenamestring file 1) type))
    (message "イメージファイルをconvertしました。")
    ))

(defun graphics-magick-convert (filename type)
  (interactive)
  (cond
   ((= type 1) (resize-with-centimeter 16 filename))
   ((= type 2) (resize-with-centimeter 10 filename))
   ((= type 3) (resize-with-centimeter 3 filename))
   ((= type 4) (pixel-resize 20 filename))
   ((= type 5) (pixel-resize 12 filename))
   ((= type 6) (pixel-resize 4 filename))
   ((= type 7) (identify filename))))

(defun get-image-size (filename)
  (interactive)
  (unwind-protect
      (save-excursion
        (setq buffer (get-buffer-create "TempImgSize"))
        (set-buffer buffer)
        (execute-shell-command (concat "gm identify -verbose " filename) nil "TempImgSize" nil (filer-get-directory))
        (goto-char (point-min))
        (scan-buffer "Geometry: \\([1-9][0-9]*\\)x\\([1-9][0-9]*\\)$" :tail t :regexp t)
        (values (parse-integer (match-string 1)) (parse-integer (match-string 2))))
    (and buffer
         (delete-buffer buffer)))
    )

(defun resize-with-centimeter (centimeter filename)
  (interactive)
  (multiple-value-bind (w h) (get-image-size filename)
    (let* ((w2 (if (> h w) (round (* (sqrt 2) h)) w)))
      (when (> h w)
        (let* ((size (concat (write-to-string w2) "x" (write-to-string h))))
          (call-process (concat "gm convert -size " size " -resize " size " -compose over -gravity Center -background white -extent " size " " filename " " filename))))
      (let*
          ((ppc (write-to-string (round (/ w2 centimeter))))
           (density (concat ppc "x" ppc)))
        (call-process (concat "gm convert -density " density " -units pixelspercentimeter " filename " " (pathname-name filename) ".png")
                      :exec-directory (filer-get-directory)
                      :wait t)
        ))))


(defun pixel-resize (centimeter filename)
  (interactive)
  (multiple-value-bind (w h) (get-image-size filename)
    (let* ((w2 (if (> h w) (round (* (sqrt 2) h)) w)))
      (when (> h w)
        (let* ((size (concat (write-to-string w2) "x" (write-to-string h))))
          (call-process (concat "gm convert -size " size " -resize " size " -compose over -gravity Center -background white -extent " size " " filename " " filename))))
      (let*
          ((new-size (concat (write-to-string (* 40 centimeter)) "x" (write-to-string (round (/ (* 40 centimeter h) w2))))))
        (call-process (concat "gm convert -size " new-size " " filename " -resize " new-size " +profile \"*\" " (pathname-name filename) ".png")
                      :exec-directory (filer-get-directory)
                      :wait t)))))

(defun identify (filename)
  (interactive)
  (save-excursion
    (let ((bufname (concat filename "_Info")))
      (setq buffer (get-buffer-create bufname))
      (switch-to-buffer buffer)
      (execute-shell-command (concat "gm identify -verbose " filename) nil bufname nil (filer-get-directory))
      (goto-char (point-min)))))

; rename
(define-key filer-keymap #\F2 'filer-rename)

;; ファイラの開いているディレクトリーでコンソールを起動
(define-key filer-keymap '(#\C-x #\c)
            #'(lambda ()
                (interactive)
                (call-process (or *eshell* *shell*) :no-std-handles t :show :show
                              :exec-directory (filer-get-directory))))


;; ファイラRET
(defvar *file-asoc*
  '(
;     ("*.svg" . "C:/Program Files/Google/Chrome/Application/chrome.exe")
    ("*.pdf" . "C:/Program Files/Tracker Software/PDF Viewer/PDFXCview.exe")
    ))

(define-key filer-keymap #\RET 'my-filer-ok)

(defun my-filer-ok ()
  (let ((file (filer-get-text)))
    (cond (file
           (filer-set-text "")
           (setq file (merge-pathnames file (filer-get-directory)))
           (when ed::*filer-follow-links*
             (setq file (ed::filer-read-link file)))
           (cond ((wild-pathname-p file)
                  (filer-focus)
                  (filer-set-directory (directory-namestring file))
                  (filer-reload (file-namestring file)))
                 ((file-directory-p file)
                  (filer-focus)
                  (filer-chdir file))
                 (t
                  (check-valid-pathname file)
                  (my-filer-readin file) ;;
                 )))
          (t
           (let ((files (filer-get-mark-files t)))
             (cond (files
                    (when ed::*filer-follow-links*
                      (setq files (mapcar #'ed::filer-read-link files)))
                      (my-filer-readin files) ;;
                   )
                   ((and (setq files (filer-get-mark-files nil))
                         (null (cdr files)))
                    (filer-chdir (car files)))
                   ((filer-current-file-dot-dot-p)
                    (filer-up-directory))
                   ((setq files (filer-get-current-file))
                    (when ed::*filer-follow-links*
                      (setq files (ed::filer-read-link files)))
                    (if (file-directory-p files)
                        (filer-chdir files)
                        (my-filer-readin files))) ;;#
                   (t nil)))))))

(defun my-filer-readin (file)
  (my-filer-readin-1 file)
  (if (filer-modal-p)
      (filer-cancel)
    (si:*activate-toplevel)))

(defun my-filer-readin-1 (file)
  (if (listp file)
      (mapc #'my-filer-readin-1 file)
    (unless (file-directory-p file)
      (filer-asoc file *file-asoc*)
      (add-history file '*minibuffer-file-name-history*)))
  t)

(defun filer-asoc (file asoc)
  (cond ((null asoc)
         (find-file file))
        ((pathname-match-p file (caar asoc))
         (launch-application (concat "\"" (cdar asoc)  "\" \"" (map-slash-to-backslash file) "\"")))
        (t
         (filer-asoc file (cdr asoc)))))

; ファイラX
(defun my-filer-shell-execute ()
  (let ((file (filer-get-current-file)))
    (when (or (null ed::*filer-query-before-process*)
              (yes-or-no-p "~A~%を実行しますか?" file))
      (if (file-directory-p file)
          (launch-application (concat "explorer /select," "\"" (map-slash-to-backslash (filer-get-current-file)) "\""))
        (progn
;           (filer-forward-line 1)
          (shell-execute file (filer-get-directory)))))))

(define-key filer-keymap #\X 'my-filer-shell-execute)

; filer-grep
(define-key filer-keymap #\C-f 'filer-grep)
(define-key filer-keymap #\/ 'filer-casual-search)
(undefine-key filer-keymap #\\)

; メーラー
(require "x-key-macro")

(autoload 'kamail3 "kamail3/defs" t)
(autoload 'kamail3-toggle "kamail3/defs" t)

(autoload 'kamail3-mailto "kamail3/defs" nil)
(setq *command-line-mailto-hook* #'kamail3-mailto)

(global-set-key #\C-k 'kamail3-toggle)

; ミニバッファとウィンドウ
(set-extended-key-translate-table exkey-escape #\F13)
(define-key minibuffer-local-map #\F13 'quit-recursive-edit)
(define-key minibuffer-local-must-match-map #\F13 'quit-recursive-edit)
(define-key minibuffer-local-completion-map #\F13 'quit-recursive-edit)
(define-key minibuffer-local-command-line-map #\F13 'quit-recursive-edit)

(define-key minibuffer-local-map #\Home 'beginning-of-line)
(define-key minibuffer-local-must-match-map #\Home 'beginning-of-line)
(define-key minibuffer-local-completion-map #\Home 'beginning-of-line)
(define-key minibuffer-local-command-line-map #\Home 'beginning-of-line)

(global-set-key #\F13 'esc-delete-window)
(defun esc-delete-window ()
  (interactive)
  (if (/= (count-windows) 1)
      (delete-window)))

; Temp バッファ
(defvar *temp-buffer-name* "*Temp*")

(defun temp-get-buffer () ; バッファがなければ作成、あれば返す
  (setq *temp-buffer* (get-buffer-create *temp-buffer-name*)))

(defun temp-load-file (&optional f)
  (interactive)
  (let ((buf (temp-get-buffer)) ; 出力バッファ
        )
    (split-window-vertically)
    (switch-to-buffer-other-window buf)
    ))

(global-set-key #\C-t 'temp-load-file)

; Shell3
(require "shell3")
(global-set-key '(#\C-x #\s) 'shell)

; ファイラの開いているディレクトリでShell起動
(defun shell-from-filer ()
  (interactive)
  (set-buffer (get-buffer-create "*Shell*"))
  (let ((proc (buffer-process (selected-buffer))))
    (and proc (eq (process-status proc) ':run)
         (return-from shell t)))
  (goto-char (point-max))
  (shell-mode)
  (make-process *eshell* :output (selected-buffer) :exec-directory (filer-get-directory))
  (filer-cancel)
  )

(define-key filer-keymap '(#\C-x #\s) 'shell-from-filer)

; Align
(require "indent-to-token+")

; バッファタブの操作 separate-buffer-tab
(require "buffer-tab-operation")

; Windows 仕様
(require "gates_kai")

;; タブトグルの順番をきれいに
(setq *next-buffer-in-tab-order* t)

;; .xyzzy を lisp-mode で編集する
(pushnew '("\\.xyzzy$" . lisp-mode) *auto-mode-alist*)

; Markdownモード
(require "markdown-mode")
(push '("\\.md$" . (lambda ()
                     (markdown-mode)
                     )) *auto-mode-alist*)
(push '("\\.markdown$" . (lambda ()
                     (markdown-mode)
                     )) *auto-mode-alist*)

; Railwayモード
(require "railway-mode")
(push '("\\.rail$" . (lambda ()
                       (railway-mode)
                       )) *auto-mode-alist*)

; Haskell-mode
(require "haskell-mode")
(require "ghci")
(push '("\\.hs$" . (lambda ()
                     (haskell-mode)
                     (ghci-mode)
                     )) *auto-mode-alist*)
(push '("\\.lhs$" . (lambda ()
                      (haskell-mode)
                      (ghci-mode)
                      )) *auto-mode-alist*)
(push '("\\.elm$" . (lambda ()
                      (haskell-mode)
                      )) *auto-mode-alist*)
(push '("\\.purs$" . (lambda ()
                      (haskell-mode)
                      )) *auto-mode-alist*)
(push '("\\.dhall$" . (lambda ()
                      (haskell-mode)
                      )) *auto-mode-alist*)
(global-set-key #\M-/ 'hasktags-load-tags-file)

; セッション
(add-hook '*before-find-file-hook*
          #'(lambda (x)
              (when (string-matchp "\\.ssn$" x)
                (load-session x)
                (selected-buffer))))

; lispモードのインデントをスペースで
(require "lispmode")
(setq *lisp-tab-always-indent* nil)
(setq *lisp-indent-tabs-mode* nil)
(setq ed::lisp-indent-level 4)
(setq ed::lisp-brace-offset -4)
(setq ed::lisp-continued-statement-offset 4)
(setq ed::lisp-brace-imaginary-offset 0)
(setq ed::lisp-label-offset -2)
(setq ed::lisp-comment-indent 3)
(setq ed::lisp-argdecl-indent 3)

; Paren
(require "paren")
(turn-on-global-paren)

; セレクションをコメントアウトする
(require "comment")
(defun comment-out-selection ()
  (interactive)
  (comment-out-region (selection-point) (selection-mark) 1))

(defun uncomment-selection ()
  (interactive)
  (comment-out-region (selection-point) (selection-mark) -10 ))
(set-extended-key-translate-table exkey-S-tab #\F20) ; Shift-Tabでコメントアウト
(define-key selection-keymap #\F20 'comment-out-selection)

(set-extended-key-translate-table exkey-S-backspace #\F21) ; Shift-BackSpaceでアンコメント
(define-key selection-keymap #\F21 'uncomment-selection)

;; セレクションインデント
(defun indent-selection ()
  (interactive)
  (if (get-selection-type)
      (indent-region (- (selection-mark) 1) (selection-point))
    (save-excursion
      (and (yes-or-no-p "バッファ全体を一発インデントしますよ。")
           (indent-region (point-min) (point-max))))))
(define-key selection-keymap #\TAB 'indent-selection)


;; ghci（電卓用）
(defun ghci-get-buffer ()
  (setq *ghci-buffer* (get-buffer-create *ghci-buffer-name*)))

(defvar *ghci-command* "stack exec ghci")
(defvar *ghci-buffer-name* "*GHCi*")
(defvar *ghci-buffer* nil)

(define-key ctl-x-map #\C-s
            #'(lambda ()
                (interactive)
                (let ((buf (ghci-get-buffer)) ; 出力バッファ
                      proc ; プロセス
                      )
                  (switch-to-buffer buf)
                  (setq proc (buffer-process buf))
                  (if (and proc (eq (process-status proc) ':run))
                      proc
                    (progn
                      (erase-buffer buf)
                      (setq proc (make-process *ghci-command*
                                               :output buf))))
                  (ghci-shell-mode)
                  (end-of-buffer))))

; (defun ghci-shell ()
;   (interactive)
;   (let ((buf (ghci-get-buffer)) ; 出力バッファ
;         proc ; プロセス
;         )
;     (switch-to-buffer buf)
;     (setq proc (buffer-process buf))
;     (if (and proc (eq (process-status proc) ':run))
;         proc
;       (progn
;         (erase-buffer buf)
;         (setq proc (make-process *ghci-command*
;                                  :output buf))))
;     (ghci-shell-mode)
;     (end-of-buffer)))
;
; (define-key ctl-x-map #\C-s 'ghci-shell)

(define-key filer-keymap '(#\C-x #\C-s)
            #'(lambda ()
                (interactive)
                (let ((buf (ghci-get-buffer)) ; 出力バッファ
                      proc ; プロセス
                      )
                  (switch-to-buffer buf)
                  (setq proc (buffer-process buf))
                  (if (and proc (eq (process-status proc) ':run))
                      proc
                    (progn
                      (erase-buffer buf)
                      (setq proc (make-process *ghci-command*
                                               :output buf
                                               :exec-directory (filer-get-directory)))))
                  (ghci-shell-mode)
                  (end-of-buffer))))

;; マウスホイールでスクロールさせてもセレクションを・・・
(require "keep-selection")

(defun previous-page-kept-selection ()
   (interactive)
   (move-kept-selection #'previous-page))

(global-set-key #\PageUp 'previous-page-kept-selection)
(global-set-key #\PageDown 'next-page-kept-selection)

;;; マウス右ボタン押しながらホイール操作でバッファ切り替え
(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "foreign"))

(defpackage :buffer-bar-scroll
  (:use :lisp :editor))

(in-package :buffer-bar-scroll)

(c:define-dll-entry c:short get-key-state (c:int) "user32" "GetKeyState")

(defvar *after-buffer-bar-scroll* nil)

(defun mouse-rbutton-up ()
  (interactive)
  (if *after-buffer-bar-scroll*
      (setq *after-buffer-bar-scroll* nil)
    (mouse-menu-popup)))

(defun scroll-buffer-bar-or-window (window n lines)
  (if (< (get-key-state #x02) 0)
      (let ((*next-buffer-in-tab-order* t))
        (setq *after-buffer-bar-scroll* t)
        (if (> n 0) (next-buffer) (previous-buffer)))
    (with-selected-window
      (set-window window)
      (move-kept-selection #'scroll-window (* n (or lines (window-lines window)))))))

(global-set-key #\RBtnUp 'mouse-rbutton-up)
(setq mouse-wheel-handler #'scroll-buffer-bar-or-window)

;; 中ボタンでバッファを削除
(require "buffer-bar-click-to-delete")
(buffer-bar-click-to-delete:set-buffer-bar-activate-flag 1)
(buffer-bar-click-to-delete:set-buffer-bar-lbutton-flag 1)

;; ファイラでアーカイブするときのデフォルトのファイル名を圧縮元のファイル名に
(defun my-filer-get-archive-name ()
  (multiple-value-bind (result data)
      (dialog-box '(dialog 0 0 251 95
                    (:caption "ファイルの圧縮")
                    (:font 9 "MS UI Gothic")
                    (:control
                     (:static nil "ファイル名(&N):" #x50020000 7 64 38 8)
                     (:edit name nil #x50810080 7 74 160 14)
                     (:button IDOK "OK" #x50010001 194 7 50 14)
                     (:button IDCANCEL "キャンセル" #x50010000 194 24 50 14)
                     (:static nil "作成するディレクトリ(&D):" #x50020000 8 7 64 8)
                     (:combobox dir nil #x50210102 8 18 160 85)
                     (:button ref "参照..." #x50010000 169 17 22 14)
                     (:static nil "基準ディレクトリ(&B):" #x50020000 7 35 53 8)
                     (:edit base nil #x50810080 7 45 160 14)
                     (:button baseref "参照..." #x50010000 169 45 22 14)))
                  (list (cons 'name (format nil "~A.zip" (pathname-name (first (filer-get-mark-files)))))
                        (cons 'dir (filer-get-directory))
                        (cons 'dir ed::*filer-archive-directory-history*)
                        (cons 'base (filer-get-directory)))
                  '((name :non-null "アーカイブファイル名を入力してください" :enable (IDOK))
                    (ref :related dir :directory-name-dialog (:title "参照"))
                    (baseref :related base :directory-name-dialog (:title "参照"))))
    (when result
      (let* ((dir (cdr (assoc 'dir data)))
             (file (merge-pathnames (cdr (assoc 'name data)) dir)))
        (add-history dir 'ed::*filer-archive-directory-history*)
        (values (cond ((string-matchp "\\.zip$" file) file)
                      ((string-matchp "\\.lzh$" file) file)
                      ((string-matchp "\\.cab$" file) file)
                      ((string-matchp "\\.tar$" file) file)
                      ((string-matchp "\\.tar\\.gz$" file) file)
                      ((string-matchp "\\.tar\\.bz2$" file) file)
                      ((string-matchp "\\.tar\\.Z$" file) file)
                      ((string-matchp "\\.tbz$" file) file)
                      ((string-matchp "\\.tgz$" file) file)
                      ((string-matchp "\\.taz$" file) file)
                      ((string-matchp "\\.bza$" file) file)
                      ((string-matchp "\\.gza$" file) file)
                      ((string-matchp "\\.yz1$" file) file)
                      ((string-matchp "\\.7z$" file) file)
                      ((string-matchp "\\.exe$" file) file)
                      (t (concat file ".zip")))
                (merge-pathnames (cdr (assoc 'base data))
                                 (filer-get-directory)))))))

(defun my-filer-create-archive ()
  (let ((marks (filer-get-mark-files)))
    (when marks
      (multiple-value-bind (arc base)
          (my-filer-get-archive-name)
        (when arc
          (filer-subscribe-to-reload (directory-namestring arc))
          (let ((sfx (string-matchp "\\.exe$" arc)))
            (when sfx
              (setf (subseq arc (- (length arc) 3)) "zip"))
            (create-archive arc
                            (if (string-matchp "\\.cab$" arc)
                                (mapcar #'(lambda (f)
                                            (if (file-directory-p f)
                                                (merge-pathnames "*.*" f)
                                              f))
                                        marks)
                              marks)
                            base)
            (when sfx
              (convert-to-SFX arc "-gw"))
            (filer-clear-all-marks)))))))

(define-key filer-keymap #\O 'my-filer-create-archive)
