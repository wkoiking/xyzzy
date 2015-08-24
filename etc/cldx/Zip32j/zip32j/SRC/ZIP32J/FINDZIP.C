/* 
	findzip.c
	by 吉岡 恒夫(email:QWF00133@niftyserve.or.jp)
	コピー、改造、利用、再配布等自由
	(PDSと同等に扱ってもらって結構です)

	目的：
	unzip32.dllのUnZip()APIを利用して
	UnZipFindFirst/UnZipFindNext等のAPIを実現 
	# unzip32.dllがこれらのAPIをサポートしたらもはや用なしです。(^^;
	
	UnZip()コマンドの -l オプションから情報を取得。
														*/

#include <wtypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "findzip.h"

#define FINDZIP_VERSION_NUM 1
#define UNZIP32_DLL_NAME "unzip32.dll"
#define UNZIP32_MAX_OUTPUT_SIZE 100000

static BOOL orig_findfirst;	//unzip32.dllのfindfirstが使えるか？(自動判定)

static HINSTANCE hLib;					// unzip32 のライブラリのハンドル

// unzip32.dllの関数(の一部)

// オリジナルの関数。
static BOOL (WINAPI *OrigUnZipQueryFunctionList)(const int iFunction);
static WORD (WINAPI *OrigUnZipGetVersion)(VOID);
int (WINAPI *OrigUnZip)(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize);
static HARC (WINAPI *OrigUnZipOpenArchive)(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode);
static int (WINAPI *OrigUnZipFindFirst)(HARC hArc,
			LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo);
static int (WINAPI *OrigUnZipCloseArchive)(HARC hArc);
static int (WINAPI *OrigUnZipFindNext)(HARC hArc,INDIVIDUALINFO *lpSubInfo);

// 書庫内容(INDIVIDUALINFO形式)のリスト
typedef struct FINDZIP_FIND_LIST_
{
	INDIVIDUALINFO info;
	struct FINDZIP_FIND_LIST_ *next;
}FINDZIP_FIND_LIST;


// OpenArchiveなどの情報はここえ貯える。
typedef struct FINDZIP_FIND_INFO_
{
	FINDZIP_FIND_LIST *list;
	FINDZIP_FIND_LIST *current;
	char arcfilename[FNAME_MAX32];
	HWND hwnd;
	DWORD mode;
}FINDZIP_FIND_INFO;

WORD WINAPI ZipUnZipGetVersion(VOID){return UnZipGetVersion();}
WORD WINAPI UnZipGetVersion(VOID)
{
	if(!finddll_init()){return 0;}
	return OrigUnZipGetVersion();
}

