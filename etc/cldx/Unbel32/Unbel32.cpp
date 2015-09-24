/*

$Id: Unbel32.cpp,v 1.9 2005/04/30 17:11:27 hirosoft Exp $

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

// Based example.c
// Belon Format by N.I

About this source : Export Functions
*/


#include <windows.h>
#include <io.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include <stdlib.h>
#include <sys/stat.h>

#include "resource.h"
#include "comm-arc.h"
#include "Unbel32.h"
#include "Util.h"




#define NAME "Unbel32.dll 0.45 (Classic)"
static INDIVIDUALINFO belinfo;
static EXTRACTINGINFO belfile_info;
static BOOL check_run = FALSE;
static HINSTANCE inst;


//Entry Point
extern "C" BOOL WINAPI _export DllMain(HINSTANCE _inst,DWORD dwReason,LPVOID lpReserved)
{ 
    inst = _inst;
    return TRUE;
}


//UnbelGetVersion
//Unbel32.dllのバージョンを獲得します。

WORD WINAPI _export UnbelGetVersion()
{
    return 45;
}


//UnbelGetSubVersion
//Unbel32.dllのサブバージョンを獲得します。
WORD WINAPI _export UnbelGetSubVersion()
{
  return 1;
}


//Unbel
//解凍します。

int WINAPI _export Unbel(HWND hwnd,LPCSTR comm,LPSTR bufs,DWORD size)
{
    int argc;
    char *argv[30];
    char *rbuf = NULL;
    int i = 1;
    

    if(comm == NULL || strlen(comm) <= 0 || (argc = cmd2argv(comm,argv)) == 0)
        return ERROR_COMMAND_NAME;

    int nRet = Unbel_run(argv[0],argv[1],hwnd,rbuf,1);

    //メモリリーク解消
    argvfree(argc,argv);

    return nRet;
}

//UnbelGetFileCount
//アーカイブ内のファイル数を獲得します。

int WINAPI _export UnbelGetFileCount(LPCSTR szArcFile)
{
    if ((Chack_Archive(szArcFile)) == TRUE){
        return 1;
    }
    return -1;
}

//UnbelCheckArchive
//アーカイブが使えるかチェックします。

BOOL WINAPI _export UnbelCheckArchive(LPCSTR pFileName,const int iMode)
{
/*
    char *hk;
    char *bf;
    int s;
    
    if (pFileName == NULL || strlen(pFileName) <= 0)	return FALSE;
    
    if (iMode == 1){

        //ファイル名の処理
        bf = Short2Long(pFileName);

        hk=strrchr(bf,'.');
        strupr(hk);
        s = strlen(hk);

        if(s <= 3 || hk[0] != '.' || hk[1] != 'B' || hk[2] != 'E' || hk[3] != 'L'){
            return FALSE;
        }
    }
*/
    return (Chack_Archive(pFileName));
}

//UnbelOpenArchive
//アーカイブを開きます。

HARC WINAPI _export UnbelOpenArchive(const HWND hwnd,LPCSTR szFileName,const DWORD dwMode){

	HARC dummy_handle;

    if((Open_bel((char *)szFileName)) != 0){
        return NULL;
    }
    check_run = TRUE;

	dummy_handle = GlobalAlloc(GHND,4);
    return dummy_handle;
}

//UnbelFindFirst
//統合アーカイバ方式です。
//dwOriginalSize,dwCompressedSize,wRatio,szFileName,uFlag
//のみ返ります。

int WINAPI _export UnbelFindFirst(HARC harc, LPSTR szWildName,INDIVIDUALINFO FAR *lpSubInfo){
    int flag;

    if((strchr(szWildName,':')) != NULL){

		// TODO : ここでBelonファイル名が指定された場合のみの処理。
		// しかしながら、パス名との判定は困難。
		// 現在はドライブ指定子で比較しているが、ここはユーザーが選択できる形(Dialog + Registory)の
		// 処理とするべきである。

        //0.42以前との互換モード、ここでのみ処理するべき。
        flag = Open_bel(szWildName);
        if (lpSubInfo != NULL){
            memcpy(lpSubInfo,&belinfo,sizeof(INDIVIDUALINFO));
            check_run = TRUE;
        }
        if (flag == 0){
            return 0;  //正常
        }
        return flag;    //エラー有
    }

    //新モード。
    //*.* 123.bmp  のような形式。* かファイル名判定。
    if (check_run == TRUE){
        if (szWildName[0] != '*'){
            if (strcmp(szWildName,belfile_info.szSourceFileName) != 0){
                return ERROR_CANNOT_OPEN;
            }
        }
        if (lpSubInfo != NULL){
            memcpy(lpSubInfo,&belinfo,sizeof(INDIVIDUALINFO));
        }else{
            return ERROR_HARC_ISNOT_OPENED;
        }

        if (belinfo.uFlag == 0){
            belinfo.uFlag = (unsigned int)0;
        }
		check_run = TRUE;
        return belinfo.uFlag;
    }
    return ERROR_HARC_ISNOT_OPENED;
}


//UnbelCloseArchive
//UnbelOpenArchiveでオープンされた書庫を閉じます。

