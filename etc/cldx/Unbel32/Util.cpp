/*

$Id: Util.cpp,v 1.3 2005/04/30 17:11:27 hirosoft Exp $

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

About this source : Tool Functions
*/

#include <windows.h>
#include <COMMCTRL.H>
#include "resource.h"
#include "Util.h"


//#ifdef STRICT
	#define H_WNDPROC WNDPROC
//#else
//	//C寄りのBC5用。(汗
//	#define H_WNDPROC FARPROC
//#endif



//Alloc Heap Memory
char *alloc_memory(int byte)
{
	return (char *)HeapAlloc(GetProcessHeap(),
		HEAP_ZERO_MEMORY|HEAP_GENERATE_EXCEPTIONS,byte);
}


//Delete Heap Memory
void delete_memory(char *buf)
{
	HeapFree(GetProcessHeap(),0,buf);
}


//Command-line convert
BOOL get_command(LPSTR ret,const char *cmd,LPINT pos)
{
	int ed = 0;
	BOOL ed_loop = FALSE;
	int tmp;
	BOOL DC = FALSE;
	int m = 0;
	const char *string = cmd + *pos;
	BOOL r = TRUE;

	while (ed_loop == FALSE)
	{
		tmp = *string;

		switch (tmp)
		{
		case '\0':
			ed = -1;
			ed_loop = TRUE;
			r = FALSE;
			break;

		case '\"':
			DC = !DC;
			string++;
			break;

		case ' ':
			if (DC == FALSE)
			{
				ed_loop = TRUE;
				string++;
				/*if (*string == '\0')
				{
				retc = 0;
				}*/
				break;
			}

		default:
			ret[m] = tmp;
			m++;
			string++;
			break;
		}
		ed++;
	}

	*pos = *pos + ed;
	ret[m] = '\0';

	return r;
}


//Convert Command-line to Argv
int cmd2argv(const char *cmd,char **argv)
{
	char **_argv = argv;
	char *tmp = alloc_memory(lstrlen(cmd) + 1);
	int p = 0;
	int _argc = 0;
	BOOL end_flag = TRUE;
	
	if (cmd == NULL)
		return 0;
	
	while(end_flag == TRUE)
	{
		end_flag = get_command(tmp,cmd,&p);
		*_argv = alloc_memory(lstrlen(tmp) + 1);
		lstrcpy(*_argv,tmp);
		_argv++;
		_argc++;
	}
	
	delete_memory(tmp);
	return _argc;
}


//Free memory alloced by cmd2argv
void argvfree(int argc,char **argv)
{
	int i;
	for(i = 0;i < argc;i++)
		delete_memory(argv[i]);
}


//MakeProgress
//プログレスダイアログを表示します。

HWND MakeProgress(HINSTANCE _inst,HWND OyaWindow){
	HWND tmp = CreateDialog(_inst,MAKEINTRESOURCE(IDD_PROG),OyaWindow,ProgDlg);
	//ShowWindow(tmp,SW_SHOW);
	return tmp;
}

//CloseProgress
//プログレスダイアログを閉じます。
/* //obsolate.
void CloseProgress(HWND hwnd){
	DestroyWindow(hwnd);
}
*/

//ProgressSetText
//hwndのcontrolにmojiを設定します。

int ProgressSetText(HWND hwnd,int control,char *moji){
	SendDlgItemMessage(hwnd,control,WM_SETTEXT,0,(long)moji);
	return 0;
}


//ProgDlg
//プログレスダイアログのメッセージを処理します。

int CALLBACK ProgDlg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return 0;

	case WM_COMMAND:
	//	switch (LOWORD (wparam))
	//	{
		//case IDOK:
		//	EndDialog(hwnd,0);
		//	break;
	//	}
	break;
	}

	return 0;
}

//Short2Long
//file をショートファイルをロングファイルに変換し、
//返しつつ、名前部分をextに格納します。

char *Short2Long(const char *file){

	//LPCTSTR DC = "\\";
	WIN32_FIND_DATA fi;
	char short_file[MAX_PATH] = "";
	char long_file[MAX_PATH] = "";
	char tmp_file[MAX_PATH] = "";
	LPTSTR tmp_file2 = "";
	int p;
	char *ret;

	lstrcpy(short_file,file);
	//ファイルがあるか検査
	if(file == NULL || ((FindFirstFile(file,&fi)) == INVALID_HANDLE_VALUE)){
		return NULL;
	}

	//ショートファイルをロングに変換しつつ、名前部分を
	//獲得
	do{
		tmp_file2 = strrchr (short_file,'\\');
		if (tmp_file2 != NULL){
			p = tmp_file2 - short_file;
			if (lstrlen (long_file) == 0){
				lstrcpy(long_file,fi.cFileName);
			}else{
				lstrcpy(tmp_file,fi.cFileName);
				lstrcat(tmp_file,"\\");
				lstrcat(tmp_file,long_file);
				lstrcpy(long_file,tmp_file);
			}
			short_file[p] = '\0';
			FindFirstFile(short_file,&fi);
		}else{
			lstrcpy(tmp_file,short_file);
			lstrcat(tmp_file,"\\");
			lstrcat(tmp_file,long_file);
			lstrcpy(long_file,tmp_file);
		}
	}while(tmp_file2 != NULL);

	//旧式。
	ret = &long_file[0];

	//ret = _strdup(long_file);
	return ret;
}

//ErrorMsg
//Windows規定のエラーメッセージをダイアログに出力します。

void ErrorMsg(void){
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,NULL,GetLastError(),0,(LPTSTR) &lpMsgBuf,500,NULL);
	MessageBox( NULL,(const char *)lpMsgBuf, "HiroRAR - System Error", MB_OK | MB_ICONINFORMATION );
	LocalFree(lpMsgBuf);
}


/* Add by 0.45 */


/* Show progress */
unacedll* prog_start(HINSTANCE _inst,HWND parent)
{
	DWORD tid;
	
	unacedll *ace = (unacedll *)malloc(sizeof(unacedll));
	ace->parent = parent;
	ace->_inst = _inst;

	if ( (ace->event = CreateEvent(NULL,TRUE,FALSE,NULL)) == NULL)
		return FALSE;


	ace->progress_thread = CreateThread(NULL,0,prog_thread,ace,0,&tid);
	if(ace->progress_thread == NULL)
		return NULL;

	/* Wait creating Window */
	WaitForSingleObject(ace->event,INFINITE);

	if (ace->progress == NULL)
	{
		prog_close(ace);
		return NULL;
	}

	return ace;
}


//Close progress
void prog_close(unacedll *ace)
{
	if (ace == NULL)
		return;
	
	if (ace->progress != NULL)
		DestroyWindow(ace->progress);
	CloseHandle(ace->event);
	CloseHandle(ace->progress_thread);

	free(ace);
}


/* Progress Thread */
DWORD WINAPI prog_thread(LPVOID para)
{
	unacedll *ace = (unacedll *)para;
	MSG msg;
	DWORD ret;

	ace->progress = MakeProgress(ace->_inst,ace->parent);

	if (ace->progress == NULL)
	{
		SetEvent(ace->event);
		return 1;
	}

	ShowWindow(ace->progress,SW_SHOW);
	ret = SetEvent(ace->event);

	while(GetMessage(&msg,NULL,0,0) > 0)
	{
		if ( IsDialogMessage(ace->progress,&msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}