int WINAPI ZipUnZip(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize){
	return UnZip(hWnd,szCmdLine,szOutput,dwSize);
}
int WINAPI UnZip(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize)
{
	if(!finddll_init()){return -1;}
	return OrigUnZip(hWnd,szCmdLine,szOutput,dwSize);
}
HARC WINAPI ZipUnZipOpenArchive(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode){
	return  UnZipOpenArchive(hWnd,szFileName,dwMode);
}
HARC WINAPI UnZipOpenArchive(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode)
{
	FINDZIP_FIND_INFO *handle;

	if(!finddll_init()){return 0;}
	{
		// オリジナルのUnZipOpenArchiveが使えるかチェック
		HARC ret=OrigUnZipOpenArchive(hWnd,szFileName,dwMode);
		if(orig_findfirst){return ret;}
		if(ret!=NULL){
			orig_findfirst=TRUE;
			return ret;
		}
	}

	if((handle=malloc(sizeof(FINDZIP_FIND_INFO)))==NULL){
		return (HARC)NULL;
	}
	if((handle->list=malloc(sizeof(FINDZIP_FIND_LIST)))==NULL){
		return (HARC)NULL;
	}
	handle->list->next=NULL;
	strcpy(handle->arcfilename,szFileName);
	handle->hwnd=hWnd;
	handle->mode=dwMode;
	return handle;
}
/* 右側の空白類を削る*/
static void RightTrim(char *str)
{
	char *ptr;
	for(ptr=str+strlen(str)-1;ptr>=str && isspace(*ptr);ptr--){
		;
	}
	*(ptr+1)='\0';
}
int UnZip32OutputLineToIndividualInfo(char *line,INDIVIDUALINFO *info)
{
	/*
	result of zip -v 
0         1         2         3         4         5         6
0123456789012345678901234567890123456789012345678901234567890123456789
 Length  Method   Size  Ratio   Date    Time   CRC-32     Name
 ------  ------   ----  -----   ----    ----   ------     ----
  13092  Deflate   5384  59%  95-08-25  14:48  e5081f1b   aiueo
 ------          ------  ---                              -------
  13092            5384  58%                              1      

	result of zip -l
0         1         2         3         4         5         6         7         8
012345678901234567890123456789012345678901234567890123456789012345678901234567890123
  Name          Original    Packed  Ratio   Date     Time   Attr Method   CRC-32
--------------  --------  -------- ------ -------- -------- ---- -------- --------
aiueo              13092      5384  59.4% 95-08-25  14:48:34 a--w Deflate e5081f1b
	
	*/
	char method[100];
	char attr[100];
	int ratio1,ratio2;
	int year,month,date,hour,min,sec;
	int ret;
	char *line2;

	if(strlen(line)<82){return -1;}

	line2=line+strlen(line)-(82-14);	//ファイル名を除いた部分
	*line2='\0';line2+=2;
	
	strcpy(info->szFileName,line);
	RightTrim(info->szFileName);	//右の空白削除
	/* -l オプションの結果を解析 */
	ret=sscanf(line2,"%d %d %d.%d%% %d-%d-%d %d:%d:%d %s %s %x"
		,&(info->dwOriginalSize)
		,&(info->dwCompressedSize)
		,&ratio1,&ratio2
		,&year,&month,&date
		,&hour,&min,&sec
		,attr
		,method
		,&(info->dwCRC));
	if(ret!=13){
		return -1;
	}
	info->uFlag=0;
	info->uOSType=(UINT)-1;
	info->wRatio=ratio1*10+ratio2;
	if(year<80){year=year+100;}
	info->wDate=(year-80 << 4+5) | (month <<5) | (date);
	info->wTime=(hour <<5+6) | (min <<5) | (sec / 2);
	strncpy(info->szAttribute,attr,8);
	strcpy(info->szMode,"-zip-");
	
	/* -v オプションの結果を解析
	ret=sscanf(line,"%d %s %d %d%% %d-%d-%d %d:%d %x"
		,&(info->dwOriginalSize)
		,&method
		,&(info->dwCompressedSize)
		,&ratio
		,&year,&month,&date
		,&hour,&second
		,&(info->dwCRC)
	);
	if(ret!=10){
		return -1;
	}
	info->wRatio=ratio*10;
	info->wDate=(year << 4+5) & (month <<5) & (date);
	info->wTime=(hour <<5+6) & (second <<6);
	strcpy(info->szFileName,line+58);
	*/
	return 0;
}
int WINAPI ZipUnZipFindFirst(HARC hArc,LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo){
	return UnZipFindFirst(hArc,szWildName,lpSubInfo);
}
int WINAPI UnZipFindFirst(HARC hArc,LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo)
{
	FINDZIP_FIND_INFO *handle=(FINDZIP_FIND_INFO*)hArc;
	char szOutput[UNZIP32_MAX_OUTPUT_SIZE];
	char szCmdLine[FNAME_MAX32+100];
	char *ptr,*oldptr;
	int ret;
	FINDZIP_FIND_LIST *find_list_ptr;

	if(orig_findfirst){
		return OrigUnZipFindFirst(hArc,szWildName,lpSubInfo);
	}

	// UnZipのコマンドラインを作成
	strcpy(szCmdLine,"-l ");
	strcat(szCmdLine,"\"");
	strcat(szCmdLine,handle->arcfilename);
	strcat(szCmdLine,"\"");
	strcat(szCmdLine," ");
	strcat(szCmdLine,szWildName);

	ret=UnZip(handle->hwnd,szCmdLine,szOutput,UNZIP32_MAX_OUTPUT_SIZE-1);
	if(strlen(szOutput)>=UNZIP32_MAX_OUTPUT_SIZE-3){
		return -1;
	}
	ptr=szOutput;
	while((ptr=strchr(ptr,'\n')) && *ptr++){
		if(strlen(ptr)>10 && strncmp(ptr,"--------------",14)==0){
			break;
		}
	}
	if(ptr==NULL){return -1;}
	(ptr=strchr(ptr,'\n')) && ptr++;
	find_list_ptr=handle->list;
	oldptr=ptr;
	while(ptr=strchr(ptr,'\n')){
		INDIVIDUALINFO info;

		*ptr='\0';ptr++;


		if(strlen(oldptr)>10 && strncmp(oldptr,"--------------",7)==0){
			break;
		}

		if(UnZip32OutputLineToIndividualInfo(oldptr,&info)==-1){
			while(find_list_ptr!=NULL){
				FINDZIP_FIND_LIST *oldfindptr=find_list_ptr;
				find_list_ptr=find_list_ptr->next;
				free(oldfindptr);
			}
			return -1;
		}
		if((find_list_ptr->next=malloc(sizeof(FINDZIP_FIND_LIST)))==NULL){
			FINDZIP_FIND_LIST *oldfindptr;
			find_list_ptr=handle->list;
			while(find_list_ptr!=NULL){
				oldfindptr=find_list_ptr;
				find_list_ptr=find_list_ptr->next;
				free(oldfindptr);
			}
			return -1;
		}
		find_list_ptr=find_list_ptr->next;
		find_list_ptr->next=NULL;
		find_list_ptr->info=info;
		
		oldptr=ptr;
	}
	handle->current=handle->list->next;
	return UnZipFindNext(hArc,lpSubInfo);
}
int WINAPI ZipUnZipCloseArchive(HARC hArc){
	return UnZipCloseArchive(hArc);
}
int WINAPI UnZipCloseArchive(HARC hArc)
{
	FINDZIP_FIND_LIST *find_list_ptr;

	if(orig_findfirst){
		return OrigUnZipCloseArchive(hArc);
	}
	find_list_ptr=((FINDZIP_FIND_INFO*)hArc)->list;

	while(find_list_ptr!=NULL){
		FINDZIP_FIND_LIST *oldfindptr;
		oldfindptr=find_list_ptr;
		find_list_ptr=find_list_ptr->next;
		free(oldfindptr);
	}
	free((FINDZIP_FIND_INFO*)hArc);
	return 0;
}
int WINAPI ZipUnZipFindNext(HARC hArc,INDIVIDUALINFO *lpSubInfo){
	return UnZipFindNext(hArc,lpSubInfo);
}
int WINAPI UnZipFindNext(HARC hArc,INDIVIDUALINFO *lpSubInfo)
{
	FINDZIP_FIND_INFO *ptr=(FINDZIP_FIND_INFO*)hArc;

	if(orig_findfirst){
		return OrigUnZipFindNext(hArc,lpSubInfo);
	}
	
	if(ptr->current ==NULL){return -1;}
	*lpSubInfo=(ptr->current->info);
	ptr->current = ptr->current->next;
	return 0;
}
#define ISARC_OPEN_ARCHIVE				23	/* UnlhaOpenArchive */
#define ISARC_CLOSE_ARCHIVE				24	/* UnlhaCloseArchive */
#define ISARC_FIND_FIRST				25	/* UnlhaFindFirst */
#define ISARC_FIND_NEXT					26	/* UnlhaFindNext */
BOOL WINAPI ZipUnZipQueryFunctionList(const int iFunction){
	return UnZipQueryFunctionList(iFunction);
}
BOOL WINAPI UnZipQueryFunctionList(const int iFunction)
{
	if(finddll_init()==0){return FALSE;}
	switch(iFunction){
	case ISARC_OPEN_ARCHIVE:
	case ISARC_CLOSE_ARCHIVE:
	case ISARC_FIND_FIRST:
	case ISARC_FIND_NEXT:
		return TRUE;
	default:
		return OrigUnZipQueryFunctionList(iFunction);
	}
}


