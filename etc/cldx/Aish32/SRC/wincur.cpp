// wincur.cpp - Windows Cursor Control Class
//
// Copyright (C) 1999 Hiroyuki Shimada. All rights reserved.

#include "wincur.h"

namespace shimaden {

TWinCursor::TWinCursor(HWND hwnd, HINSTANCE hinst, bool recover)
	: f_hwnd(hwnd), f_hinst(hinst), f_recover(recover)
// 引数
//     hwnd   : ウィンドウ・ハンドル
//     hinst  : アプリケーション・インスタンスのハンドル
//              これの意味については LoadCursor WIN32 API のヘルプを参照のこと
//     recover: デストラクタ実行時に、カーソルを初期状態に戻すかどうか
// 戻り値
//     なし
{
	f_hCurs = 0;
}

TWinCursor::~TWinCursor()
{
	if(f_hCurs) {
		if(f_recover) {
			::SetClassLong(f_hwnd, GCL_HCURSOR,
				reinterpret_cast<LONG>(f_hCurs));
		}
	}
}

HCURSOR TWinCursor::SetCursor(LPCTSTR cursor)
// カーソルを cursor に変更する
// 引数
//     cursor: カーソル名文字列またはカーソル・リソース識別子
//             これの意味は LoadCursor WIN32 API のヘルプを参照のこと
// 戻り値
//     失敗: 0
//     成功: 以前のカーソルのハンドル
{
	HCURSOR hCurs;
	HCURSOR ret;

	hCurs = ::LoadCursor(f_hinst, cursor); // カーソルのハンドルをロード
	if(hCurs) {
		if(f_hCurs == 0)
			f_hCurs = hCurs;
		ret = reinterpret_cast<HCURSOR>(::SetClassLong(f_hwnd, GCL_HCURSOR,
			reinterpret_cast<LONG>(hCurs)));
	} else {
		ret = 0;
	}

	return ret;
}

HCURSOR TWinCursor::GetCursor()
// 現在のカーソルのハンドルを返す
// 戻り値
//     失敗: 0
//     成功: 現在のカーソルのハンドル
{
	return reinterpret_cast<HCURSOR>(::GetClassLong(f_hwnd, GCL_HCURSOR));
}


} // namespace shimaden
