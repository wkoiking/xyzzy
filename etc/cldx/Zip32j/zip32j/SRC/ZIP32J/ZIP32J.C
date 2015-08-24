/*
	zip32j.c
		by Yoshioka Tsuneo(QWF00133@niftyserve.or.jp)

  統合化アーカイバプロジェクト(仮称(?))に合わせた
  ZIP圧縮ライブラリ

		by Yoshioka Tsuneo(QWF00133@niftyserve.or.jp)
		welcome any e-mail!!
		You can use this file as Public Domain Software.
		Copy,Edit,Re-distibute and for any purpose,you can use this file.

*/
#include "resource.h"
#include "zip32j.h"
#include "cmdline.h"
#include "zipapi.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mbstring.h>

static BOOL flagInProcess=0;
static char *zip_output;
static DWORD zip_output_size;

static char password[81];
static int use_password;
static char comment[32*1024];
static char tempdir[MAX_PATH];
static BOOL use_tempdir;

static HINSTANCE _hInst;
static HWND parent_hwnd = NULL;
static HWND status_hwnd = NULL;
static ZPOPT22 *_ZpOptPtr;
static int StatusDialogCancel;
static char sfxdat[1000]="";

WORD WINAPI ZipGetVersion(VOID)
{
	WORD ver;
	VS_FIXEDFILEINFO *verinfo;
	HRSRC res;
	HGLOBAL hres;
	
	res = FindResource(_hInst,VS_VERSION_INFO,RT_VERSION);
	hres = LoadResource(_hInst,res);
	verinfo = (VS_FIXEDFILEINFO *)((char *)LockResource(hres) + 40);
	ver = (verinfo->dwFileVersionMS >> 16) * 100 + (verinfo->dwFileVersionMS & 0xffff);
	return ver;
	/* return ZIP_VERSION; */
}

BOOL WINAPI ZipGetRunning(VOID)
{
	return flagInProcess;
}

BOOL WINAPI ZipConfigDialog(const HWND _hwnd
	, LPSTR _lpszComBuffer,const int _iMode)
{
	MessageBox(_hwnd,"ZIP32J","ZIP32J.DLL Configration",MB_OK);
	return TRUE;
}

static void doevents()
{
	MSG msg;
	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }
}

static BOOL CALLBACK PrintDialogProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message){
	case WM_INITDIALOG:
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			StatusDialogCancel = 1;
			break;
		}
		break;
	}
	return 0;
}

static int WINAPI DummyPrint(LPSTR buf, unsigned long size)
{
	unsigned long out_size;

	doevents();
	if(_ZpOptPtr->fQuiet == 0){
		char tmpbuf[1002];
		int len;
		char *ptr=tmpbuf;
		char *bufptr = buf;
		if(status_hwnd ==NULL){
			StatusDialogCancel = 0;
			status_hwnd = CreateDialog(_hInst,MAKEINTRESOURCE(IDD_DIALOG_STATUS),parent_hwnd,PrintDialogProc);
			ShowWindow(status_hwnd,SW_SHOW);
			SendDlgItemMessage(status_hwnd,IDC_EDIT_STATUS,EM_SETSEL,0,0);
		}
		//len = strlen(tmpbuf);
		//if(len>1000){len=1000;}
		len = size;
		if(len>1000){len = 1000;}
		while(len--){
			if(*bufptr =='\n'){
				*ptr++='\r';
				*ptr++='\n';
			}else if(*buf >= 32){
				*ptr++=*bufptr;
			}
			bufptr++;
		}
		*ptr='\0';
		//strncpy(tmpbuf,buf,len);tmpbuf[len]='\0';
		/*if(len>0 && tmpbuf[len-1] == '\n' && tmpbuf[len-2] != '\r'){
			tmpbuf[len-1] = '\0';
			tmpbuf[len] = '\n';
			tmpbuf[len+1] = 'A';
			tmpbuf[len+2] = '\0';
		}*/
		//SendDlgItemMessage(status_hwnd,IDC_EDIT_STATUS,EM_SETSEL,0,0);
		SendDlgItemMessage(status_hwnd,IDC_EDIT_STATUS,EM_REPLACESEL,1,tmpbuf);
		//SendDlgItemMessage(status_hwnd,IDC_EDIT_STATUS,EM_REPLACESEL,1,"\r\n");
		//SetDlgItemText(status_hwnd,IDC_EDIT_STATUS,buf);		
		UpdateWindow(status_hwnd);
	}
	/*
		printf("ZIP32J.DLL:[%s]\n", buf);
	*/
	if(zip_output==NULL){out_size=0; goto endlabel;}
	if(zip_output_size==0){out_size=0;goto endlabel;}
	if(zip_output_size > size){
		out_size=size;
	}else{
		out_size=zip_output_size-1;
	}
	strncpy(zip_output,buf,out_size);
	zip_output+=out_size;
	*zip_output='\0';
	zip_output_size-=out_size;
	if(zip_output_size==1){zip_output_size=0;}
endlabel:
	return (unsigned int) out_size;
}