// DLLの初期化 
static BOOL finddll_init(void)
{
	if(hLib){return TRUE;}
	if((hLib=LoadLibrary(UNZIP32_DLL_NAME))<(HINSTANCE)HINSTANCE_ERROR){
		return FALSE;
	}
#if 0
	// 型チェック警告を出さないようにする(うっとおしいので、、)
#pragma warning(disable:4113)
	UnZip=GetProcAddress(hLib,"UnZip");
#pragma warning(default:4113)
#endif /* if 0 */
	
	OrigUnZipQueryFunctionList=(BOOL (WINAPI *)(const int iFunction))
		GetProcAddress(hLib,"UnZipQueryFunctionList");
	OrigUnZipGetVersion=(WORD (WINAPI *)(VOID))
		GetProcAddress(hLib,"UnZipGetVersion");
	OrigUnZip=(int (WINAPI *)(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize))
		GetProcAddress(hLib,"UnZip");

	OrigUnZipOpenArchive=(HARC (WINAPI *)(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode))
		GetProcAddress(hLib,"UnZipOpenArchive");
	OrigUnZipFindFirst= (int (WINAPI *)(HARC hArc,
			LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo))
		GetProcAddress(hLib,"UnZipFindFirst");
	OrigUnZipCloseArchive= (int (WINAPI *)(HARC hArc))
		GetProcAddress(hLib,"UnZipCloseArchive");
	OrigUnZipFindNext=( int (WINAPI *)(HARC hArc,INDIVIDUALINFO *lpSubInfo))
		GetProcAddress(hLib,"UnZipFindNext");
	
	//orig_findfirst=UnZipQueryFunctionList(ISARC_FIND_FIRST);
	orig_findfirst=FALSE;

	return TRUE;
}
BOOL finddll_end(void)
{
	if(hLib==NULL){return TRUE;}
	FreeLibrary(hLib);
	hLib = NULL;
	return TRUE;
}
#if 0
BOOL   WINAPI   DllMain (HANDLE hInst, ULONG ul_reason_for_call,LPVOID lpReserved)
{
   switch( ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
		return dll_init();
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_PROCESS_DETACH:
		return dll_end();
		break;
    case DLL_THREAD_DETACH:
		break;
    }
    return TRUE;
}	
#endif	/* if 0 */

