/*

$Id: Util.h,v 1.4 2005/04/30 17:11:27 hirosoft Exp $

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

#ifndef UTIL_H
#define UTIL_H


char *alloc_memory(int byte);
void delete_memory(char *buf);
BOOL get_command(LPSTR ret,const char *cmd,LPINT pos);
int cmd2argv(const char *cmd,char **argv);
void argvfree(int argc,char **argv);

int Chack_Archive(const char *file);
int Unbel_run(char *filename,char *dest,HWND hwnd,char *bufs,const int run_mode);
HWND MakeProgress(HINSTANCE _inst,HWND OyaWindow);

int ProgressSetText(HWND hwnd,int control,char *moji);
int CALLBACK ProgDlg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
int Open_bel(char *file);
char *Short2Long(const char *file);
void ErrorMsg(void);

//Add by 0.45
//Unace32.dllÇ©ÇÁÇÃà¯ópÇ‰Ç¶ãCÇ…ÇµÇ»Ç¢ÇÊÇ§Ç…ÅB(èŒ)


typedef struct
{
	HWND progress;
	HWND parent;
	HANDLE progress_thread;
	HANDLE run_thread;
	HANDLE event;
	HINSTANCE _inst;
}unacedll;

unacedll* prog_start(HINSTANCE _inst,HWND parent);
void prog_close(unacedll *ace);
DWORD WINAPI prog_thread(LPVOID para);


#endif	/* UTIL_H */

/*EOF*/