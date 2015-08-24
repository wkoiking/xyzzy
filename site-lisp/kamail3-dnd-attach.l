;; -*- Mode: lisp; -*-
#|text
�� Function
xyzzy�����t�@�C���E�O���t�@�C���Ȃǂ��� KaMail V3 �� �h���b�O���h���b
�v�Ńt�@�C����ǉ�����B�V�t�g�������Ȃ���s���ƈ��k���邽�߂̃_�C�A��
�O��\������B

�� License
Permission is hereby granted, free of charge, to any person obtaining 
a copy of this software and associated documentation files (the 
"Software"), to deal in the Software without restriction, including 
without limitation the rights to use, copy, modify, merge, publish, 
distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to 
the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


copyright 2007   whimsical_xyzzy

�EAuthor: mnakano <mnakano.gm������gmail.com>


�� Install
 $XYZZY/site-lisp �ɂ��̃t�@�C����z�u���A~/.kamail3/config.l �ɉ��L��
 �ǉ����܂��B
 (require "kamail3-dnd-attach")


�� Usage
  draft�o�b�t�@�Ƀt�@�C�炩��A�Y�t�������t�@�C���� D&D ���Ă��������B
  �Y�t�t�@�C���Ƃ��Ēǉ�����܂��B
  �t�@�C������́AM-m �őI���t�@�C�� or �J�[�\�������̃t�@�C����Y�t�ł��܂��B

  Shift�������Ȃ���Y�t����� ���k���ēY�t���܂��B
  ���k�t�@�C���͎����ł͍폜����Ȃ��̂ŁA���ӂ��Ă��������B

�� Note
  KaMailV3�Ax-key-macro ���K�v�ł��B

��History
 on 2008-03-16
  �E���ō쐬

|#


(provide "kamail3-dnd-attach")

(require "kamail3/defs")
(in-package "kamail3")

(defvar *archive-work-dir* nil)

(defun draft-add-attachments-internal (files &optional archive-p)
  (let ((odir (default-directory))
        ratio)
    (when files
      (setq *draft-attachment-directory*
            (directory-namestring (car files))))
    (if (or archive-p
            (some #'file-directory-p files))
        (let* ((expanded-files (mapcan #'(lambda (file)
                                           (if (file-directory-p file)
                                               (directory file
                                                          :absolute t
                                                          :file-only t
                                                          :recursive t)
                                             (list file)))
                                       files))
               (archive (draft-attachments-archive-create expanded-files)))
          (if archive
              (if (file-exist-p archive)
                  (draft-add-attachment archive)
                (error "Not found: ~A" archive))
            (error "Canceled"))
          (setq ratio (* (/ (file-length archive)
                            (reduce #'+ (mapcar #'(lambda (x) (if (file-directory-p x)
                                                                  0
                                                                (file-length x)))
                                                expanded-files)))
                         100.0))
          )
      (dolist (file files)
        (draft-add-attachment file)))
    (when (string= (buffer-name (selected-buffer)) *buffer-draft*) (draft-refresh)) ; �ǉ�
;   (draft-refresh);
    (set-default-directory odir)
    (message "attachments added: ~S ~@[~A%~]" files ratio)))

(defun draft-add-attachments-archive-internal (files)
  (draft-add-attachments-internal files t))

;;----------------------------------------------------------------------
;;.. drag-and-drop �ŗL�������̐؂�ւ�
(defun make-draft-drag-and-drop-hook ()
  (let ((current-drag-and-drop-hook *drag-and-drop-hook*))
    #'(lambda (window file-list)
        (let ((buffer (window-buffer window)))
          (if (eq 'draft-mode (buffer-local-value buffer 'buffer-mode))
              (funcall (if (minusp (ed::GetKeyState 16))
                           'draft-add-attachments-archive-internal
                         'draft-add-attachments-internal)
                       file-list)
            (funcall current-drag-and-drop-hook window file-list)))))
  )
(defvar original-drag-and-drop-hook nil)
(defvar drag-and-drop-hook-mode nil)
(defun toggle-draft-drag-and-drop (&optional (flag nil sv))
  (if (ed::toggle-mode 'drag-and-drop-hook-mode flag sv)
      (unless original-drag-and-drop-hook
        (setq original-drag-and-drop-hook *drag-and-drop-hook*)
        (setq *drag-and-drop-hook* (make-draft-drag-and-drop-hook)))
    (progn
      (if original-drag-and-drop-hook
          (setq *drag-and-drop-hook* original-drag-and-drop-hook))
      (setq original-drag-and-drop-hook nil)
      ))
  )

(defun disable-draft-drag-and-drop (pframe)
  (if (equal (ed::pseudo-frame-name pframe) *pframe-name*)
      (toggle-draft-drag-and-drop nil)))

(defun draft-drag-and-drop-selector ()
  (toggle-draft-drag-and-drop
       (equal (ed::pseudo-frame-name ed::*current-pseudo-frame*) *pframe-name*)))


;;----------------------------------------------------------------------
;;... kamail3�t���[���������Ńt�b�N�ɒǉ����ꂽ��Ԃɂ���ݒ�
;; 
;(add-hook '*delete-pseudo-frame-hook* 'disable-draft-drag-and-drop)
;(add-hook '*select-pseudo-frame-hook* 'draft-drag-and-drop-selector)

;;; �t�b�N�ɂ͏�ɒǉ�����Ă��邪�Adraft�o�b�t�@�ł̂�
(toggle-draft-drag-and-drop t)

;;----------------------------------------------------------------------
; .. �t�@�C��őI�������t�@�C����Y�t�t�@�C���ɂ��� Draft�o�b�t�@���J��

; (defun attach-files-draft-buffer (file-list)
;   (interactive)
;   (let ((buffer (find-buffer *buffer-draft*)))
;     (if buffer
;         (set-buffer buffer)
;       (progn
;         (draft-create)
;         (setq buffer (find-buffer *buffer-draft*))))
;     (funcall (if (minusp (ed::GetKeyState 16))
;                  'draft-add-attachments-archive-internal
;                'draft-add-attachments-internal)
;              file-list)
;     (refresh-screen)
;     ))

(defun attach-files-draft-buffer (file-list)
  (interactive)
  (let ((buffer (find-buffer *buffer-draft*)))
    (unless buffer (draft-create)))
  (funcall (if (minusp (ed::GetKeyState 16))
	       'draft-add-attachments-archive-internal
	     'draft-add-attachments-internal)
	   file-list)
  (refresh-screen)
  )

(defun filer-attach-files-to-kamail3 ()
  (let ((files (or (filer-get-mark-files)
                   (list (filer-get-current-file)))))
    (attach-files-draft-buffer files)))
(define-key filer-keymap #\M-m 'filer-attach-files-to-kamail3)