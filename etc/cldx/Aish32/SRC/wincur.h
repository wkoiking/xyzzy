// wincur.cpp - Windows Cursor Control Class
//
// Copyright (C) 1999 Hiroyuki Shimada. All rights reserved.
//
#if !defined(WDN_WINCUR_H)
#define WDN_WINCUR_H

#include <windows.h>
#include <winuser.h>

namespace shimaden {

class TWinCursor
{
private:
protected:
	const HWND f_hwnd;
	const HINSTANCE f_hinst;
	const bool f_recover;
	HCURSOR f_hCurs;  // カーソルのハンドル
public:
	TWinCursor(HWND hwnd, HINSTANCE hinst = 0, bool recover = true);
	~TWinCursor();

	HCURSOR SetCursor(LPCTSTR cursor);
	HCURSOR GetCursor();
};




} // namespace shimaden

#endif
