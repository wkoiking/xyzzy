;;; -*- Mode: Lisp; Package: WIN-USER -*-
;;; win-window.l --- win-window �̑���

;; Copyright (C) 2001-2005 OHKUBO Hiroshi <ohkubo@s53.xrea.com>

;; Author: OHKUBO Hiroshi <ohkubo@s53.xrea.com>
;; Time-stamp: <2005/10/08 14:59:18 +0900>

;;; Commentary:

;; Description:
;;
;;  Windows �� Window �𑀍삵�܂��B
;;

;; Installation:
;;
;;      1. �A�[�J�C�u��W�J���� win-window.l �� $XYZZY/site-lisp ��
;;         �R�s�[���܂��B
;;
;;      2. �o�C�g�R���p�C�����܂��B
;;
;;              M-x byte-compile-file
;;              Byte compile file: $XYZZY/site-lisp/win-window.l
;;
;;      3. ~/.xyzzy �܂��� $XYZZY/site-lisp/siteinit.l �Ɉȉ��̃R�[�h��
;;         �ǉ����܂��B
;;
;;              (require "win-window")
;;
;;      4. ��L�̐ݒ�𔽉f�����邽�߂ɁAxyzzy ���ċN�����܂��B
;;         siteinit.l �ɋL�q�����ꍇ�� Ctrl �L�[�� Shift �L�[�������Ȃ���
;;         xyzzy ���ċN�����A�_���v�t�@�C�����č\�z���܂��B
;;

;; Uninstallation:
;;
;;      1. win-window.l �Ɋւ���L�q���폜���܂��B
;;
;;      2. siteinit.l �ɋL�q���Ă����ꍇ�� Ctrl �L�[�� Shift �L�[������
;;         �Ȃ��� xyzzy ���ċN�����A�_���v�t�@�C�����č\�z���܂��B
;;

;; Changes:
;;
;;      Sat, 08 Oct 2005 14:34:12 +0900
;;        �E���[���A�h���X��ύX�B
;;        �E���C�Z���X����(�C��BSD���C�Z���X)���L�ځB
;;
;;      Fri, 28 May 2004 10:23:27 +0900
;;        �EGetWindowRect, IsZoomed �̖߂�l�̌^�� int -> BOOL �ɕύX
;;
;;      Sat, 03 May 2003 04:26:39 +0900
;;        �E���ō쐬�B
;;

;; Licence:
;;
;;    win-window �͏C��BSD���C�Z���X�Ɋ�Â��ė��p�\�ł��B
;;    <http://www.opensource.org/licenses/bsd-license.php>
;;
;;
;;    Copyright (C) 2001-2005, OHKUBO Hiroshi.  All rights reserved.
;;
;;    Redistribution and use in source and binary forms, with or without
;;    modification, are permitted provided that the following conditions
;;    are met:
;;
;;    1. Redistributions of source code must retain the above copyright
;;       notice, this list of conditions and the following disclaimer.
;;
;;    2. Redistributions in binary form must reproduce the above copyright
;;       notice, this list of conditions and the following disclaimer in
;;       the documentation and/or other materials provided with the
;;       distribution.
;;
;;    3. Neither the name of the University nor the names of its
;;       contributors may be used to endorse or promote products derived
;;       from this software without specific prior written permission.
;;
;;    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;;    ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;;    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;;    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;;    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;;    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;;    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;;    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;;    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;;    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;;    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;;

;;; Code:

(provide "win-window")

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "foreign")
  (require "wip/winapi"))

(in-package "winapi")

(unless (fboundp 'GetWindowRect)
  (*define-dll-entry BOOL GetWindowRect (HWND (RECT *)) "user32"))
(unless (fboundp 'IsZoomed)
  (*define-dll-entry BOOL IsZoomed (HWND) "user32"))

(in-package "win-user")

(export '(wwindow-zoomed-p
          xyzzy-zoomed-p
          get-wwindow-rectangle
          get-wclient-rectangle
          set-wwindow-rectangle
          get-wwindow-height
          get-wwindow-width
          set-wwindow-height
          set-wwindow-width
          enlarge-xyzzy-by-wwindow-size
          shrink-xyzzy-by-wwindow-size))

(defun wwindow-zoomed-p (hwnd)
  (not (= 0 (IsZoomed hwnd))))

(defun xyzzy-zoomed-p ()
  (wwindow-zoomed-p (ed:get-window-handle)))

(defun get-wwindow-rectangle (hwnd)
  (let ((wr (winapi:make-RECT)))
    (GetWindowRect hwnd wr)
    (list (RECT-left wr)
          (RECT-top wr)
          (RECT-right wr)
          (RECT-bottom wr))))

(defun get-wclient-rectangle (hwnd)
  (let ((wr (make-RECT)))
    (GetClientRect hwnd wr)
    (list (RECT-left wr)
          (RECT-top wr)
          (RECT-right wr)
          (RECT-bottom wr))))

(defun set-wwindow-rectangle (hwnd left top right bottom &optional (repaint t))
  (let ((width (- right left))
        (height (- bottom top)))
    (setq repaint (if repaint 1 0))
    (MoveWindow hwnd left top width height repaint)))

(defun get-wwindow-height (hwnd)
  (let ((rect (get-wwindow-rectangle hwnd)))
    (- (nth 3 rect) (nth 1 rect))))

(defun get-wwindow-width (hwnd)
  (let ((rect (get-wwindow-rectangle hwnd)))
    (- (nth 2 rect) (nth 0 rect))))

(defun set-wwindow-height (hwnd height &optional (repaint t))
  (let* ((rect (get-wwindow-rectangle hwnd)))
    (MoveWindow hwnd (nth 0 rect) (nth 1 rect)
                (- (nth 2 rect) (nth 0 rect))
                (+ (nth 1 rect) height) (if repaint 1 0))))

(defun set-wwindow-width (hwnd width &optional (repaint t))
  (let* ((rect (get-wwindow-rectangle hwnd)))
    (MoveWindow hwnd (nth 0 rect) (nth 1 rect)
                (+ (nth 0 rect) width)
                (- (nth 3 rect) (nth 1 rect)) (if repaint 1 0))))


(defun enlarge-xyzzy-by-wwindow-size (dll-hwnd dll-pos &optional (keep-basis-point t))
  (when (and (integerp dll-hwnd)
             (plusp dll-hwnd))
    (let* ((LEFT 0) (TOP 1) (RIGHT 2) (BOTTOM 3)
           (rect (get-wwindow-rectangle (ed:get-window-handle)))
           (dll-rect (get-wwindow-rectangle dll-hwnd))
           (left (nth LEFT rect))
           (top (nth TOP rect))
           (right (nth RIGHT rect))
           (bottom (nth BOTTOM rect))
           (dll-width (- (nth RIGHT dll-rect) (nth LEFT dll-rect)))
           (dll-height (- (nth BOTTOM dll-rect) (nth TOP dll-rect))))
      (cond
       ((or (and keep-basis-point (eql dll-pos LEFT))
            (eql dll-pos RIGHT))
        (setq right (+ right dll-width)))
       ((or (and keep-basis-point (eql dll-pos TOP))
            (eql dll-pos BOTTOM))
        (setq bottom (+ bottom dll-height)))
       ((eql dll-pos LEFT)
        (setq left (- left dll-width)))
       ((eql dll-pos TOP)
        (setq top (- top dll-height))))
      (set-wwindow-rectangle (ed:get-window-handle) left top right bottom))))

(defun shrink-xyzzy-by-wwindow-size (dll-hwnd dll-pos &optional (keep-basis-point t))
  (when (and (integerp dll-hwnd)
             (plusp dll-hwnd))
    (let* ((LEFT 0) (TOP 1) (RIGHT 2) (BOTTOM 3)
           (rect (get-wwindow-rectangle (ed:get-window-handle)))
           (dll-rect (get-wwindow-rectangle dll-hwnd))
           (left (nth LEFT rect))
           (top (nth TOP rect))
           (right (nth RIGHT rect))
           (bottom (nth BOTTOM rect))
           (dll-width (- (nth RIGHT dll-rect) (nth LEFT dll-rect)))
           (dll-height (- (nth BOTTOM dll-rect) (nth TOP dll-rect))))
      (cond
       ((or (and keep-basis-point (eql dll-pos LEFT))
            (eql dll-pos RIGHT))
        (setq right (- right dll-width)))
       ((or (and keep-basis-point (eql dll-pos TOP))
            (eql dll-pos BOTTOM))
        (setq bottom (- bottom dll-height)))
       ((eql dll-pos LEFT)
        (setq left (+ left dll-width)))
       ((eql dll-pos TOP)
        (setq top (+ top dll-height))))
      (set-wwindow-rectangle (ed:get-window-handle) left top right bottom))))

;;; win-window.l ends here