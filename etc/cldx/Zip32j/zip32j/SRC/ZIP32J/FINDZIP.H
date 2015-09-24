/* 
	findzip.h
	by 吉岡 恒夫(email:QWF00133@niftyserve.or.jp)
	コピー、改造、利用、再配布等自由
	(PDSと同等に扱ってもらって結構です)

	目的：
	unzip32.dllのUnZip()APIを利用して
	UnZipFindFirst/UnZipFindNext等のAPIを実現 
	# unzip32.dllがこれらのAPIをサポートしたらもはや用なしです。(^^;
	
	UnZip()コマンドの -l オプションから情報を取得。
														*/

#define ISARC_FIND_FIRST			25
typedef HGLOBAL HARC;
#define FNAME_MAX32	512


typedef struct {
	DWORD	dwOriginalSize;		/* ファイルのサイズ。		*/
	DWORD	dwCompressedSize;	/* 圧縮後のサイズ。		*/
	DWORD	dwCRC;			/* 格納ファイルのチェックサム/CRC */
	UINT	uFlag;			/* 解凍やテストの処理結果	*/
	UINT	uOSType;		/* このファイルの作成に使われたＯＳ。*/
	WORD	wRatio;			/* 圧縮率（パーミル)		*/
	WORD	wDate;			/* 格納ファイルの日付。		*/
	WORD	wTime;			/* 格納ファイルの時刻。		*/
	char	szFileName[FNAME_MAX32 + 1];/* アーカイブファイル名*/
	char	dummy1[3];
	char	szAttribute[8];		/* 格納ファイルの属性。		*/
	char	szMode[8];		/* 格納ファイルの格納モード。	*/
} INDIVIDUALINFO, FAR *LPINDIVIDUALINFO;

#ifndef UNZIP32_H
int WINAPI UnZip(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize);
// BOOL UnZipQueryFunctionList(const int iFunction);
WORD WINAPI UnZipGetVersion(VOID);
HARC WINAPI UnZipOpenArchive(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode);
int WINAPI UnZipFindFirst(HARC hArc,
			LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo);
int WINAPI UnZipCloseArchive(HARC hArc);
int WINAPI UnZipFindNext(HARC hArc,INDIVIDUALINFO *lpSubInfo);
BOOL WINAPI UnZipQueryFunctionList(const int iFunction);
#endif

int WINAPI ZipUnZip(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize);
// BOOL UnZipQueryFunctionList(const int iFunction);
WORD WINAPI ZipUnZipGetVersion(VOID);
HARC WINAPI ZipUnZipOpenArchive(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode);
int WINAPI ZipUnZipFindFirst(HARC hArc,
			LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo);
int WINAPI ZipUnZipCloseArchive(HARC hArc);
int WINAPI ZipUnZipFindNext(HARC hArc,INDIVIDUALINFO *lpSubInfo);
BOOL WINAPI ZipUnZipQueryFunctionList(const int iFunction);

int finddll_init();
int finddll_end();





