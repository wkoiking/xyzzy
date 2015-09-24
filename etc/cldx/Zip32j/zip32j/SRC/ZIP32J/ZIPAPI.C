/*
	zipapi.c

		by Yoshioka Tsuneo(QWF00133@niftyserve.or.jp)
		welcome any e-mail!!
		You can use this file as Public Domain Software.
		Copy,Edit,Re-distibute and for any purpose,you can use this file.

*/
#include "zipapi.h"
#include <windows.h>
#include <winbase.h>
#include <stdio.h>

static HINSTANCE ZIP32DLLhLib=NULL;
static int ZIP32DLLVersionItr=0;
static char libname[1000]="";

void (WINAPI *ZpVersion)(ZpVer *) = NULL;
int (WINAPI *ZpInit)(ZIPUSERFUNCTIONS *lpZipUserFunc) =NULL;
BOOL (WINAPI *ZpSetOptions22)(ZPOPT22 Opts) =NULL;
BOOL (WINAPI *ZpSetOptions23)(LPZPOPT23 Opts) =NULL;
ZPOPT22 (WINAPI *ZpGetOptions22)(void) =NULL;
ZPOPT23 (WINAPI *ZpGetOptions23)(void) =NULL;
int(WINAPI *ZpArchive)(ZCL C) = NULL;


char *ZIP32DLLLibName()
{
	return libname;
}
int ZIP32DLLVersion(void)
{
	return ZIP32DLLVersionItr;
}
int ZIP32DLLLoadLibrary(void)
{
	HINSTANCE hLib;
	ZpVer ver;
	char str[1000];

	if(ZIP32DLLhLib){return 0;}
	hLib=LoadLibrary("ZIP32.DLL");
	if(hLib){
		strcpy(libname,"ZIP32.DLL");
	}else{
		hLib=LoadLibrary("IZIP32J.DLL");
		if(hLib){
			strcpy(libname,"IZIP32J.DLL");
		}else{
			MessageBox(NULL,"Can't load ZIP32.DLL","Error",MB_ICONEXCLAMATION);
			return -1;
		}
	}

	ZIP32DLLhLib=hLib;
	ZpVersion=GetProcAddress(hLib,"ZpVersion");
	if(ZpVersion==NULL){
		sprintf(str,"Can't GetProcAddress(%s,%s)",libname,"ZpVersion");
		MessageBox(NULL,str,"Error",MB_ICONEXCLAMATION);
		goto errorlabel;
	}
	ZpVersion(&ver);
	ZIP32DLLVersionItr = ver.zip.major*256 + ver.zip.minor;
	
	ZpInit=GetProcAddress(hLib,"ZpInit");
	if(ZpInit==NULL){
		sprintf(str,"Can't GetProcAddress(%s,%s)",libname,"ZpInit");
		MessageBox(NULL,str,"Error",MB_ICONEXCLAMATION);
		goto errorlabel;
	}
	if(ZIP32DLLVersion() >= (2<<8)+3){
		ZpSetOptions23=GetProcAddress(hLib,"ZpSetOptions");
		ZpGetOptions23=GetProcAddress(hLib,"ZpGetOptions");
		if(ZpSetOptions23==NULL || ZpGetOptions23==NULL){
			sprintf(str,"Can't GetProcAddress(%s,%s)",libname,"ZpSetOptions(23)/ZpGetOptions(23)");
			MessageBox(NULL,str,"Error",MB_ICONEXCLAMATION);
			goto errorlabel;
		}
	}else{
		ZpSetOptions22=GetProcAddress(hLib,"ZpSetOptions");
		ZpGetOptions22=GetProcAddress(hLib,"ZpGetOptions");
		if(ZpSetOptions22==NULL || ZpGetOptions22==NULL){
			sprintf(str,"Can't GetProcAddress(%s,%s)",libname,"ZpSetOptions(22)/ZpGetOptions(22)");
			MessageBox(NULL,str,"Error",MB_ICONEXCLAMATION);
			goto errorlabel;
		}
	}
	ZpArchive=GetProcAddress(hLib,"ZpArchive");
	if(ZpArchive==NULL){
		sprintf(str,"Can't GetProcAddress(%s,%s)",libname,"ZpArchive");
		MessageBox(NULL,str,"Error",MB_ICONEXCLAMATION);
		goto errorlabel;
	}

	
	return 0;
errorlabel:
	ZIP32DLLFreeLibrary();
	return -1;
}
int ZIP32DLLFreeLibrary(void)
{
	if(ZIP32DLLhLib==NULL){return 0;}
	ZpVersion=ZpInit=ZpSetOptions22=ZpSetOptions23=ZpGetOptions22=ZpGetOptions23=NULL;
	FreeLibrary(ZIP32DLLhLib);
	ZIP32DLLhLib = NULL;
	return 0;
}