static BOOL CALLBACK PasswordDialogProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	char pass1[81],pass2[81];
	switch(message){
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hwnd,0);
		case IDOK:
			GetDlgItemText(hwnd,IDC_EDIT_PASS1,pass1,sizeof(pass1));
			GetDlgItemText(hwnd,IDC_EDIT_PASS2,pass2,sizeof(pass2));
			if(strcmp(pass1,pass2)==0){
				strcpy(password,pass1);
				EndDialog(hwnd,0);
			}else{
				MessageBox(NULL,"Please Input the same password.","ZIP32J.DLL",0);
				SetDlgItemText(hwnd,IDC_EDIT_PASS1,"");
				SetDlgItemText(hwnd,IDC_EDIT_PASS2,"");
			}
		}
	}
	return 0;
}

static void PasswordPrompt(void)
{
	DialogBox(_hInst,MAKEINTRESOURCE(IDD_DIALOG_PASSWORD),parent_hwnd,PasswordDialogProc);
}

static int WINAPI DummyPassword(LPSTR p, int n, const LPSTR m, const LPSTR name)
{
	if(use_password){
		if(password[0]=='\0'){
			PasswordPrompt();
		}
		strcpy(p,password);
		/* p[strlen(password)]='\0'; */
		return 0;
	}else{
		return 1;
	}
}

static int WINAPI DummyService(LPCSTR filename, unsigned long size)
{
//	_RPTF2 (_CRT_WARN, "%s (%d)\n", filename, size);
	return StatusDialogCancel;
}