int WINAPI _export UnbelCloseArchive(HARC harc){
	
	if (harc != NULL)	GlobalFree(harc);
	
    ZeroMemory(&belinfo,sizeof(belinfo));
    ZeroMemory(&belfile_info,sizeof(belfile_info));
    check_run = FALSE;

    return 0;
}

//UnbelFindNext
//統合アーカイバ方式です。
//BEL形式は1つのファイルしか処理しないですが、とりあえず。

int WINAPI _export UnbelFindNext(HARC harc,INDIVIDUALINFO FAR *lpSubInfo){
    if (lpSubInfo != NULL){
        memcpy(lpSubInfo,&belinfo,sizeof(INDIVIDUALINFO));
        return -1;
    }
    return ERROR_HARC_ISNOT_OPENED;
}

//UnbelGetArcFileName
//アーカイブファイル名を格納します。

int WINAPI _export UnbelGetArcFileName(HARC harc,LPSTR lpBuffer,int nSize){
    if(check_run == FALSE && harc == NULL)
        return ERROR_NOT_SEARCH_MODE;

    if (lpBuffer != NULL && nSize > 0){
        if ((lstrcpyn(lpBuffer,belinfo.szFileName,nSize)) == NULL)
            return -1;
        lpBuffer[nSize-1] = '\0';
        return 0;
    }
    return ERROR_BUF_TOO_SMALL;
}

//UnbelGetFileName
//圧縮されたファイル名を獲得します。

int WINAPI _export UnbelGetFileName(HARC harc,LPSTR lpBuffer, const int nSize){
    if(check_run == FALSE && harc == NULL)
        return ERROR_NOT_SEARCH_MODE;
    if (lpBuffer!=NULL && nSize > 0) {
        if((lstrcpyn(lpBuffer, belfile_info.szSourceFileName, nSize)) == NULL)
            return -1;
        lpBuffer[nSize-1] = '\0';
        return 0;
    }
    return ERROR_BUF_TOO_SMALL;
}


// Private Fanction

//Chack_Archive
//ファイルがちゃんと読み込めて、16MB以下か
//検査します。

