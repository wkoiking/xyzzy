(provide "x-key-macro")
(in-package "editor")

(require "wip/winapi")

(c:define-dll-entry c:short GetKeyState (c:int) "user32")

(defmacro x-key-macro (keycode x-func x-key-func)
  `'(lambda ()
     (interactive)
;     (funcall
     (call-interactively
      (if (minusp (GetKeyState ,keycode))
	   #',x-key-func
	 #',x-func))))