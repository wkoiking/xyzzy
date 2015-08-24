;;; -*- Mode: Lisp; Package: TREEVIEW -*-
;;; get-item.l --- treeview

;; Copyright (C) 2001-2006 OHKUBO Hiroshi <ohkubo@s53.xrea.com>

;; Author: OHKUBO Hiroshi <ohkubo@s53.xrea.com>
;; Time-stamp: <2006/11/03 05:58:38 +0900>

;;; Code:

(provide "treeview/get-item")

(require "treeview/defs")
(require "treeview/dll")

(in-package "treeview")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; item �擾
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun treeview-get-selected-item ()
  "���ݑI�����Ă���A�C�e�����擾�B"
  (treeview-dll-get-selected-item *treeview-id*))

(defun treeview-get-child-item (hitem)
  "item �̐擪�q�A�C�e�����擾�B
�q item �����݂��Ȃ��ꍇ�� 0 ��Ԃ��B"
  (treeview-dll-get-child-item *treeview-id* hitem))
(defun treeview-get-next-sibling-item (hitem)
  "item �̎��̌Z��A�C�e��(���̃A�C�e��)���擾�B"
  (treeview-dll-get-next-sibling-item *treeview-id* hitem))
(defun treeview-get-prev-sibling-item (hitem)
  "item �̑O�̌Z��A�C�e��(��̃A�C�e��)���擾�B"
  (treeview-dll-get-prev-sibling-item *treeview-id* hitem))
(defun treeview-get-parent-item (hitem)
  "item �̐e�A�C�e�����擾�B"
  (treeview-dll-get-parent-item *treeview-id* hitem))
(defun treeview-get-next-visible-item (hitem)
  "���̕\���A�C�e�����擾�B"
  (treeview-dll-get-next-item *treeview-id* hitem winapi:TVGN_NEXTVISIBLE))
(defun treeview-get-prev-visible-item (hitem)
  "�O�̕\���A�C�e�����擾�B"
  (treeview-dll-get-next-item *treeview-id* hitem winapi:TVGN_PREVIOUSVISIBLE))

(defun treeview-get-root-item ()
  "treeview �̈�ԏ�̃A�C�e�����擾�B"
  (treeview-dll-get-root-item *treeview-id*))
(defun treeview-get-first-visible-item ()
  "��ʐ擪�\���A�C�e�����擾�B"
  (treeview-dll-get-next-item *treeview-id* 0 winapi:TVGN_FIRSTVISIBLE))
