#|text

���g�p�@
keep-selection.l(���̃t�@�C��) ��~/site-lisp �ɂ�����
.xyzzy ���C~/site-lisp/siteinit.l �� 
(require "keep-selection")
�Ə����Ă��������D
~/site-lisp/siteinit.l�ɏ������ꍇ�́C�_���v���Ȃ����Ă��������D

���L�[�o�C���h
�f�t�H���g�ł̓L�[�o�C���h����ؐݒ肵�܂���̂ŁC���D�݂Őݒ肵�Ă��������D

��
(global-set-key #\Down  'next-line-kept-selection)
(global-set-key #\Up    'previous-line-kept-selection)
(global-set-key #\Right 'forward-char-kept-selection)
(global-set-key #\Left  'backward-char-kept-selection)

(global-set-key #\C-n   'next-line-kept-selection)
(global-set-key #\C-p   'previous-line-kept-selection)
(global-set-key #\C-f   'forward-char-kept-selection)
(global-set-key #\C-b   'backward-char-kept-selection)

���ƍE��������
���̃v���O�����́CGPL2.0�܂��͂���ȍ~��GPL���C�Z���X�ɏ]����
�z�z�������̂Ƃ��܂��D

�������
m-nakano (@jaist.ac.jp)

|#


(provide "keep-selection")

(in-package "editor")

(export '(move-kept-selection
          keep-selection-key-binds
          next-line-kept-selection
          previous-line-kept-selection
          next-virtual-line-kept-selection
          previous-virtual-line-kept-selection
          forward-char-kept-selection
          backward-char-kept-selection
          next-word-kept-selection
          previous-word-kept-selection
          delete-char-kept-selection
          delete-backward-char-kept-selection
          next-page-kept-selection
          previous-page-kept-selection
          beginning-of-virtual-line-kept-selection
          end-of-virtual-line-kept-selection
          ))

(defun move-kept-selection (fun &rest args)
  (if (pre-selection-p)
      (let ((stype (get-selection-type))
            (bpoint (selection-mark))
            (epoint (selection-point)))
        (prog1
            (apply fun args)
          (save-excursion
            (goto-char epoint)
            (start-selection stype t bpoint)
            (fix-selection-point))))
    (apply fun args)))


(defun kept-selection-key-binds (keyslist keymap)
  (macrolet ((defun-postfix (fsymbol postfix &optional docpostfix)
               (unless docpostfix
                 (setq docpostfix ""))
               `(progn
                  (defun ,(intern (format nil "~A~A" `,fsymbol `,postfix)) ()
                    ,(format nil "~A~A" (documentation `,fsymbol 'function) `,docpostfix)
                    (interactive)
                    (funcall #'move-kept-selection (symbol-function ',fsymbol)))
                  ,(intern (format nil "~A~A" `,fsymbol `,postfix)))))
    (dolist (keys keyslist)
      (let ((bind (lookup-keymap keymap keys)))
        (define-key keymap keys #'(lambda () (interactive)
                                    (funcall #'move-kept-selection bind)))))))

(defun next-line-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'next-line n))

(defun previous-line-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'previous-line n))

(defun next-virtual-line-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'next-virtual-line n))

(defun previous-virtual-line-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'previous-virtual-line n))

(defun forward-char-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'forward-char n))

(defun backward-char-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'backward-char n))

(defun next-word-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'next-word n))

(defun previous-word-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'previous-word n))

(defun delete-char-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'delete-char n))

(defun delete-backward-char-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'delete-backward-char n))

(defun next-page-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'next-page n))

(defun previous-page-kept-selection (&optional (n 1))
  (interactive)
  (move-kept-selection #'previous-page n))

(defun end-of-virtual-line-kept-selection ()
  (interactive)
  (move-kept-selection #'end-of-virtual-line))

(defun beginning-of-virtual-line-kept-selection ()
  (interactive)
  (move-kept-selection #'beginning-of-virtual-line))