BOOL Chack_Archive(const char *file){
	DWORD size;
	HANDLE han;
	int l;
	int n;
	BOOL result;

	// TODO : ファイルを2系統に開くのをどうにかできないか？

	han = CreateFile(file,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if ((int)han == -1){
		return FALSE;
	}
	size = GetFileSize(han,NULL);
	CloseHandle(han);
	if (size >= 16777216){	// (16777216/1024)/1024=16 となる
		return FALSE;
	}


	//contributed by HAL
	//Belonのヘッダは必ず0x1Fで終わることを利用して判別。

	FILE* fp=fopen(file,"rb");

	for (l=0;l<3;l++)
		fgetc(fp);
	n=fgetc(fp);
	n+=256*fgetc(fp);
	for (l=5;l<n-1;l++)
		fgetc(fp);
	if (fgetc(fp)==0x1F)
		result=TRUE;
	else
		result=FALSE;
	fclose(fp);
	return result;
}

//UnbelConfigDialog
//バージョン情報が現れます。

BOOL WINAPI _export UnbelConfigDialog(const HWND hWnd,LPSTR pOption,const int iMode)
{
    MessageBox (hWnd,"(C)Arcdll Project",NAME,MB_OK|MB_ICONINFORMATION);
    return TRUE;
}


// --- Private function


//Open_bel
//FindFirst系で使うルーチンです。Belonファイルの
//情報を獲得します。

int Open_bel(char *file){
    int flag;
    lstrcpy(belinfo.szFileName,file);
    flag = Unbel_run(file,NULL,NULL,NULL,0);//
    belinfo.uFlag = flag;
    double tmp1 = (double)belinfo.dwOriginalSize;
    double tmp2 = (double)belinfo.dwCompressedSize;
    if (belinfo.dwOriginalSize > 0 && belinfo.dwCompressedSize > 0){
        belinfo.wRatio = (WORD)((tmp2 / tmp1)*100);
    }

    return flag;
}


//ファイルがあるかどうか検査
short file_load_check(char *file)
{
	WIN32_FIND_DATA fi;

	HANDLE tmp = FindFirstFile(file,&fi);
	if (tmp == INVALID_HANDLE_VALUE)
		return 0;
	FindClose(tmp);

	if ( (GetFileAttributes(file)) & FILE_ATTRIBUTE_DIRECTORY == TRUE)
		return 2;	// Folder
	else
		return 1;	// File
}


//Unbel_run
//BELファイルの解凍、テストをします。
//解凍するには、run_mode を 1 に、テストするには run_mode を 0 に
//してください。

int Unbel_run(char *filename,char *dest,HWND hwnd,char *bufs,const int run_mode){

    char *chk1;
    int chk2;
    char *chk3;
    FILE *han;    //入力のハンドル
    FILE *hanb;   //出力のハンドル
    int fs1;    //元ファイルの大きさ
    int fs2;
    int i;
    int j;
    int l;
    int lp;
    HWND progwnd = NULL;
    unsigned char *buf;
    unsigned char *buf2;
    unsigned char *p;
    //char *pp;
    unsigned char *p1;
    unsigned char *p2;
    unsigned char a;
    char *msg = NULL;
    char bf[MAX_PATH];
    char bff[MAX_PATH];
//    WIN32_FIND_DATA fi;
    unacedll *diag = NULL;
    //char *retbuf;

    if (run_mode == 1){
        //プログレスを用意
        diag = prog_start(inst,hwnd);
        progwnd = diag->progress;
        ProgressSetText(progwnd,IDC_INFILE,filename);
        ProgressSetText(progwnd,IDC_STEP,"Open INFILE...");
    }

    /* ファイルをバッファ(buf1)にロード */
    han=fopen(filename,"rb");
    if (han == NULL){
		prog_close(diag);
        return ERROR_ARCHIVE_FILE_OPEN;
    }

    if (run_mode == 1){
        ProgressSetText(progwnd,IDC_STEP,"Read Header...");
    }

    fs1 = filelength( (_fileno(han)) );
    if (run_mode == 0){
        belinfo.dwCompressedSize = (DWORD)fs1;
    }
    buf=(unsigned char *)malloc(fs1);   //元ファイルと同じ大きさのバッファを作る
    fread(buf,sizeof(char),fs1,han); //bufに転送
    fclose(han);        //元ファイル閉じる

    /* 解凍先バッファ(buf2)確保 */
    fs2=buf[0]+buf[1]*256+buf[2]*65536; //先サイズを計算
    if (fs2 >= 16777216){
		prog_close(diag);
        return ERROR_NOT_ARC_FILE;
    }
    if (run_mode == 0){
        belinfo.dwOriginalSize = (DWORD)fs2;
    }

    if (run_mode == 1){
        ProgressSetText(progwnd,IDC_STEP,"Set Main-Buffer...");
    
        p1=buf+8;
        p2=buf+buf[3]+buf[4]*256;
        buf2=(unsigned char*)malloc(fs2);   //先ファイルと同じ大きさのバッファを作る

        /* バッファ(buf2)に解凍 */
        ProgressSetText(progwnd,IDC_STEP,"Extracting...");

        p=buf2;i=0;
        while(i<fs2-1){ //iがfs2-1未満だと
            lp=*(p1++);
            l=(i+(lp&31)>fs2-1)?fs2-i-1:lp&31;
            i+=l;
            for(j=0;j<l;j++){
                *(p++)=*(p2++);
            }
            if(lp!=31){
                if(lp<32){
                    l=*(p1++);
                }else{
                    l=lp/32;
                }
                a=*(p2-1);
                i+=l-1;
                for(j=0;j<l-1;j++){
                    *(p++)=a;
                }
            }
        }
        *p=*p2;
        ProgressSetText(progwnd,IDC_STEP,"Make OUTFILE name...");
    }

    /* 保存ファイルネーム作成 */
    strcpy(bf,filename);

    if (run_mode == 0){
        //テスト時のコード
        chk1 = strrchr((char *)bf,'\\');
        chk1 = chk1 + 1;
        strcpy(bff,chk1);
        strcpy(belinfo.szFileName,bf);  //
    }

    p = (unsigned char *) bf;
    while(*p&&*p!='.')++p;
    *(++p)=buf[5];
    *(++p)=buf[6];
    *(++p)=buf[7];
    *(++p)=0;

    if (run_mode == 0){
        //テスト時のコード
        chk1 = strrchr((char *)bf,'\\');
        chk1 = chk1 + 1;
        strcpy(bff,chk1);
        strcpy(belfile_info.szSourceFileName,bf);


    }else{
        //解凍時のコード
        if ( (lstrlen(dest)) > 0 ){
            //解凍先部分の処理
            chk1 = strrchr(dest,'\\');
            if ((chk2 = (strlen(chk1))) == 1){  //最後に \ がついている
                strcpy(bff,dest);
                chk2 = strlen(bff);
                chk2--;
                bff[chk2] = '\0';
            }

            //解凍先ファイル名の処理
            chk3 = strrchr(bf,'\\');    //名前を獲得
            strcat(bff,(char *)chk3);
            strcpy(bf,bff);
        }

        ProgressSetText(progwnd,IDC_OUTFILE,bf);

        //KAZUu氏による、確認部分
        if ( (file_load_check(bf)) != 0)
            msg = "すでにファイルが存在しています。上書きしますか？";

        if (msg != NULL) {
            if (MessageBox(progwnd, msg, "確認", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES){
                //chmod(bf, S_IREAD | S_IWRITE);
            }else{
                return ERROR_ALREADY_EXIST;
            }
        }

        /* バッファ(buf2)を保存 */
        ProgressSetText(progwnd,IDC_STEP,"Save to OUTFILE...");

        hanb=fopen(bf,"wb");
        if (hanb == NULL){
			prog_close(diag);
            return ERROR_CANNOT_WRITE;
        }
        fwrite((const char *)buf2,sizeof(char),fs2,hanb);
        fclose(hanb);
        free(buf2);
        //プログレスの放棄
        prog_close(diag);
    }
    free(buf);

    return 0;
}


/*EOF*/
