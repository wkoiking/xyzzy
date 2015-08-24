/*

$Id: Unbelt.c,v 1.2 2005/04/30 17:15:39 hirosoft Exp $

Copyright (c) 2001-2004 Shimada "Misha" Hirofumi
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

About this source : Tester
*/

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <windows.h>
#include "../comm-arc.h"

typedef WORD (WINAPI *UnbelGetVersion_) ();
typedef int (WINAPI *Unbel_) (HWND,LPCSTR,LPSTR,DWORD);
typedef BOOL (WINAPI *UnbelCheckArchive_) (LPCSTR,const int);
typedef HARC (WINAPI *UnbelOpenArchive_) (const HWND,LPCSTR,const DWORD);
typedef int (WINAPI *UnbelFindFirst_) (HARC,LPSTR,INDIVIDUALINFO FAR *);
typedef int (WINAPI *UnbelGetArcFileName_) (HARC,LPSTR,int);
typedef int (WINAPI *UnbelGetFileName_) (HARC,LPSTR,const int);
typedef int (WINAPI *UnbelCloseArchive_) (HARC);


int main(int argc, char* argv[])
{
	char buf[FNAME_MAX32 + 1];
	char buf2[FNAME_MAX32 + 1];
	char cpbuf[500];
	INDIVIDUALINFO inf;
	int v1;
	int ret;
	BOOL chk;
	HARC A;
	int ff;
	int g1;
	int g2;
	HMODULE dll;

	UnbelGetVersion_ UnbelGetVersion;
	Unbel_ Unbel;
	UnbelCheckArchive_ UnbelCheckArchive;
	UnbelOpenArchive_ UnbelOpenArchive;
	UnbelFindFirst_ UnbelFindFirst;
	UnbelGetArcFileName_ UnbelGetArcFileName;
	UnbelGetFileName_ UnbelGetFileName;
	UnbelCloseArchive_ UnbelCloseArchive;
	
	// 決め打ち実験用、常用には適しない。
	/*
	argv[1] = "\"C:\\Documents and Settings\\Shimada Hirofumi\\デスクトップ\\belon\\example.bel\"";
	argv[2] = "\"C:\\Documents and Settings\\Shimada Hirofumi\\\"";
	*/

	printf("Unbel32.dll Tester\n");
	printf("(C)Arcdll Project\n");
	
	dll = LoadLibrary("Unbel32.dll");
	if (dll == NULL)
	{
		printf("Can't find Unbel32.dll\n");
		return -1;
	}
	
	UnbelGetVersion = (UnbelGetVersion_)GetProcAddress(dll,"UnbelGetVersion");
	Unbel = (Unbel_)GetProcAddress(dll,"Unbel");
	UnbelCheckArchive = (UnbelCheckArchive_)GetProcAddress(dll,"UnbelCheckArchive");
	UnbelOpenArchive = (UnbelOpenArchive_)GetProcAddress(dll,"UnbelOpenArchive");
	UnbelFindFirst = (UnbelFindFirst_)GetProcAddress(dll,"UnbelFindFirst");
	UnbelGetArcFileName = (UnbelGetArcFileName_)GetProcAddress(dll,"UnbelGetArcFileName");
	UnbelGetFileName = (UnbelGetFileName_)GetProcAddress(dll,"UnbelGetFileName");
	UnbelCloseArchive = (UnbelCloseArchive_)GetProcAddress(dll,"UnbelCloseArchive");


	printf("IN FILE : %s\n",argv[1]);

	printf("== Get Version ==\n");
	v1 = UnbelGetVersion();
	printf("Unbel32.dll 0.%u\n",v1);

	printf("== Extract ==\n");
	ret = Unbel(NULL,argv[1],NULL,0);
	printf("Unbel Return : %u\n",ret);

	if (argv[2] != NULL && argc >= 3){
		printf("== Extract (With second option)==\n");
		strcpy(cpbuf,argv[1]);
		strcat(cpbuf," ");
		strcat(cpbuf,argv[2]);
		ret = Unbel(NULL,cpbuf,NULL,0);
		printf("Unbel Return : %u\n",ret);
	}

	printf("== Check ==\n");
	chk = UnbelCheckArchive(argv[1],0);
	printf("CheckArchive Return : %u\n",chk);

	printf("== Check (With Type check) ==\n");
	chk = UnbelCheckArchive(argv[1],1);
	printf("CheckArchive Return : %u\n",chk);

	printf("== Find ==\n");
	A = UnbelOpenArchive(NULL,argv[1],0);
	if ( A == NULL )
	{
		printf("OpenArchive Return : NULL\n");
		return 0;
	}

	printf("OpenArchive Return : OK, to Read archive.\n",A);


	ff = UnbelFindFirst(A,"*.*",&inf);
	printf("FindFirst Return : %u\n",ff);

	g1 = UnbelGetArcFileName(A,buf,MAX_PATH);
	g2 = UnbelGetFileName(A,buf2,MAX_PATH);
	
	printf("GetArcFileName Return : %s / %u\n",buf,g1);
	printf("GetFileName Return : %s / %u\n",buf2,g2);

	UnbelCloseArchive(A);

	return 0;
}