#if 0
//int WINAPI WinMain(HINSTANCE hThisInst,HINSTANCE hPrevInst,LPSTR lpszArgs,int nWinMode)
// デバッグ用のメイン関数(DLLでは不要)
int main(int argc,char *argv[])
{
	HINSTANCE hThisInst=NULL;

	HARC harc;
	INDIVIDUALINFO info;
	int ret;

	DllMain(hThisInst,DLL_PROCESS_ATTACH,0);
	harc=FindZipOpenArchive(NULL,"c:\\tmp\\tar\\aa.zip",0);
	if(harc==NULL){
		exit(1);
	}
	ret=FindZipFindFirst(harc,"",&info);
	printf("original:compress:crc:flag:os:ratio:yy-mm-dd:hh-mm-ss:filename:attr:mode\n");

	while(ret==0){
		printf("%d:%d:%x:%u:%d:%d:%d-%d-%d:%d-%d-%d:%s:%s:%s\n"
			,info.dwOriginalSize
			,info.dwCompressedSize
			,info.dwCRC
			,info.uFlag
			,info.uOSType
			,(int)info.wRatio
			,(info.wDate>>4+5) & 0x7f, (info.wDate>>5) & 0x0f,(info.wDate) & 0x1f
			,(info.wTime>>6+5) & 0x1f,(info.wTime>>5)&0x2f,((info.wTime) & 0x1f)*2
			,info.szFileName
			,info.szAttribute
			,info.szMode);
		/*printf("%s:%d:%d:\n"
			,info.szFileName
			,info.dwOriginalSize
			,info.dwCompressedSize);*/
		ret=FindZipFindNext(harc,&info);
	}
	FindZipCloseArchive(harc);	
	DllMain(hThisInst,DLL_PROCESS_DETACH,0);
	return 0;
}
#endif /* if 0 */