static BOOL CALLBACK CommentDialogProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message){
	case WM_INITDIALOG:
		SetDlgItemText(hwnd,IDC_EDIT_COMMENT,comment);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			EndDialog(hwnd,FALSE);
			return TRUE;
		case IDOK:
			GetDlgItemText(hwnd,IDC_EDIT_COMMENT,comment,sizeof(comment));
			EndDialog(hwnd,TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

static LPSTR WINAPI DummyComment(LPSTR buf)
{
	strcpy(comment,buf);
	DialogBox(_hInst,MAKEINTRESOURCE(IDD_DIALOG_COMMENT),parent_hwnd,CommentDialogProc);
	strcpy(buf,comment);
	return buf;
}

/*
	コマンドライン(cmdline)を解析してオプション等(ZpOpt,ZpZCL)を設定
	Zcl.FNV,Zcl.lpszZipFNはfreeする必要がある。
*/
static int ParseCommandLine(ZPOPT22 *ZpOpt,ZCL *ZpZCL,char *cmdline)
{
	char **argv=NULL;
	int argc;
	char *arcname=NULL;
	char **files=NULL;
	int filenum=0;
	char **files_clean=NULL;	
	int i;
	char *opt;
	char *rootdir=NULL;
	int return_code=-1;
	char *p;

	memset(ZpOpt,'\0',sizeof(ZPOPT22));
	ZpOpt->fLevel='6';
	ZpOpt->fVerbose=TRUE;	// (^_^;

	use_password=0;
	password[0]='\0';
	sfxdat[0]='\0';

	if((argv=split_cmdline_with_response(cmdline))==NULL) goto endlabel;
	argc=ptrarraylen(argv);
	if((files=malloc(sizeof(char *)*(argc+1)))==NULL) goto endlabel;
	if(argc<2) goto endlabel;
	

	for(i=0;i<argc;i++){
		if(*argv[i]=='-'){
			opt=argv[i]+1;
			if((strncmp(opt,"-sfx",4))==0){
				p=opt+4;
				if(*p=='\0'){
					strcpy(sfxdat,"sfx32gui.dat");
				}else if(*p=='='){
					strcpy(sfxdat,p+1);
				}else{
					goto endlabel;
				}
				continue;
			}
			while(*opt){
				switch(*opt){
				case 'b':
					use_tempdir=TRUE;
					i++;
					if(i>=argc){goto endlabel;}
					strcpy(tempdir,argv[i]);
					break;
				case 'e':
					ZpOpt->fEncrypt=1;
					use_password = 1;
					break;
				case 'P':
					ZpOpt -> fEncrypt = 1;
					i++;
					if(i>=argc){goto endlabel;}
					strcpy(password,argv[i]);
					use_password = 1;
					break;
				case 'S':ZpOpt->fSystem=1;break;
				case '$':ZpOpt->fVolume=TRUE;break;
				case 'X':ZpOpt->fExtra=TRUE;break;
				case 'D': ZpOpt->fNoDirEntries=TRUE; break;
				case 't':
					if (*(opt+1)=='t'){
						ZpOpt->fExcludeDate=1;
					}else if(*(opt+1)=='\0'){
						ZpOpt->fIncludeDate=1;
					}else{
						goto endlabel;
					}
					if(++i>=argc){goto endlabel;}
					strcpy(ZpOpt->Date,argv[i]);
					continue;
					break;
				case 'v':ZpOpt->fVerbose=TRUE;break;
				case 'q':ZpOpt->fQuiet=1;break;
				case 'l':
					if(*(opt+1)=='l'){
						ZpOpt->fCRLF_LF=1;
						opt++;
					}else{
						ZpOpt->fLF_CRLF=1;
					}
					break;
				case 'j': ZpOpt->fJunkDir=1; break;
				case 'g': ZpOpt->fGrow=1; break;
				case 'k': ZpOpt->fForce=TRUE;break;
				case 'm':ZpOpt->fMove=1;break;
				case 'd': ZpOpt->fDeleteEntries=TRUE; break;
				case 'u':ZpOpt->fUpdate=TRUE;break;
				case 'f': ZpOpt->fFreshen=TRUE; break;
				case 'J': ZpOpt->fJunkSFX=1;break;
				case 'o':ZpOpt->fLatestTime=1;break;
				case 'z':ZpOpt->fComment=1;break;
				case 'A': ZpOpt->fOffsets=TRUE; break;
				case '!':ZpOpt->fPrivilege=TRUE;break;
				case 'r':
					ZpOpt->fRecurse=TRUE;
					break;
				case 'R':ZpOpt->fRecurse=2;break;
				case 'F': 
					if(*(opt+1)=='F'){
						ZpOpt->fRepair=2;
						opt++;
					}else{
						ZpOpt->fRepair=1;
					}
					break;
				default:
					if(isdigit(*opt)){
						ZpOpt->fLevel=*opt;
					}else{
					goto endlabel;
					}
					break;
				}
				opt++;
			}
			continue;
		}else{
			if(arcname==NULL){
				if((arcname=strdup(argv[i]))==NULL) goto endlabel;
			}else if(rootdir==NULL  && *argv[i]!='\0' 
				/* && *(argv[i]+strlen(argv[i])-1)=='\\'){ */
				&& ((_mbsrchr(argv[i],'\\')==argv[i]+strlen(argv[i])-1)
				    ||(_mbsrchr(argv[i],'/')==argv[i]+strlen(argv[i])-1))
				){
				if( (rootdir=strdup(argv[i]))==NULL  )goto endlabel;
			}else{
				if(realloc2(&files,sizeof(char*)*(filenum+1))==NULL){
					goto endlabel;
				}
				if((files[filenum]=strdup(argv[i]))==NULL)goto endlabel;
				filenum++;
			}
		}
	}
	if(realloc2(&files,sizeof(char*)*(filenum+1))==NULL){
		goto endlabel;
	}
	files[filenum]=NULL;
	if((files_clean=strarraydup(files))==NULL) goto endlabel;
	if(rootdir!=NULL && *rootdir != '\0'){
		if(rootdir[strlen(rootdir)-1]=='\\'){
			rootdir[strlen(rootdir)-1]='\0';
		}
		strcpy(ZpOpt->szRootDir,rootdir);
	}
	ZpZCL->argc=filenum;
	ZpZCL->lpszZipFN=arcname;
	ZpZCL->FNV=files_clean;
	return_code=0;
endlabel:
	if(files_clean==NULL && arcname!=NULL){free(arcname);arcname=NULL;}
	if(files!=NULL){free(files);files=NULL;}
	if(rootdir!=NULL){free(rootdir);rootdir=NULL;}
	if(argv!=NULL){free(argv);argv=NULL;}
	return return_code;
}

static void ZPOPT22TO23(ZPOPT22 *ZpOpt22,ZPOPT23 *ZpOpt23)
{
	memset(ZpOpt23,0,sizeof(*ZpOpt23));
/* struct for zip2.3 */
	ZpOpt23->Date=ZpOpt22->Date;               /* Date to include after */
	ZpOpt23->szRootDir=ZpOpt22->szRootDir;          /* Directory to use as base for zipping */
	ZpOpt23->fSuffix = ZpOpt22->fSuffix;             /* include suffixes (not implemented) */
	ZpOpt23->fEncrypt = ZpOpt22->fEncrypt;            /* encrypt files */
	ZpOpt23->fSystem = ZpOpt22->fSystem;             /* include system and hidden files */
	ZpOpt23->fVolume = ZpOpt22->fVolume;             /* Include volume label */
	ZpOpt23->fExtra = ZpOpt22->fExtra;              /* Include extra attributes */
	ZpOpt23->fNoDirEntries = ZpOpt22->fNoDirEntries;       /* Do not add directory entries */
	ZpOpt23->fExcludeDate = ZpOpt22->fExcludeDate;        /* Exclude files earlier than specified date */
	ZpOpt23->fIncludeDate = ZpOpt22->fIncludeDate;        /* Include only files earlier than specified date */
	ZpOpt23->fVerbose = ZpOpt22->fVerbose;            /* Mention oddities in zip file structure */
	ZpOpt23->fQuiet = ZpOpt22->fQuiet;              /* Quiet operation */
	ZpOpt23->fCRLF_LF = ZpOpt22->fCRLF_LF;            /* Translate CR/LF to LF */
	ZpOpt23->fLF_CRLF = ZpOpt22->fLF_CRLF;            /* Translate LF to CR/LF */
	ZpOpt23->fJunkDir = ZpOpt22->fJunkDir;            /* Junk directory names */
	ZpOpt23->fGrow = ZpOpt22->fGrow;               /* Allow appending to a zip file */
	ZpOpt23->fForce = ZpOpt22->fForce;              /* Make entries using DOS names (k for Katz) */
	ZpOpt23->fMove = ZpOpt22->fMove;               /* Delete files added or updated in zip file */
	ZpOpt23->fDeleteEntries = ZpOpt22->fDeleteEntries;      /* Delete files from zip file */
	ZpOpt23->fUpdate = ZpOpt22->fUpdate;             /* Update zip file--overwrite only if newer */
	ZpOpt23->fFreshen = ZpOpt22->fFreshen;            /* Freshen zip file--overwrite only */
	ZpOpt23->fJunkSFX = ZpOpt22->fJunkSFX;            /* Junk SFX prefix */
	ZpOpt23->fLatestTime = ZpOpt22->fLatestTime;         /* Set zip file time to time of latest file in it */
	ZpOpt23->fComment = ZpOpt22->fComment;            /* Put comment in zip file */
	ZpOpt23->fOffsets = ZpOpt22->fOffsets;            /* Update archive offsets for SFX files */
	ZpOpt23->fPrivilege = ZpOpt22->fPrivilege;          /* Use privileges (WIN32 only) */
	ZpOpt23->fEncryption = ZpOpt22->fEncryption;         /* TRUE if encryption supported, else FALSE.this is a read only flag */
	ZpOpt23->fRecurse = ZpOpt22->fRecurse;            /* Recurse into subdirectories. 1 => -R, 2 => -r */
#if 0	// zip23 beta
	if(ZpOpt22->fRecurse==2)ZpOpt23->fRecurse=1;}else if(ZpOpt22->fRecurse==1){ZpOpt23->fRecurse=2;}
#else   // zip23 release
	ZpOpt23->fRecurse = ZpOpt22->fRecurse;
	ZpOpt23->szTempDir = tempdir;
	ZpOpt23->fTemp = use_tempdir;
#endif
	ZpOpt23->fRepair = ZpOpt22->fRepair;             /* Repair archive. 1 => -F, 2 => -FF */
	ZpOpt23->fLevel = ZpOpt22->fLevel;              /* Compression level (0 - 9) */
}

static int ZpMakeSFX(const HWND hwnd,char *_sfxdat,char *arcname,ZPOPT22 *ZpOpt)
{
	char sfxname[1000]="";
	FILE *sfxdatfp;
	FILE *sfxexefp;
	FILE *arcfp;
	char sfxdat[1000]="";
	char buf[1024];
	int n,n2;

#if 0
	if(!(_sfxdat[1]==':' && _sfxdat[2]=='\\')){
		GetSystemDirectory(sfxdat,200);
		strcat(sfxdat,"\\");
	}
	strcat(sfxdat,_sfxdat);
#else
	if(!(_sfxdat[1]==':' && _sfxdat[2]=='\\')){
		char *filepart=NULL;
		char path[MAX_PATH];
		DWORD ret = SearchPath(NULL,_sfxdat,NULL,sizeof(path),path,filepart);
		if(ret!=0 && ret<sizeof(path)){
			strcpy(sfxdat,path);
		}
		// GetSystemDirectory(sfxdat,200);
		// strcat(sfxdat,"\\");
	}
	if(sfxdat[0]=='\0'){
		strcpy(sfxdat,_sfxdat);
	}
#endif
	if(!(strlen(arcname)>=4 && stricmp(arcname+strlen(arcname)-4,".ZIP")==0)){
		sprintf(buf,"File Extension is not '.ZIP'. File=[%s]",arcname);
		MessageBox(hwnd,buf,"ZIP32J",0);
		return -1;
	}
	strcpy(sfxname,arcname);
	strcpy(sfxname+strlen(sfxname)-4,".EXE");
	if((sfxdatfp=fopen(sfxdat,"rb"))==NULL){
		sprintf(buf,"Can't open sfx data file[%s]",sfxdat);
		MessageBox(hwnd,buf,"ZIP32J",0);
		return -1;
	}
	if((sfxexefp=fopen(sfxname,"wb"))==NULL){
		sprintf(buf,"Can't open sfx file[%s]",sfxname);
		MessageBox(hwnd,buf,"ZIP32J",0);
		fclose(sfxdatfp);
		return -1;
	}
	while((n=fread(buf,1,1024,sfxdatfp))>0){
		n2 = fwrite(buf,1,n,sfxexefp);
		if(n!=n2){
			MessageBox(hwnd,"Write Error","ZIP32J",0);
			fclose(sfxdatfp);
			fclose(sfxexefp);
			return -1;
		}
	}
	fclose(sfxdatfp);
	if((arcfp=fopen(arcname,"rb"))==NULL){
		sprintf(buf,"Can't open archive file[%s]",arcname);
		MessageBox(hwnd,buf,"ZIP32J",0);
		fclose(sfxexefp);
		return -1;
	}
	while((n=fread(buf,1,1024,arcfp))>0){
		n2 = fwrite(buf,1,n,sfxexefp);
		if(n!=n2){
			MessageBox(hwnd,"Write Error","ZIP32J",0);
			fclose(arcfp);
			fclose(sfxexefp);
			return -1;
		}
	}
	fclose(arcfp);
	fclose(sfxexefp);
	unlink(arcname);
	strcpy(arcname,sfxname);
	ZpOpt->fOffsets = 1;
	return 0;
}

/*
	Zipをコマンドライン形式で呼び出す
*/
int WINAPI Zip(const HWND _hwnd,LPCSTR _szCmdLine,LPSTR _szOutput,const DWORD _dwSize)
{
	ZIPUSERFUNCTIONS ZipUserFunctions;
	ZPOPT22 ZpOpt;
	ZCL Zcl;
	int ret;
	int zip_return_code=1;

	flagInProcess++;
	parent_hwnd = _hwnd;
	/* ZpInit=NULL;ZpArchive=NULL;*/
	Zcl.FNV=NULL;Zcl.lpszZipFN=NULL;
	zip_output=_szOutput; zip_output_size=_dwSize;
	_ZpOptPtr = &ZpOpt;

#ifndef NODLL_TEST
	if(ZIP32DLLLoadLibrary()==-1){return -1;}
#endif
	ZipUserFunctions.print=DummyPrint;
	ZipUserFunctions.password=DummyPassword;
	ZipUserFunctions.comment=DummyComment;
	ZipUserFunctions.ServiceApplication = DummyService;
	if(ZpInit!=NULL){
		ret=(*ZpInit)(&ZipUserFunctions);
		if(ret==0){ goto endlabel;}
	}
	ret=ParseCommandLine(&ZpOpt,&Zcl,(char *)_szCmdLine);
	if(ret==-1){
		goto endlabel;
	}
	if(sfxdat[0]!='\0'){
		ret = ZpMakeSFX(parent_hwnd,sfxdat,Zcl.lpszZipFN,&ZpOpt);
		if(ret!=0)goto endlabel;
	}
	if(ZpSetOptions22!=NULL || ZpSetOptions23!=NULL){
		if(ZIP32DLLVersion()>= (2<<8)+3){
			ZPOPT23 ZpOpt23;
			ZPOPT22TO23(&ZpOpt,&ZpOpt23);
			ret=(*ZpSetOptions23)(&ZpOpt23);
		}else{
			ret=(*ZpSetOptions22)(ZpOpt);
		}
	}
	if(ZpArchive!=NULL){
		ret=(*ZpArchive)(Zcl);
		if(ret!=0){goto endlabel;}
	}
	zip_return_code=1;

endlabel:
	if(status_hwnd){
		/* EndDialog(status_hwnd,0);*/
		DestroyWindow(status_hwnd);
		status_hwnd=NULL;
	}
	if(Zcl.FNV!=NULL)free(Zcl.FNV);
	if(Zcl.lpszZipFN!=NULL)free(Zcl.lpszZipFN);	
	parent_hwnd = NULL;
	flagInProcess--;
	return ret;
}

/*
	現在利用できる関数ならTrue
*/
BOOL WINAPI ZipQueryFunctionList(const int _iFunction)
{
	int r;
	switch( _iFunction){
	case ISARC						:r=TRUE;break;
	case ISARC_GET_VERSION			:r=TRUE;break;
	case ISARC_GET_RUNNING			:r=TRUE;break;

	case ISARC_CONFIG_DIALOG		:r=TRUE;break;
	case ISARC_QUERY_FUNCTION_LIST	:r=TRUE;break;

	default:
		r=FALSE;
		break;
	}
	return r;
}

/*
	暗号化をサポートしていたらTrue
*/
BOOL WINAPI ZipQueryEncryption(VOID)
{
	if (ZIP32DLLLoadLibrary() == -1)
	{
		return FALSE;
	}
	if (strcmp(ZIP32DLLLibName(), "IZIP32J.DLL") == 0)
	{
		char dat[MAX_PATH];
		return SearchPath(NULL, "ZCRYPT32.DLL", NULL, sizeof(dat), dat, NULL);
	}
	if (ZpGetOptions22 != NULL || ZpGetOptions23 != NULL)
	{
		if (ZIP32DLLVersion() >= (2<<8)+3)
		{
			ZPOPT23 ZpOpt23 = (*ZpGetOptions23)();
			return ZpOpt23.fEncryption;
		}
		else
		{
			ZPOPT22 ZpOpt22 = (*ZpGetOptions22)();
			return ZpOpt22.fEncryption;
		}
	}
	return FALSE;
}

#include "findzip.h"
/*
	DLLのメイン関数、、だけど今のところ何も使ってない。
*/
BOOL APIENTRY DllMain( HANDLE hModule, 
                        DWORD ul_reason_for_call, 
                        LPVOID lpReserved )
{
	_hInst = hModule;
    switch( ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
	break;
    case DLL_THREAD_ATTACH:
    break;
    case DLL_THREAD_DETACH:
    break;
    case DLL_PROCESS_DETACH:
		finddll_end();
		ZIP32DLLFreeLibrary();
    break;
    }
    return TRUE;
}
