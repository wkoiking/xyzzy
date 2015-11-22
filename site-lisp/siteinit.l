;; xig

(load-library "xig/xig")
(setq xig::*xig-git-path* (map-slash-to-backslash "C:/Program Files/Git/cmd/git.exe"))

;; diff
(setq ed::*diff-command-name*
      (map-slash-to-backslash "C:/Users/5630/xyzzy/diffutils-2.8.7-1-bin/bin/diff.exe"))

;; C-x gでGit Bashを起動する
; git for windows をインストールする必要あり
; (setf *git-bash* "\"C:\\\\Program Files\\\\Git\\\\bin\\\\sh.exe\" --login -i")
; (setf *git-bash* (map-slash-to-backslash "\"C:/Program Files/Git/bin/sh.exe\" --login -i"))
(setf *git-bash* (map-slash-to-backslash "C:/Users/5630/xyzzy/bin/cmder/Cmder.exe"))

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
(global-set-key #\M-/ 'hasktags-make-tags-file)

; HTMLブラウザ
(require "win-window")
(require "browserex")
; ファイラ設定
;; ファイラのフォントサイズを設定

(add-hook '*init-app-menus-hook*
          #'(lambda ()
              (set-filer-font :face "MSゴシック" :size 10)))

(setq *filer-use-recycle-bin* t)
(setq *filer-primary-directory* "C:/Users/Nao/Dropbox/")
(setq *filer-secondary-directory* "C:/Users/Nao/Dropbox/")
(setq *filer-directories*
      `(
        ("User" . "C:/Users/Nao/Dropbox/")
        ("Desktop" . ,(get-special-folder-location :desktop))
        ("Haskell" . "C:/Users/Nao/Dropbox/Haskell/")
        ("blog" . "C:/Users/Nao/Dropbox/Public/blog/")
        ("work" . "C:/Users/Nao/Dropbox/work/")
        ("aアプリケーション" . "C:/Users/5630/Desktop/使用しないアプリケーション/")
        ("Delhi Metro 8（ローカル）" . "C:/Users/5630/DelhiMetro8/hascats/")
        ("Delhi Metro 8 (kk)" . "//KPFSV/kk/k04/03件名業務/02_海外/07_インド関連/デリーメトロ/11_ATS設計/hascats/")
        ("Delhi Metro 8（ns-pub）" .  "//KPFSV/ns-pub/p04/国際システム設計部_河合_デリーATS資料（保管期限：2020年3月31日）品証共有/delhi-line-8-ats-2015/")
        ("Delhi Metro 8 (comment from DMRC)" .  "//KPFSV/kk/k04/03件名業務/02_海外/07_インド関連/デリーメトロ/06_Final Designe/100_Comment from DMRC/")
        ("Delhi Office Server" . "//192.168.1.10/Fileserver/Common/01_Technical")
        ("k勤怠関係" . "C:/Users/Nao/Dropbox/work/k勤怠関係/")
        ("d台帳" . "C:/Users/Nao/Dropbox/work/d台帳/")
        ("g外注依頼" . "C:/Users/Nao/Dropbox/work/g外注依頼/")
        ("b部会資料" . "//KPFSV/kk/k04/01支援業務/04_ワークフロー/58_畑崎直紀/01B．部会資料/")
        ("Sao Paulo 6（ローカル1）" . "C:/Users/5630/SP6/hascats/")
        ("Sao Paulo 6" . "//KPFSV/kk/k04/03件名業務/02_海外/11_ブラジル/サンパウロメトロ6号/011_ATS_(藤永)/")
        ("Sao Paulo 6（国際送付）" . "//KPFSV/ns-pub/p04/海外SEG⇔国際_サンパウロ6号_国際藤永管理_期限2015年8月1日/")
        ("Sao Paulo 6（進捗報告）" . "//KPFSV/kk/k04/03件名業務/02_海外/11_ブラジル/サンパウロメトロ6号/003C_Executive Design/002B_進捗報告/")
        ("ssh" . "C:/Users/5630/.ssh/")
        ("sスタートアップ" . "C:/Users/5630/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/")
        ("JMRT（ローカル）" . "C:/Users/Nao/Dropbox/work/JMRT/")
        ("JMRT" . "//KPFSV/ns-pub/p04/ジャカルタMRT作業フォルダ_期限2018年12月31日/")
        ("JMRT国際送付" . "//KPFSV/ns-pub/p04/海外SEG⇔国際_ジャカルタMRT_国際近内管理_期限2014年8月1日/")
        ("pプロジェクト案件進捗管理" . "//KPFSV/kk/k04/01支援業務/04_ワークフロー/44_石川達哉/【国際システム設計部】/プロジェクト案件進捗会議/")
        ("s接地基準協同研究委員会（ローカル）" . "C:/Users/Nao/Dropbox/work/s接地基準協同研究委員会/")
        ("s接地基準協同研究委員会" . "//KPFSV/kk/k04/01支援業務/04_ワークフロー/45_内海博文(インドPJ)/01_個人フォルダ/5121_高橋昌秀/電気学会（接地）/")
        ("CTCセンタ（ローカル）" . "C:/Users/Nao/Dropbox/work/131期/t台湾在来CTC/")
        ("CTCセンタ" . "//KPFSV/kk/k04/03件名業務/02_海外/01_台湾関連/01_台湾在来/CTCセンタ/")
        ("scanner" . "C:/scanner_fuji/")
        ("WQUP（ローカル）" . "C:/Users/Nao/Dropbox/work/WQUP/")
        ("WQUP" . "//KPFSV/kk/k04/03件名業務/02_海外/00_海外共通/")
        ("ISO文章" . "//KPFSV/kk/k04/00共通業務/01_ISO文書/7600（技術共通）/")
        ("KBA進捗" . "//KPFSV/ns-pub/p04/2020年期限/国際事業部KBA_期限200331/")
        ("ns-rule" . "//KPFSV/ns-rule/r01/")
        ("ns-pub/04" . "//KPFSV/ns-pub/p04/")
        ("nsec" . "//157.64.37.166/nsec/10．データ交換/")
        ("kpcad" . "//Kpcad-srv/技術共通/")
        ("kpcad02部品標準化委員会" . "//kpcad-srv02/技術共通/新規品目登録（主管：部品標準化分科会）/新規品目登録受付/")
        ("a照査（畑崎）" . "//KPFSV/kk/k04/01支援業務/04_ワークフロー/58_畑崎直紀/00．照査承認依頼/藤永/")
        ("a照査（内海）" . "//KPFSV/kk/k04/01支援業務/04_ワークフロー/45_内海博文(インドPJ)/10_照査・承認/")
        ("a承認（石川）" . "//KPFSV/kk/k04/01支援業務/04_ワークフロー/44_石川達哉/01 押印待ち/")
        ("o海外" . "//KPFSV/kk/k04/03件名業務/02_海外/")
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
        ("CPP" "*.cpp")
        ("H" "*.h")
        ("Markdown" "*.md" "*.markdown")
        ("TXT" "*.txt")
        ("SVG" "*.svg")
        ("SAI" "*.sai" "*.sai")
        ("CSV" "*.csv")
        ("HTML" "*.html" "*.htm")
        ("PNG" "*.png")
        ("BMP" "*.bmp")
        ("DWG" "*.dwg" ".dxf")
        ("JPG" "*.jpg")
        ("Image" "*.bmp" "*.jpg" "*.png" "*.tif" "*.gif")
        ("Lisp" "*.l")
        ("lc" "*.lc")
        ("TIFF" "*.tif")
        ("Tex" "*.tex")
        ("PDF" "*.pdf")
        ("xls" "*.xls" "*.xlsx")
        ("doc" "*.doc" "*.docx")
        ("GIF" "*.gif")
        ("EXE" "*.exe")
        ("vsd" "*.vsd")
        ("ZIP" "*.zip")
        ("MOV" "*.MOV")
        ("Railway" "*.rail")))

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
                          (:item nil "pandocのimgとしてコピー(&P)"
                           #'(lambda () (interactive) (filer-get-pandoc-image)))
                          (:item nil "pandocのimgとしてコピー2(&I)"
                           #'(lambda () (interactive) (filer-get-pandoc-image2)))
                          )))
                (track-popup-menu *get-filenamestring-menu*)))

(defun filer-get-filenames (type sep open-bracket close-bracket open-quote close-quote)
  (interactive)
  (let* ((selected-files (or (filer-get-mark-files)
                             (list (filer-get-current-file))))
         (file-strs (mapcar #'(lambda (filename) (filer-get-filenamestring filename type)) selected-files)))
    (copy-to-clipboard (filer-make-clipboard-string file-strs sep open-bracket close-bracket open-quote close-quote))
    (message "ファイル名をコピーしました")))

(defun filer-get-filenamestring (filename type)
  (interactive)
  (let (str)
    (cond
     ((= type 1) (setq str (if (file-directory-p filename)
                               (car (reverse (pathname-directory (concat filename "x"))))
                             (file-namestring filename))))
     ((= type 2) (setq str filename))
     ((= type 3) (setq str (map-slash-to-backslash filename)))
     ((= type 4) (setq str (concat "file:///" filename))))
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

(defun string-trim-extension (str)
  (reverse (string-trim-extension-sub (reverse str))))

(defun string-trim-extension-sub (str)
  (if (= 0 (length str))
      ""
    (if (char-equal (elt str 0) #\.)
        (subseq str 1)
      (string-trim-extension-sub (subseq str 1)))))

(defun filer-get-pandoc-image ()
  (interactive)
  (let* ((selected-files (or (filer-get-mark-files)
                            (list (filer-get-current-file))))
         (filenames (mapcar #'(lambda (filename) (filer-get-filenamestring filename 1)) selected-files))
         (fullpaths (mapcar #'(lambda (filename) (filer-get-filenamestring filename 2)) selected-files))
;          (working-dir (default-directory))
         (working-dir (get-buffer-file-name))
         (image-strs (mapcar #'(lambda (filename fullpath)
                                 (concat "[" (string-trim-extension filename) "]: ./"
                                                                   (subseq fullpath (string-left-match-number working-dir fullpath))))
                             filenames fullpaths)))
    (copy-to-clipboard (filer-make-clipboard-string image-strs
                        "\n" "" "" "" ""))
    (message "ファイル名をコピーしました")))

(defun filer-get-pandoc-image2 ()
  (interactive)
  (let* ((selected-files (or (filer-get-mark-files)
                            (list (filer-get-current-file))))
         (filenames (mapcar #'(lambda (filename) (filer-get-filenamestring filename 1)) selected-files))
         (fullpaths (mapcar #'(lambda (filename) (filer-get-filenamestring filename 2)) selected-files))
;          (working-dir (default-directory))
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
        (call-process (concat "gm convert -density " density " -units pixelspercentimeter " filename " " (string-trim-extension filename) ".png")
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
        (call-process (concat "gm convert -size " new-size " " filename " -resize " new-size " +profile \"*\" " (string-trim-extension filename) ".png")
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
    ("*.svg" . "C:/Program Files/Google/Chrome/Application/chrome.exe")
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
          (filer-forward-line 1)
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

; Ispell
(setq *ispell-command* "C:/Users/5630/xyzzy/ispell/ispell.exe")

; デフォルトディレクトリ
(set-default-directory "C:/Users/Nao/Dropbox/")

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

(defvar *ghci-command* "cabal repl")
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
