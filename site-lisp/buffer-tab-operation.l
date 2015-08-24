;;;; �o�b�t�@�^�u�̑��� (by TieC)

(provide "buffer-tab-operation")

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "WinSDK/api/Windows")
  (require "WinSDK/api/CommCtrl"))

(in-package "win-user")

(export 'buffer-tab-move)

(defun tab-bar-handle-list ()
  (do* ((hwndParent (ed:get-window-handle))
	(lpszClass (si:make-string-chunk "SysTabControl32"))
	(hwndChildAfter (FindWindowExA hwndParent NULL lpszClass NULL)
			(FindWindowExA hwndParent hwndChildAfter lpszClass NULL))
	(handle-list '()))
      ((zerop hwndChildAfter)
       handle-list)
    (push hwndChildAfter handle-list)))

(let (buffer-bar-handle)
  (defun get-buffer-bar-handle ()
    (if buffer-bar-handle
	buffer-bar-handle
	(let ((lpString (si:make-string-chunk "OleDropTargetInterface")))
	  (dolist (handle (tab-bar-handle-list))
	    (when (/= (GetPropA handle lpString) NULL)
	      (return (setf buffer-bar-handle handle))))))))

(defun buffer-tab-move (from to)
  (let ((char[1024] (make-c-type-chunk char 1024))
	(cti (make-TCITEMA))
	(hwnd (get-buffer-bar-handle)))
    (setf (TCITEMA-mask cti) (logior TCIF_TEXT TCIF_IMAGE TCIF_PARAM)
	  (TCITEMA-pszText cti) char[1024]
	  (TCITEMA-cchTextMax cti) (si:chunk-size char[1024]))
    (when (= TRUE (SendMessage hwnd TCM_GETITEMA from cti))
      (when (< from to)
	(incf to))
      (setf to (SendMessage hwnd TCM_INSERTITEMA to cti))
      (when (>= to 0)
	(if (< from to)
	    (decf to)
	    (incf from))
	(SendMessage hwnd TCM_DELETEITEM from 0)
	to))))

(in-package "editor")

(export '(internal-buffer-p buffer-tab-position
	  buffer-tab-move-left buffer-tab-move-right
	  *set-buffer-tab-ignore-list*
	  *buffer-tab-position-after-create* *buffer-tab-position-after-delete*
	  close-right-buffer-tabs close-left-buffer-tabs
	  close-other-buffer-tabs close-all-buffer-tabs

	  *buffer-information-symbol-list*
	  save-buffer-information

	  file-system-supports-named-streams-p
	  *buffer-information-stream-name*
	  write-file-with-buffer-information
	  open-file-with-buffer-information

	  enable-separate-modified-buffer-tab
	  separate-modified-buffer-tab-enabled-p
	  separate-buffer-tab))

(defun internal-buffer-p (buffer)
  "��\���o�b�t�@����"
  (char= (schar (buffer-name buffer) 0) #\SPC))

(defun buffer-tab-position (&optional (buffer (selected-buffer)))
  "�o�b�t�@�^�u�̈ʒu��Ԃ�"
  (unless (bufferp buffer)
    (error 'type-error :datum buffer :expected-type 'buffer))
  (unless (internal-buffer-p buffer)
    (position buffer (buffer-list :buffer-bar-order t))))

;; �ړ�
(defun buffer-tab-move-left (&optional buffer)
  "�o�b�t�@�^�u�����Ɉړ�"
  (interactive)
  (let ((pos (buffer-tab-position (or buffer (selected-buffer)))))
    (when pos
      (win-user:buffer-tab-move pos (1- pos)))))
(defun buffer-tab-move-right (&optional buffer)
  "�o�b�t�@�^�u���E�Ɉړ�"
  (interactive)
  (let ((pos (buffer-tab-position (or buffer (selected-buffer)))))
    (when pos
      (win-user:buffer-tab-move pos (1+ pos)))))

(defvar *set-buffer-tab-ignore-list* '("*load kwd*" "XTAGS" "*MAKE-XTAGS*" "*diff*")
  "�ʒu�w������Ȃ��o�b�t�@�^�u�̃��X�g")

(defvar *buffer-tab-position-after-create* nil
  "�o�b�t�@�^�u�쐬��̈ʒu�w�� :top :left :right :bottom")
(defun set-buffer-tab-position-after-create (buffer)
  (unless (or (internal-buffer-p buffer)
	      (not *buffer-tab-position-after-create*)
	      (eq :bottom *buffer-tab-position-after-create*)
	      (member (buffer-name buffer) *set-buffer-tab-ignore-list* :test 'equal))
    (refresh-screen 1)
    (let ((from (buffer-tab-position buffer))
	  (to (case *buffer-tab-position-after-create*
		(:top 0)
		(:left (buffer-tab-position (selected-buffer)))
		(:right (1+ (buffer-tab-position (selected-buffer)))))))
      (when (and from to)
	(win-user:buffer-tab-move from to)))))
(add-hook '*create-buffer-hook* 'set-buffer-tab-position-after-create)

(defvar *buffer-tab-position-after-delete* nil
  "�o�b�t�@�^�u�폜��̈ʒu�w�� :top :left :right :bottom")
(defun set-buffer-tab-position-after-delete (buffer)
  (unless (or (internal-buffer-p buffer)
	      (member (buffer-name buffer) *set-buffer-tab-ignore-list* :test 'equal))
    (let ((top (get-next-buffer :top nil t))
	  (bottom (get-next-buffer :bottom nil t)))
      (with-set-buffer
	(case *buffer-tab-position-after-delete*
	  (:top (set-buffer (if (eq buffer top)
				(get-next-buffer buffer nil t)
				top)))
	  (:left (set-buffer (if (eq buffer top)
				 (get-next-buffer buffer nil t)
				 (get-next-buffer buffer t t))))
	  (:right (set-buffer (if (eq buffer bottom)
				  (get-next-buffer buffer t t)
				  (get-next-buffer buffer nil t))))
	  (:bottom (set-buffer (if (eq buffer bottom)
				   (get-next-buffer buffer t t)
				   bottom)))))))
  t)
(add-hook '*delete-buffer-hook* 'set-buffer-tab-position-after-delete)

#|
(defun confirm-close-buffer-tabs (buffer)
  "�^�u����邩�m�F����"
  (refresh-screen 1)
  (if (need-buffer-save-p buffer)
      (case (message-box
	     (format nil "~s�͕ύX����Ă��܂��B�ύX���e��j�����܂���?" (buffer-name buffer))
	     nil '(:question :button3)
	     :button1 "�͂�(&Y)" :button2 "���ׂĔj��(&A)" :button3 "������(&N)" :button4 "�L�����Z��")
	(:button1 :yes)
	(:button2 :all)
	(:button3 :no)
	(:button4 :cancel))
      :non-confirm))
|#

;; �폜
(defun close-right-buffer-tabs ()
  "���݂̃o�b�t�@�^�u���E���̃o�b�t�@�^�u�����ׂĕ���"
  (interactive)
  (let ((kill-list (cdr (member (selected-buffer) (buffer-list :buffer-bar-order t)))))
    (save-excursion
      (dolist (buffer kill-list)
	(when (need-buffer-save-p buffer)
	  (set-buffer buffer)
	  (refresh-screen 1))
	(if (internal-buffer-p buffer)
	    (return)
	    (kill-buffer buffer))))))

(defun close-left-buffer-tabs ()
  "���݂̃o�b�t�@�^�u��荶���̃o�b�t�@�^�u�����ׂĕ���"
  (interactive)
  (let ((kill-list (subseq (buffer-list :buffer-bar-order t) 0 (buffer-tab-position))))
    (save-excursion
      (dolist (buffer kill-list)
	(when (need-buffer-save-p buffer)
	  (set-buffer buffer)
	  (refresh-screen 1))
	(kill-buffer buffer)))))

(defun close-other-buffer-tabs ()
  "���݂̃o�b�t�@�^�u�ȊO�̃o�b�t�@�^�u�����ׂĕ���"
  (interactive)
  (close-right-buffer-tabs)
  (close-left-buffer-tabs))

(defun close-all-buffer-tabs ()
  "���ׂẴo�b�t�@�^�u�����"
  (interactive)
  (let ((buffer (selected-buffer)))
    (close-other-buffer-tabs)
    (when (need-buffer-save-p buffer)
      (refresh-screen 1))
    (kill-buffer buffer)))

;; ����/����
(defvar *buffer-information-symbol-list* nil
  "�ۑ����� symbol �̃��X�g")
(defun save-buffer-information (filename &optional (buffer (selected-buffer)))
  "Buffer ���̕ۑ�"
  (interactive "F")
  (with-open-file (os filename
		      :direction :output
		      :if-exists :overwrite
		      :if-does-not-exist :create)
    (flet ((write-sexp (sexp)
	     (write sexp :stream os :escape t :circle t)
	     (terpri os)))
      (when (find-package "undo")
	(write-sexp `(enable-post-buffer-modified-hook nil))
	(funcall (intern "write-undo-information" "undo") buffer os))
      (write-sexp `(rename-buffer ,(buffer-name buffer)))
      (write-sexp `(set-buffer-file-name ,(get-buffer-file-name buffer)))
      (write-sexp `(set-buffer-modified-p ,(buffer-modified-p buffer)))
      (write-sexp `(funcall ',(buffer-local-value buffer 'buffer-mode)))
      (write-sexp `(enable-post-buffer-modified-hook
		    ,(post-buffer-modified-hook-enabled-p buffer)))
      (dolist (symbol *buffer-information-symbol-list*)
	(write-sexp `(setq ,symbol ,(buffer-local-value buffer symbol))))
      t)))

(defun file-system-supports-named-streams-p (pathname)
  "�t�@�C���V�X�e�������O�t���X�g���[�����T�|�[�g���Ă��邩�̔���"
  (let* ((root (cond ((pathname-device pathname)
		      (concat (pathname-device pathname) ":\\"))
		     ((pathname-host pathname)
		      (concat "\\\\" (pathname-host pathname) "\\"))
		     (t
		      (concat (pathname-device (default-directory)) ":\\"))))
	 (lpRootPathName (si:make-string-chunk root))
	 (lpFileSystemFlags (c:make-c-type-chunk winapi:LPDWORD)))
    (and (not (zerop (winapi:GetVolumeInformationA
		      lpRootPathName winapi:NULL winapi:NULL winapi:NULL
		      winapi:NULL lpFileSystemFlags winapi:NULL winapi:NULL)))
	 (plusp (logand (c:c-type-chunk-value lpFileSystemFlags) winapi:FILE_NAMED_STREAMS)))))

(defvar *buffer-information-stream-name* "BufferInfo.l")
(defun write-file-with-buffer-information (filename &optional nomsg append char-encoding eol-code)
  (interactive "FWrite file: "
    :title0 "Write File" :default0 *default-write-file-directory*)
  (write-file filename nomsg append char-encoding eol-code)
  (when (file-system-supports-named-streams-p filename)
    (save-buffer-information (concat filename ":" *buffer-information-stream-name*))))

(defun open-file-with-buffer-information (filename)
  (interactive "fOpen file: ")
  (find-file filename)
  (when (file-exist-p
	 (setf filename (concat filename ":" *buffer-information-stream-name*)))
    (load filename :verbose nil :print nil)))

(defun separate-buffer-tab-dialog (instance-list buffer-count &aux (instance-count (length instance-list)))
  (when (< 1 buffer-count)
    (setf instance-list (append instance-list (list (cons nil "�V��������")))))
  (multiple-value-bind (result data)
      (dialog-box
       '(dialog 0 0 219 131
	 (:caption "�^�u�̕���/�ړ���")
	 (:font 9 "MS UI Gothic")
	 (:control
	  (:listbox list nil #x50b10101 7 7 205 97)
	  (:button IDOK "����/�ړ�" #x50010001 107 110 50 14)
	  (:button IDCANCEL "�L�����Z��" #x50010000 162 110 50 14)))
       `((list . ,(mapcar #'cdr instance-list))
	 (list . ,(if (= 1 buffer-count) 0 instance-count)))	;�����I���A�C�e���w��
       '((list :must-match t :enable (IDOK))))
    (values result
	    (car (find (cdr (assoc 'list data))
		       instance-list :test #'equal :key #'cdr)))))

(let (enable-separate-modified-buffer-tab)
  (defun enable-separate-modified-buffer-tab (enable)
    "�ύX���ꂽ�o�b�t�@�^�u�������\�ɂ���"
    (if enable
	(if (file-system-supports-named-streams-p (si:getenv "TMP"))
	    (setf enable-separate-modified-buffer-tab t)
	    (error "�t�@�C���V�X�e�������O�t���X�g���[�����T�|�[�g���Ă��܂���"))
	(setf enable-separate-modified-buffer-tab nil)))

  (defun separate-modified-buffer-tab-enabled-p ()
    enable-separate-modified-buffer-tab)

  (defun separate-buffer-tab (&optional instance)
    "�o�b�t�@�^�u�𕪗�������"
    (interactive "p")
    (let* ((instance-list (delete (si:*instance-number) (list-xyzzy-windows) :key 'car))
	   (instance-count (length instance-list))
	   (buffer-count (count-buffers))
	   (buffer (selected-buffer))
	   (filename (get-buffer-file-name buffer))
	   (cmd (si:system-root)))
      (cond ((and (not instance-list)
		  (= 1 buffer-count))
	     (error "�����ł��܂���"))
	    ((separate-modified-buffer-tab-enabled-p))
	    ((not filename)
	     (error "�t�@�C����ۑ����Ă�������"))
	    ((and (buffer-modified-p buffer)
		  (not (no-or-yes-p "~s�͕ύX����Ă��܂��B�ύX���e��j�����܂���?" buffer)))
	     (return-from separate-buffer-tab)))
      (when (and (interactive-p)
		 (not instance)
		 instance-list)
	(if (= 1 instance-count buffer-count)
	    (setf instance (caar instance-list))
	    (multiple-value-bind (result data)
		(separate-buffer-tab-dialog instance-list buffer-count)
	      (if result
		  (setf instance data)
		  (return-from separate-buffer-tab)))))
      (when (with-set-buffer
	      (save-excursion
		(set-buffer buffer)
		(run-hook-with-args-while-success '*query-kill-buffer-hook*)))
	(if (member instance instance-list :key 'car)
	    (progn
	      (activate-xyzzy-window instance)
	      (setf cmd (concat (merge-pathnames "xyzzycli.exe" cmd))))
	    (setf cmd (concat (merge-pathnames "xyzzy.exe" cmd))))
	(if (separate-modified-buffer-tab-enabled-p)
	    (progn
	      (setf filename (make-temp-file-name))
	      (write-file-with-buffer-information filename t)
	      (call-process (concat cmd " \"-e(progn (open-file-with-buffer-information \\\"" filename "\\\") (delete-file \\\"" filename "\\\"))\"")))
	    (call-process (concat cmd " \"" filename "\"")))
	(if (= 1 buffer-count)
	    (progn
	      (not-modified)
	      (kill-xyzzy))
	    (delete-buffer buffer)))))
  )