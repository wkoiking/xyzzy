;;; -*- Mode: Lisp; Package: BUFFER-BAR-CLICK-TO-DELETE -*-
;;;
;;; This file is not part of xyzzy.
;;;
;; Last updated: <2004/11/01 23:15:52 +0900>
;;

(provide "buffer-bar-click-to-delete")

(eval-when (:compile-toplevel :load-toplevel :execute)
  (require "wip/winapi")
  (unless (find-package "buffer-bar-click-to-delete")
	(defpackage "buffer-bar-click-to-delete"
	  (:use "lisp" "editor" "foreign" "winapi"))))

(in-package "winapi")

; マウスのイベント
; init の引数に指定します．
(*define WM_LBUTTONDOWN 513)	; 左押し
(*define WM_LBUTTONUP 514)		; 左離し
(*define WM_LBUTTONDBLCLK 515)	; 左ダブルクリック
(*define WM_RBUTTONDOWN 516)	; 右押し
(*define WM_RBUTTONUP 517)		; 右離し
(*define WM_RBUTTONDBLCLK 518)	; 右ダブルクリック
(*define WM_MBUTTONDOWN 519)	; 真ん中(ホイール)押し
(*define WM_MBUTTONUP 520)		; 真ん中(ホイール)離し
(*define WM_MBUTTONDBLCLK 521)	; 真ん中(ホイール)ダブルクリック

(in-package "buffer-bar-click-to-delete")

;; 外から使うかもしれないもの

(export '(set-buffer-bar-hook-message
		  set-buffer-bar-activate-flag
		  set-buffer-bar-lbutton-flag
		  set-buffer-bar-hook-func
		  init))

(*define-dll-entry void set-buffer-bar-hook-message
  (UINT)
  "buffer-bar-mc-hook"
  "BufferBarSetHookMessage")
(*define-dll-entry void set-buffer-bar-activate-flag
  (BOOL)
  "buffer-bar-mc-hook"
  "BufferBarSetActivateFlag")
(*define-dll-entry void set-buffer-bar-lbutton-flag
  (BOOL)
  "buffer-bar-mc-hook"
  "BufferBarSetLbuttonFlag")
(*define-dll-entry void set-buffer-bar-hook-func
  (LPVOID LPVOID)
  "buffer-bar-mc-hook"
  "BufferBarSetMcHook")

;; 外から使うかもしれないもの終わり


(*define-dll-entry INT buffer-bar-mc-hook-setup
  (HWND LPVOID)
  "buffer-bar-mc-hook"
  "BufferBarMcHookSetup")
(*define-dll-entry LPVOID get-buffer-bar-click-hook
  ()
  "buffer-bar-mc-hook"
  "BufferBarGetOnClickHook")
(*define-dll-entry LPVOID get-buffer-bar-wheel-hook
  ()
  "buffer-bar-mc-hook"
  "BufferBarGetOnWheelHook")

;; クリックしたときに呼ばれる
(defun-c-callable
  BOOL buffer-bar-mc-callback
  ((INT id) (INT sel))
  (cond
   ((= id 1)
	(let ((buffer (nth sel (buffer-list :buffer-bar-order t))))
	  (message (buffer-name buffer))
	  (kill-buffer buffer)
	  (refresh-screen)
	  1))
   ((= id 2)
	 (let ((frame (nth sel editor::*pseudo-frame-list*)))
	   (message (editor::pseudo-frame-name frame))
	   (delete-pseudo-frame frame)
	   (refresh-screen)
	   1))
   (t 0)))

;; ホイールを回した時に呼ばれる
(defun-c-callable
 BOOL buffer-bar-wheel-callback
 ((INT id) (INT delta))
;   (cond
;    ((= id 1)
;     (if (> delta 0))
; 		(next-buffer)
; 	  (previous-buffer))
; 	(refresh-screen)
; 	1)
;    ((= id 2)
;     (if (> delta 0))
; 		(next-pseudo-frame)
; 	  (previous-pseudo-frame))
; 	(refresh-screen)
; 	1)
;    (t 0))
  )

;; 準備する関数
(defun init (&optional key)
  (if key
	  (set-buffer-bar-hook-message key))
  ;; デフォルトの関数を登録
  (or (/= (get-buffer-bar-click-hook) 0)
	  (/= (get-buffer-bar-wheel-hook) 0)
      (set-buffer-bar-hook-func #'buffer-bar-mc-callback
								#'buffer-bar-wheel-callback))
  (let ((fname (si:make-string-chunk
				(editor::pseudo-frame-name
				 (first editor::*pseudo-frame-list*))))
		bname)
	(let ((buffer (first (buffer-list :buffer-bar-order t))))
	  (setq bname (si:make-string-chunk
				   (concat
					(and (buffer-modified-p buffer)
						 "* ")
					(buffer-name buffer)))))
	(if (and bname fname)
		(let ((target (si:make-chunk nil 12)))
		  ;; 2次元配列を作る
		  (si:pack-uint32 target 0 (si:chunk-data bname))
		  (si:pack-uint32 target 4 (si:chunk-data fname))
		  (si:pack-uint32 target 8 0) ; NULL で終端
		  ;; 準備する
		  (buffer-bar-mc-hook-setup (editor:get-window-handle)
									target)))))

;; show-tool-bar を上書き

(defvar old-show-tool-bar nil)

(and (not old-show-tool-bar)
	 (setf old-show-tool-bar
		   (symbol-function 'ed:show-tool-bar))
	 (defun ed:show-tool-bar (name &optional edge x y w)
	   (let ((ret (funcall old-show-tool-bar name edge x y w)))
		 (and (or (eq name 'buffer-bar)
				  (eq name 'ed::pseudo-frame-bar))
			  (buffer-bar-click-to-delete:init))
		 ret)))

;; 以前の方法がいい人はこちら
;(add-hook '*post-startup-hook* 'buffer-bar-click-to-delete:init))