; (setf (symbol-function 'treeview-get-first-window-visible-item)
;       #'treeview-get-first-visible-item)
(defun treeview-get-last-visible-item ()
  "�W�J���Ă��钆�Ŗ����̃A�C�e�����擾"
  (treeview-dll-get-next-item *treeview-id* 0 winapi:TVGN_LASTVISIBLE))
(defun treeview-get-last-window-visible-item ()
  "��ʖ����\���A�C�e�����擾"
  (let (hitem1 hitem2)
    (setq hitem1 (treeview-get-first-visible-item))
    (dotimes (i (1- (treeview-get-visible-count)))
      (setq hitem2 (treeview-get-next-visible-item hitem1))
      (if (treeview-hitem-valid-p hitem2)
          (setq hitem1 hitem2)
        (return)))
    (if (treeview-hitem-valid-p hitem2)
        hitem2 hitem1)))

;;----------------------------------------------------------------------------
;; item �擾�g��
;;----------------------------------------------------------------------------
(defun treeview-get-first-sibling-item (hitem)
  "�ŏ��̌Z��A�C�e��(��ԏ�)���擾�B"
  (let ((return-hitem hitem))
    (loop
      (setq hitem (treeview-get-prev-sibling-item hitem))
      (unless (treeview-hitem-valid-p hitem)
        (return return-hitem))
      (setq return-hitem hitem))))
(defun treeview-get-last-sibling-item (hitem)
  "�Ō�̌Z��A�C�e��(��ԉ�)���擾�B"
  (let ((return-hitem hitem))
    (loop
      (setq hitem (treeview-get-next-sibling-item hitem))
      (unless (treeview-hitem-valid-p hitem)
        (return return-hitem))
      (setq return-hitem hitem))))
(defun treeview-get-youngest-child-item (hitem)
  "item �̈�ԉ��̎q�A�C�e�����擾�B"
  (unless (and (treeview-hitem-valid-p hitem)
               (treeview-item-has-children-p hitem))
    (return-from treeview-get-youngest-child-item 0))
  (setq hitem (treeview-get-child-item hitem))
  (let ((return-hitem hitem))
    (loop
      (setq hitem (treeview-get-next-sibling-item hitem))
      (unless (treeview-hitem-valid-p hitem)
        (return return-hitem))
      (setq return-hitem hitem))))
(defun treeview-get-ancestor-item (hitem)
  "��Ԑe�̃A�C�e�����擾�B"
  (let ((return-hitem 0))
    (loop
      (setq hitem (treeview-get-parent-item hitem))
      (unless (treeview-hitem-valid-p hitem)
        (return return-hitem))
      (setq return-hitem hitem))))
(defun treeview-get-descendants-item (hitem)
  "��ԉ��̎q�A�C�e���̎擾���J��Ԃ������ʂ̃A�C�e�����擾�B"
  (let ((return-hitem 0))
    (loop
      (setq hitem (treeview-get-youngest-child-item hitem))
      (unless (treeview-hitem-valid-p hitem)
        (return return-hitem))
      (setq return-hitem hitem))))
(defun treeview-get-eldest-descendants-item (hitem)
  "��ԏ�̎q�A�C�e���̎擾���J��Ԃ������ʂ̃A�C�e�����擾�B"
  (let ((return-hitem 0))
    (loop
      (setq hitem (treeview-get-child-item hitem))
      (unless (treeview-hitem-valid-p hitem)
        (return return-hitem))
      (setq return-hitem hitem))))
(setf (symbol-function 'treeview-get-youngest-descendants-item)
      #'treeview-get-descendants-item)
(defun treeview-get-up-item (hitem)
  "�\��/��\���Ɋւ�炸���̃A�C�e�����擾�B"
  (unless (treeview-hitem-valid-p hitem)
    (return-from treeview-get-up-item 0))
  (let (hitem1 hitem2)
    (cond
     ((treeview-hitem-valid-p (setq hitem1 (treeview-get-prev-sibling-item hitem)))
      (if (treeview-hitem-valid-p (setq hitem2 (treeview-get-descendants-item hitem1)))
          hitem2
        hitem1))
     ((treeview-hitem-valid-p (setq hitem1 (treeview-get-parent-item hitem)))
      hitem1)
     (t 0))))
(defun treeview-get-down-item (hitem)
  "�\��/��\���Ɋւ�炸����̃A�C�e�����擾�B"
  (unless (treeview-hitem-valid-p hitem)
    (return-from treeview-get-down-item 0))
  (let (hitem1 hitem2)
    (cond
     ((treeview-hitem-valid-p (setq hitem1 (treeview-get-child-item hitem)))
      hitem1)
     ((treeview-hitem-valid-p (setq hitem1 (treeview-get-next-sibling-item hitem)))
      hitem1)
     (t
      (setq hitem1 hitem)
      (while (treeview-hitem-valid-p (setq hitem1 (treeview-get-parent-item hitem1)))
        (if (treeview-hitem-valid-p (setq hitem2 (treeview-get-next-sibling-item hitem1)))
            (return-from treeview-get-down-item hitem2)))
      0))))
(defun treeview-get-bottom-item ()
  "�\��/��\���Ɋւ�炸 treeview �̈�ԉ��̃A�C�e�����擾�B"
  (unless (and (treeview-exist-p)
               (treeview-hitem-valid-p (treeview-get-root-item)))
    (return-from treeview-get-bottom-item 0))
  (let ((hitem1 (treeview-get-last-sibling-item (treeview-get-root-item)))
        hitem2)
    (if (treeview-hitem-valid-p
         (setq hitem2 (treeview-get-descendants-item hitem1)))
        hitem2
      hitem1)))

;;----------------------------------------------------------------------------
;; �J�[�\���L�[�G�~�����[�g
;;----------------------------------------------------------------------------
(defun treeview-get-up-key-item (hitem)
  "item �� Up �L�[�������ɑI������A�C�e�����擾�B"
  (when (treeview-hitem-valid-p hitem)
    (treeview-get-prev-visible-item hitem)))
(defun treeview-get-down-key-item (hitem)
  "item �� Down �L�[�������ɑI������A�C�e�����擾�B"
  (when (treeview-hitem-valid-p hitem)
    (treeview-get-next-visible-item hitem)))
(defun treeview-get-left-key-item (hitem)
  "item �� Left �L�[�������ɑI������A�C�e�����擾�B"
  (when (treeview-hitem-valid-p hitem)
    (if (treeview-expand-p hitem)
        hitem
      (treeview-get-parent-item hitem))))
(defun treeview-get-right-key-item (hitem)
  "item �� Right �L�[�������ɑI������A�C�e�����擾�B"
  (when (treeview-hitem-valid-p hitem)
    (if (treeview-expand-p hitem)
        (treeview-get-child-item hitem)
      hitem)))
(defun treeview-get-pageup-key-item (hitem)
  "item �� PageUp �L�[�������ɑI������A�C�e�����擾�B"
  (when (treeview-hitem-valid-p hitem)
    (let (next-hitem)
      (dotimes (i (1- (treeview-window-height)))
        (setq next-hitem (treeview-get-prev-visible-item hitem))
        (if (treeview-hitem-valid-p next-hitem)
            (setq hitem next-hitem)
          (return)))
      hitem)))
(defun treeview-get-pagedown-key-item (hitem)
  "item �� PageDown �L�[�������ɑI������A�C�e�����擾�B"
  (when (treeview-hitem-valid-p hitem)
    (let (next-hitem)
      (dotimes (i (1- (treeview-window-height)))
        (setq next-hitem (treeview-get-next-visible-item hitem))
        (if (treeview-hitem-valid-p next-hitem)
            (setq hitem next-hitem)
          (return)))
      hitem)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; item �I���A���s
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(defun treeview-select-act-item-1 (hitem item-mover &optional select-only)
  "item ��I���A���s����ۂ̓����֐��B"
  (let* (func-gen func
         (treeview-recenter (treeview-data-get 'treeview::recenter)) ; t, nil, :line
         (recenter-line (treeview-data-get 'treeview::recenter-line)); 0 origin
         (scroll-margin (treeview-data-get 'treeview::scroll-margin)); recenter �D��
         (selected-hitem (treeview-get-selected-item))
         (current-window-line (treeview-get-window-line selected-hitem))
         need-not-recenter-p)
    ;; �I������ item �̎擾
    (unless hitem
      (setq hitem (treeview-get-selected-item)))
    (if (and (treeview-hitem-valid-p hitem)
             (functionp item-mover))
        (setq hitem (funcall item-mover hitem)))
    (unless (treeview-hitem-valid-p hitem)
      (return-from treeview-select-act-item-1 nil))
    ;; recenter/scroll-margin ��̕\���s�̌v�Z
    (cond
     ;; �s�w�� recenter
     ((eq treeview-recenter :line)
      (unless (integerp recenter-line)
        (setq need-not-recenter-p t)))
     ;; recenter
     (treeview-recenter
      (setq recenter-line (truncate (treeview-window-height) 2)))
     ;; scroll-margin
     ((and (integerp scroll-margin) (plusp scroll-margin))
      (setq scroll-margin
            (min scroll-margin (truncate (treeview-window-height) 2)))
      (cond
       ;; ����m�[�h�̏ꍇ
       ((treeview-hitem-equal selected-hitem hitem)
        (when (and (<= scroll-margin current-window-line)
                   (<= current-window-line (- (1- (treeview-window-height)) scroll-margin)))
          (setq need-not-recenter-p t)))
       ;; ��Ɉړ����Ă���ꍇ
       ((treeview-hitem-greaterp selected-hitem hitem)
        (setq recenter-line scroll-margin)
        (when (>= (treeview-get-window-line hitem) recenter-line)
          (setq need-not-recenter-p t)))
       ;; ���Ɉړ����Ă���ꍇ
       (t
        (setq recenter-line (- (1- (treeview-window-height)) scroll-margin))
        (when (< (treeview-get-window-line hitem) recenter-line)
          (setq need-not-recenter-p t)))))
     (t
      (setq need-not-recenter-p t)))
    ;; item �̑I���� recenter/scroll-margin �ł̃X�N���[���ʂ̌v�Z/�X�N���[��
    (if (or need-not-recenter-p
            (and (treeview-item-window-visible-p hitem)
                 (let ((scroll (treeview-recenter-scroll-height hitem recenter-line)))
                   (cond ((not (numberp scroll)) nil)
                         ((zerop scroll) t)
                         ((plusp scroll) (treeview-window-bottom-p))
                         (t              (treeview-window-top-p))))))
        ;; �X�N���[���̕K�v���Ȃ��ꍇ
        (treeview-select-item hitem)
      ;; �X�N���[���̕K�v������ꍇ
      (treeview-suppress-redraw
       (treeview-select-item hitem)
       (treeview-recenter recenter-line)))
    ;; item �̎��s
    (when (and (not select-only)
               (setq func-gen (treeview-data-get 'implicit-func-gen)))
      (when (setq func (funcall func-gen hitem))
        (funcall func)))))

(defun treeview-select-act-item (&optional hitem select-only)
  "item ��I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem nil select-only))

(defun treeview-select-act-child-item (&optional hitem select-only)
  "item �̐擪�q�A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-child-item select-only))
(defun treeview-select-act-next-sibling-item (&optional hitem select-only)
  "item �̎��̌Z��A�C�e�� (���̃A�C�e��)��I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-next-sibling-item select-only))
(defun treeview-select-act-prev-sibling-item (&optional hitem select-only)
  "item �̑O�̌Z��A�C�e�� (��̃A�C�e��)��I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-prev-sibling-item select-only))
(defun treeview-select-act-parent-item (&optional hitem select-only)
  "item �̑O�̐e�A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-parent-item select-only))
(defun treeview-select-act-first-visible-item (&optional hitem select-only)
  "��ʐ擪�\���A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-first-visible-item select-only))
(defun treeview-select-act-last-visible-item (&optional hitem select-only)
  "�W�J���Ă��钆�Ŗ����̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-last-visible-item select-only))
(defun treeview-select-act-last-window-visible-item (&optional hitem select-only)
  "��ʖ����\���A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-last-window-visible-item select-only))
(defun treeview-select-act-next-visible-item (&optional hitem select-only)
  "���̕\���A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-next-visible-item select-only))
(defun treeview-select-act-prev-visible-item (&optional hitem select-only)
  "�O�̕\���A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-prev-visible-item select-only))
(defun treeview-select-act-root-item (&optional select-only)
  "treeview �̈�ԏ�̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 (treeview-get-root-item) nil select-only))
(defun treeview-select-act-youngest-child-item (&optional hitem select-only)
  "item �̈�ԉ��̎q�A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-youngest-child-item select-only))
(defun treeview-select-act-ancestor-item (&optional hitem select-only)
  "��Ԑe�̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-ancestor-item select-only))
(defun treeview-select-act-descendants-item (&optional hitem select-only)
  "��ԉ��̎q�A�C�e���̎擾���J��Ԃ������ʂ̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-descendants-item select-only))
(defun treeview-select-act-eldest-descendants-item (&optional hitem select-only)
  "��ԏ�̎q�A�C�e���̎擾���J��Ԃ������ʂ̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-eldest-descendants-item select-only))
(defun treeview-select-act-youngest-descendants-item (&optional hitem select-only)
  "��ԉ��̎q�A�C�e���̎擾���J��Ԃ������ʂ̃A�C�e����I���A���s�B
treeview-select-act-descendants-item �Ɠ���B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-youngest-descendants-item select-only))
(defun treeview-select-act-up-item (&optional hitem select-only)
  "�\��/��\���Ɋւ�炸���̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-up-item select-only))
(defun treeview-select-act-down-item (&optional hitem select-only)
  "�\��/��\���Ɋւ�炸����̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-down-item select-only))
(defun treeview-select-act-bottom-item (&optional select-only)
  "�\��/��\���Ɋւ�炸 treeview �̈�ԉ��̃A�C�e����I���A���s�B"
  (interactive)
  (treeview-select-act-item-1 (treeview-get-bottom-item) nil select-only))

;;----------------------------------------------------------------------------
;; �J�[�\���L�[�G�~�����[�g
;;----------------------------------------------------------------------------
(defun treeview-select-act-up-key-item (&optional hitem select-only)
  "Up �L�[�������̓�����G�~�����[�g���A���̌�A�C�e�������s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-up-key-item select-only))
(defun treeview-select-act-down-key-item (&optional hitem select-only)
  "Down �L�[�������̓�����G�~�����[�g���A���̌�A�C�e�������s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-down-key-item select-only))
(defun treeview-select-act-left-key-item (&optional hitem select-only)
  "Left �L�[�������̓�����G�~�����[�g���A���̌�A�C�e�������s�B"
  (interactive)
  (unless hitem
    (setq hitem (treeview-get-selected-item)))
  (when (treeview-hitem-valid-p hitem)
    (let ((next-hitem (treeview-get-left-key-item hitem)))
      (treeview-suppress-redraw
       (when (treeview-expand-p hitem)
         (treeview-expand-collapse hitem))
       (treeview-select-act-item-1 next-hitem nil select-only)))))
(defun treeview-select-act-right-key-item (&optional hitem select-only)
  "Right �L�[�������̓�����G�~�����[�g���A���̌�A�C�e�������s�B"
  (interactive)
  (unless hitem
    (setq hitem (treeview-get-selected-item)))
  (when (treeview-hitem-valid-p hitem)
    (let ((next-hitem (treeview-get-right-key-item hitem)))
      (treeview-suppress-redraw
       (when (and (treeview-item-has-children-p hitem)
                  (not (treeview-expand-p hitem)))
         (treeview-expand-expand hitem))
       (treeview-select-act-item-1 next-hitem nil select-only)))))
(defun treeview-select-act-pageup-key-item (&optional hitem select-only)
  "PageUp �L�[�������̓�����G�~�����[�g���A���̌�A�C�e�������s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-pageup-key-item select-only))
(defun treeview-select-act-pagedown-key-item (&optional hitem select-only)
  "PageDown �L�[�������̓�����G�~�����[�g���A���̌�A�C�e�������s�B"
  (interactive)
  (treeview-select-act-item-1 hitem #'treeview-get-pagedown-key-item select-only))

;;----------------------------------------------------------------------------
;; ����
;;----------------------------------------------------------------------------
(defvar *treeview-select-act-item-delay-sec* 0.01
  "treeview-select-act-item-delay �p�ϐ��B")
(defun treeview-select-act-item-delay ()
  "��u�ҋ@���A���݂̃A�C�e�������߂đI���A���s�B"
  (interactive)
  (let* ((selected-hitem (treeview-get-selected-item))
         (app-id *treeview-app-id*) fn)
    (setq fn #'(lambda ()
                 (with-app (app-id)
                   (let ((hitem (treeview-get-selected-item)))
                     (when (treeview-hitem-valid-p hitem)
;                       (treeview-select-act-item selected-hitem);�\����̖��
                       (treeview-select-item selected-hitem)
                       (treeview-select-act-item hitem))))))
    (start-timer *treeview-select-act-item-delay-sec* fn t)))

;;; get-item.l ends here