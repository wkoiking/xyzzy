/*

$Id: Unbel32.h,v 1.3 2005/04/21 14:44:54 hirosoft Exp $

Copyright (c) 1999-2004 Shimada "Misha" Hirofumi
Copyright (c) 2005 Arcdll Project

Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*/

#ifndef UNBEL32_H
#define UNBEL32_H

#include "comm-arc.h"

#ifdef __cplusplus
extern "C" {
#endif

WORD WINAPI UnbelGetVersion ();
WORD WINAPI UnbelGetSubVersion();
int WINAPI Unbel (HWND hwnd, LPCSTR filename, LPSTR bufs, DWORD size);
int WINAPI UnbelGetFileCount(LPCSTR szArcFile);
BOOL WINAPI UnbelCheckArchive(LPCSTR pFileName,const int iMode);
BOOL WINAPI UnbelConfigDialog(const HWND hWnd,LPSTR pOption,const int iMode);
int WINAPI UnbelFindFirst(HARC harc, LPSTR szWildName,INDIVIDUALINFO FAR *lpSubInfo);
int WINAPI UnbelFindNext(HARC harc,INDIVIDUALINFO FAR *lpSubInfo);
int WINAPI UnbelGetArcFileName(HARC harc,LPSTR lpBuffer,int nSize);
HARC WINAPI UnbelOpenArchive(const HWND hwnd,LPCSTR szFileName,const DWORD dwMode);
int WINAPI UnbelCloseArchive(HARC harc);
int WINAPI UnbelGetFileName(HARC harc,LPSTR lpBuffer, const int nSize);

#ifdef __cplusplus
}
#endif

#endif