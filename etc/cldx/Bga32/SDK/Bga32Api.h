/*===================================================================

	ZLIB -- general purpose compression library
		zlib 1.1.4 is a general purpose data compression library.
		(C) 1995-2002 Jean-loup Gailly and Mark Adler

	LIBBZIP2 -- a program and library for lossless,
				block-sorting data compression.
		bzip2-1.0.2 is distributed under a BSD-style license.
		Copyright (C) 1996-2002 Julian R Seward.  All rights reserved.

	BGA32.DLL -- Archiver by ZLIB(GZIP) / LIBBZIP2(BZIP2)
		Windows' DLL Version
		Copyright(c) 1999-2002 by Toshinobu Kimura. All rights reserved.

=====================================================================*/
#if !defined(__BGA32API_H__)
#define __BGA32API_H__

#define BGA_DLL_VERSION	  37		 /* version 0.37 */

#if !defined(FNAME_MAX32)
#define FNAME_MAX32		512
#define FNAME_MAX	FNAME_MAX32
#else
#if !defined(FNAME_MAX)
#define FNAME_MAX	128
#endif
#endif

#if !defined(COMMENT_MAX)
#define COMMENT_MAX 2048
#endif

#ifndef ARC_DECSTRACT
#define ARC_DECSTRACT

#if defined(__BORLANDC__)
#pragma option -a-
#else
#pragma pack(1)
#endif

typedef HGLOBAL HARCHIVE ;

typedef HGLOBAL HARC ;

typedef struct {
	DWORD dwOriginalSize;				/* ファイルのサイズ */
	DWORD dwCompressedSize;				/* 圧縮後のサイズ */
	DWORD dwCRC;						/* 格納ファイルのチェックサム */
	UINT  uFlag;						/* 解凍やテストの処理結果 */
	UINT  uOSType;						/* ファイル作成時のＯＳ */
	WORD  wRatio;						/* 圧縮率（パーミル) */
	WORD  wDate;						/* 格納ファイルの日付 */
	WORD  wTime;						/* 格納ファイルの時刻 */
	char  szFileName[FNAME_MAX32 + 1];	/* アーカイブファイル名 */
	char  dummy1[3];
	char  szAttribute[8];				/* 格納ファイルの属性 */
	char  szMode[8];					/* 格納ファイルの格納モード */
} INDIVIDUALINFO, FAR *LPINDIVIDUALINFO;

typedef struct {
	DWORD dwFileSize;							/* 格納ファイルのサイズ */
	DWORD dwWriteSize;							/* 解凍して書き込んだサイズ */
	char  szSourceFileName[FNAME_MAX32 + 1];	/* 処理を行う格納ファイル名 */
	char  dummy1[3];
	char  szDestFileName[FNAME_MAX32 + 1];		/* 実際に書き込まれるパス名 */
	char  dummy[3];
} EXTRACTINGINFO, FAR *LPEXTRACTINGINFO;

typedef struct {
	EXTRACTINGINFO exinfo;
	DWORD dwCompressedSize; /* 圧縮後のサイズ。*/
	DWORD dwCRC;			/* 格納ファイルのチェックサム/CRC */
	UINT  uOSType;			/* このファイルの作成に使われたＯＳ */
	WORD  wRatio;			/* 圧縮率（パーミル) */
	WORD  wDate;			/* 格納ファイルの日付。*/
	WORD  wTime;			/* 格納ファイルの時刻。*/
	char  szAttribute[8];	/* 格納ファイルの属性。*/
	char  szMode[8];		/* 格納ファイルの格納モード。*/
} EXTRACTINGINFOEX, *LPEXTRACTINGINFOEX;

typedef struct {
	DWORD			dwStructSize;
	UINT			uCommand;
	DWORD			dwOriginalSize;
	DWORD			dwCompressedSize;
	DWORD			dwAttributes;
	DWORD			dwCRC;
	UINT			uOSType;
	WORD			wRatio;
	FILETIME		ftCreateTime;
	FILETIME		ftAccessTime;
	FILETIME		ftWriteTime;
	char			szFileName[FNAME_MAX32 + 1];
	char			dummy1[3];
	char			szAddFileName[FNAME_MAX32 + 1];
	char			dummy2[3];
}	UNLHA_ENUM_MEMBER_INFO, *LPUNLHA_ENUM_MEMBER_INFO;

typedef BOOL (CALLBACK *UNLHA_WND_ENUMMEMBPROC)(LPUNLHA_ENUM_MEMBER_INFO);

#if !defined(__BORLANDC__)
#pragma pack()
#else
#pragma option -a.
#endif

#endif /* ARC_DECSTRACT */

#if !defined(__BORLANDC__)
#define _export
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* LHA.DLL Compatible API (LHA.DLL V1.1 と互換性を考慮した API 群です） */
int WINAPI _export Bga( const HWND hWnd, LPCSTR pCmdLine,
						LPSTR pOutput, const DWORD dwSize );
WORD WINAPI _export BgaGetVersion();
BOOL WINAPI _export BgaGetRunning();

#if 0	/* 現在未実装です */
BOOL WINAPI _export BgaGetBackGroundMode();
BOOL WINAPI _export BgaSetBackGroundMode( const BOOL bBackGroundMode );
BOOL WINAPI _export BgaGetCursorMode();
BOOL WINAPI _export BgaSetCursorMode( const BOOL bCursorMode );
WORD WINAPI _export BgaGetCursorInterval();
BOOL WINAPI _export BgaSetCursorInterval( const WORD wInterval );

int WINAPI _export BgaCommandLine( HWND hWnd, HINSTANCE hInst,
								   LPCSTR szCmdLine, DWORD nCmdShow );
#endif	/* 現在未実装です */

/*
True Compatible API (『統合アーカイバＡＰＩ』独自の、互換性の高い API 群です）
*/

BOOL WINAPI _export BgaCheckArchive( LPCSTR pFileName, const int iMode );
/**/
#if !defined(CHECKARCHIVE_RAPID)
#define CHECKARCHIVE_RAPID		0	/* 簡易型（高速） */
#define CHECKARCHIVE_BASIC		1	/* 標準型（ヘッダーのみ） */
#define CHECKARCHIVE_FULLCRC	2	/* 完全型（ＣＲＣ等のチェックを含む） */

/* 以下のフラグは上記と組み合わせて使用。*/
#define CHECKARCHIVE_RECOVERY	4	/* 破損ヘッダを読み飛ばして処理 */
#define CHECKARCHIVE_SFX		8	/* SFX かどうかを返す */
#define CHECKARCHIVE_ALL		16	/* ファイルの最後まで検索する */
#endif

BOOL WINAPI _export BgaConfigDialog( const HWND hWnd, LPSTR pOption,
									 const int iMode );
/**/
#if !defined(UNPACK_CONFIG_MODE)
#define UNPACK_CONFIG_MODE		1	/* 解凍（復元）系のコマンド */
#define PACK_CONFIG_MODE		2	/* 圧縮（作成）系のコマンド */
#endif

BOOL WINAPI _export BgaQueryFunctionList( const int iFunction );
/**/
#if !defined(ISARC_FUNCTION_START)
#define ISARC_FUNCTION_START			0
#define ISARC							0	/* Unlha */
#define ISARC_GET_VERSION				1	/* UnlhaGetVersion */
#define ISARC_GET_CURSOR_INTERVAL		2	/* UnlhaGetCursorInterval */
#define ISARC_SET_CURSOR_INTERVAL		3	/* UnlhaSetCursorInterval */
#define ISARC_GET_BACK_GROUND_MODE		4	/* UnlhaGetBackGroundMode */
#define ISARC_SET_BACK_GROUND_MODE		5	/* UnlhaSetBackGroundMode */
#define ISARC_GET_CURSOR_MODE			6	/* UnlhaGetCursorMode */
#define ISARC_SET_CURSOR_MODE			7	/* UnlhaSetCursorMode */
#define ISARC_GET_RUNNING				8	/* UnlhaGetRunning */

#define ISARC_CHECK_ARCHIVE				16	/* UnlhaCheckArchive */
#define ISARC_CONFIG_DIALOG				17	/* UnlhaConfigDialog */
#define ISARC_GET_FILE_COUNT			18	/* UnlhaGetFileCount */
#define ISARC_QUERY_FUNCTION_LIST		19	/* UnlhaQueryFunctionList */
#define ISARC_HOUT						20	/* (UnlhaHOut) */
#define ISARC_STRUCTOUT					21	/* (UnlhaStructOut) */
#define ISARC_GET_ARC_FILE_INFO			22	/* UnlhaGetArcFileInfo */

#define ISARC_OPEN_ARCHIVE				23	/* UnlhaOpenArchive */
#define ISARC_CLOSE_ARCHIVE				24	/* UnlhaCloseArchive */
#define ISARC_FIND_FIRST				25	/* UnlhaFindFirst */
#define ISARC_FIND_NEXT					26	/* UnlhaFindNext */
#define ISARC_EXTRACT					27	/* (UnlhaExtract) */
#define ISARC_ADD						28	/* (UnlhaAdd) */
#define ISARC_MOVE						29	/* (UnlhaMove) */
#define ISARC_DELETE					30	/* (UnlhaDelete) */
#define ISARC_SETOWNERWINDOW			31	/* UnlhaSetOwnerWindow */
#define ISARC_CLEAROWNERWINDOW			32	/* UnlhaClearOwnerWindow */
#define ISARC_SETOWNERWINDOWEX			33	/* UnlhaSetOwnerWindowEx */
#define ISARC_KILLOWNERWINDOWEX			34	/* UnlhaKillOwnerWindowEx */

#define ISARC_GET_ARC_FILE_NAME			40	/* UnlhaGetArcFileName */
#define ISARC_GET_ARC_FILE_SIZE			41	/* UnlhaGetArcFileSize */
#define ISARC_GET_ARC_ORIGINAL_SIZE		42	/* UnlhaArcOriginalSize */
#define ISARC_GET_ARC_COMPRESSED_SIZE	43	/* UnlhaGetArcCompressedSize */
#define ISARC_GET_ARC_RATIO				44	/* UnlhaGetArcRatio */
#define ISARC_GET_ARC_DATE				45	/* UnlhaGetArcDate */
#define ISARC_GET_ARC_TIME				46	/* UnlhaGetArcTime */
#define ISARC_GET_ARC_OS_TYPE			47	/* UnlhaGetArcOSType */
#define ISARC_GET_ARC_IS_SFX_FILE		48	/* UnlhaGetArcIsSFXFile */
#define ISARC_GET_ARC_WRITE_TIME_EX		49	/* UnlhaGetArcWriteTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX	50	/* UnlhaGetArcCreateTimeEx */
#define ISARC_GET_ARC_ACCESS_TIME_EX	51	/* UnlhaGetArcAccessTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX2	52	/* UnlhaGetArcCreateTimeEx2 */
#define ISARC_GET_ARC_WRITE_TIME_EX2	53	/* UnlhaGetArcWriteTimeEx2 */
#define ISARC_GET_FILE_NAME				57	/* UnlhaGetFileName */
#define ISARC_GET_ORIGINAL_SIZE			58	/* UnlhaGetOriginalSize */
#define ISARC_GET_COMPRESSED_SIZE		59	/* UnlhaGetCompressedSize */
#define ISARC_GET_RATIO					60	/* UnlhaGetRatio */
#define ISARC_GET_DATE					61	/* UnlhaGetDate */
#define ISARC_GET_TIME					62	/* UnlhaGetTime */
#define ISARC_GET_CRC					63	/* UnlhaGetCRC */
#define ISARC_GET_ATTRIBUTE				64	/* UnlhaGetAttribute */
#define ISARC_GET_OS_TYPE				65	/* UnlhaGetOSType */
#define ISARC_GET_METHOD				66	/* UnlhaGetMethod */
#define ISARC_GET_WRITE_TIME			67	/* UnlhaGetWriteTime */
#define ISARC_GET_CREATE_TIME			68	/* UnlhaGetCreateTime */
#define ISARC_GET_ACCESS_TIME			69	/* UnlhaGetAccessTime */
#define ISARC_GET_WRITE_TIME_EX			70	/* UnlhaGetWriteTimeEx */
#define ISARC_GET_CREATE_TIME_EX		71	/* UnlhaGetCreateTimeEx */
#define ISARC_GET_ACCESS_TIME_EX		72	/* UnlhaGetAccessTimeEx */
#define ISARC_SET_ENUM_MEMBERS_PROC		80	/* UnlhaSetEnumMembersProc */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC	81	/* UnlhaClearEnumMembersProc */

#endif	/* ISARC_FUNCTION_START */

/* 格納されているファイルの情報の取得 */
/* int WINAPI _export BgaGetArcFileInfo( LPSTR szFileName,
										LPMAININFO lpMainInfo ); */

int WINAPI _export BgaGetFileCount( LPCSTR pArcFile );

HARC WINAPI _export BgaOpenArchive( const HWND hWnd, LPCSTR pFileName,
									const DWORD dwMode );
int WINAPI _export BgaCloseArchive( HARC hArc );
int WINAPI _export BgaFindFirst( HARC hArc, LPCSTR pWildName,
								 LPINDIVIDUALINFO pSubInfo );
int WINAPI _export BgaFindNext( HARC hArc, LPINDIVIDUALINFO pSubInfo );

#if 0	/* 現在未実装です */
int WINAPI _export BgaExtract( HARC harc, LPCSTR szFileName,
							   LPCSTR szDirName, DWORD dwMode );
int WINAPI _export BgaAdd( HARC harc, LPSTR szFileName, DWORD dwMode );
int WINAPI _export BgaMove( HARC harc, LPSTR szFileName, DWORD dwMode );
int WINAPI _export BgaDelete( HARC harc, LPSTR szFileName, DWORD dwMode );
int WINAPI _export BgaGetArcFileName( HARC harc, LPSTR lpBuffer,
									  const int nSize );
DWORD WINAPI _export BgaGetArcFileSize( HARC harc );
#endif	/* 現在未実装です */

DWORD WINAPI _export BgaGetArcOriginalSize( HARC hArc );
DWORD WINAPI _export BgaGetArcCompressedSize( HARC hArc );
WORD WINAPI _export BgaGetArcRatio( HARC hArc );

#if 0	/* 現在未実装です */
WORD WINAPI _export BgaGetArcDate( HARC harc );
WORD WINAPI _export BgaGetArcTime( HARC harc );
UINT WINAPI _export BgaGetArcOSType( HARC harc );
int WINAPI _export BgaGetFileName( HARC harc, LPSTR lpBuffer,
								   const int nSize );
int WINAPI _export BgaGetMethod( HARC harc, LPSTR lpBuffer, const int nSize );
DWORD WINAPI _export BgaGetOriginalSize( HARC harc );
DWORD WINAPI _export BgaGetCompressedSize( HARC harc );
WORD WINAPI _export BgaGetRatio( HARC harc );
WORD WINAPI _export BgaGetDate( HARC harc );
WORD WINAPI _export BgaGetTime( HARC harc );

DWORD WINAPI _export BgaGetWriteTime( HARC harc );
DWORD WINAPI _export BgaGetAccessTime( HARC harc );
DWORD WINAPI _export BgaGetCreateTime( HARC harc );

DWORD WINAPI _export BgaGetCRC( HARC harc );
int WINAPI _export BgaGetAttribute( HARC harc );
UINT WINAPI _export BgaGetOSType( HARC harc );

int WINAPI _export BgaIsSFXFile( HARC harc );
#endif	/* 現在未実装です */

BOOL WINAPI _export BgaSetOwnerWindow( const HWND hWnd );
BOOL WINAPI _export BgaClearOwnerWindow();

#ifndef WM_ARCEXTRACT
#define WM_ARCEXTRACT	"wm_arcextract"

#define ARCEXTRACT_BEGIN		0	/* 該当ファイルの処理の開始 */
#define ARCEXTRACT_INPROCESS	1	/* 該当ファイルの展開中 */
#define ARCEXTRACT_END			2	/* 処理終了、関連メモリを開放 */
#define ARCEXTRACT_OPEN			3	/* 該当書庫の処理の開始 */
#define ARCEXTRACT_COPY			4	/* ワークファイルの書き戻し */

typedef BOOL CALLBACK ARCHIVERPROC( HWND hWnd, UINT uMsg, UINT uState,
									LPEXTRACTINGINFOEX pEis );
typedef ARCHIVERPROC *LPARCHIVERPROC ;

BOOL WINAPI _export BgaSetOwnerWindowEx( HWND hWnd,
										 LPARCHIVERPROC pArcProc );
BOOL WINAPI _export BgaKillOwnerWindowEx( HWND hWnd );
#endif	/* WM_ARCEXTRACT */


#if !defined(EXTRACT_FOUND_FILE)
/* MODE (for OpenArchive) */
#define M_INIT_FILE_USE			0x00000001L /* レジストリの設定を使用 */
#define M_REGARDLESS_INIT_FILE	0x00000002L /* 〃 を使用しない */
#define M_NO_BACKGROUND_MODE	0x00000004L /* バックグラウンドを禁止 */
#define M_NOT_USE_TIME_STAMP	0x00000008L
#define M_EXTRACT_REPLACE_FILE	0x00000010L
#define M_EXTRACT_NEW_FILE		0x00000020L
#define M_EXTRACT_UPDATE_FILE	0x00000040L
#define M_CHECK_ALL_PATH		0x00000100L /* 厳格なファイル名サーチ */
#define M_CHECK_FILENAME_ONLY	0x00000200L /* 〃を行わない */
#define M_CHECK_DISK_SIZE		0x00000400L
#define M_REGARDLESS_DISK_SIZE	0x00000800L
#define M_USE_DRIVE_LETTER		0x00001000L /* ドライブ名から格納 */
#define M_NOT_USE_DRIVE_LETTER	0x00002000L /* 〃 を格納しない */
#define M_INQUIRE_DIRECTORY		0x00004000L
#define M_NOT_INQUIRE_DIRECTORY 0x00008000L
#define M_INQUIRE_WRITE			0x00010000L
#define M_NOT_INQUIRE_WRITE		0x00020000L
#define M_CHECK_READONLY		0x00040000L
#define M_REGARDLESS_READONLY	0x00080000L
#define M_REGARD_E_COMMAND		0x00100000L
#define M_REGARD_X_COMMAND		0x00200000L
#define M_ERROR_MESSAGE_ON		0x00400000L /* エラーメッセージを表示 */
#define M_ERROR_MESSAGE_OFF		0x00800000L /* 〃を表示しない */
#define M_BAR_WINDOW_ON			0x01000000L
#define M_BAR_WINDOW_OFF		0x02000000L
#define M_CHECK_PATH			0x04000000L
#define M_RECOVERY_ON			0x08000000L /* 破損ヘッダの読み飛ばし */

#define M_MAKE_INDEX_FILE		0x10000000L
#define M_NOT_MAKE_INDEX_FILE	0x20000000L
#define EXTRACT_FOUND_FILE		0x40000000L /* 検索されたファイルを解凍 */
#define EXTRACT_NAMED_FILE		0x80000000L /* 指定したファイルを解凍 */
#endif /* EXTRACT_FOUND_FILE */


#ifndef FA_RDONLY
/* Attribute */
#define FA_RDONLY		0x01			/* Read only attribute */
#define FA_HIDDEN		0x02			/* Hidden file */
#define FA_SYSTEM		0x04			/* System file */
#define FA_LABEL		0x08			/* Volume label */
#define FA_DIREC		0x10			/* Directory */
#define FA_ARCH			0x20			/* Archive */
#endif


#if 0	/* 現在未実装です */
int WINAPI _export BgaExtractMem(const HWND _hwndParent,
		LPCSTR _szCmdLine, LPBYTE _lpBuffer, const DWORD _dwSize,
		time_t *_lpTime, LPWORD _lpwAttr, LPDWORD _lpdwWriteSize);
int WINAPI _export BgaCompressMem(const HWND _hwndParent,
		LPCSTR _szCmdLine, const LPBYTE _lpBuffer, const DWORD _dwSize,
		const time_t *_lpTime, const LPWORD _lpwAttr,
		LPDWORD _lpdwWriteSize);
#endif

#ifndef UNLHA_LIST_COMMAND
#define UNLHA_LIST_COMMAND		1
#define UNLHA_ADD_COMMAND		2
#define UNLHA_FRESH_COMMAND		3
#define UNLHA_DELETE_COMMAND	4
#define UNLHA_EXTRACT_COMMAND	5
#define UNLHA_PRINT_COMMAND		6
#define UNLHA_TEST_COMMAND		7
#define UNLHA_MAKESFX_COMMAND	8
#define UNLHA_JOINT_COMMAND		9
#define UNLHA_CONVERT_COMMAND	10
#define UNLHA_RENAME_COMMAND	11
#endif	/* UNLHA_LIST_COMMAND */

BOOL WINAPI _export BgaSetEnumMembersProc( UNLHA_WND_ENUMMEMBPROC pEnumProc );
BOOL WINAPI _export BgaClearEnumMembersProc();


#if !defined(ERROR_START)
#define ERROR_START				0x8000
/* WARNING */
#define ERROR_DISK_SPACE		0x8005
#define ERROR_READ_ONLY			0x8006
#define ERROR_USER_SKIP			0x8007
#define ERROR_UNKNOWN_TYPE		0x8008
#define ERROR_METHOD			0x8009
#define ERROR_PASSWORD_FILE		0x800A
#define ERROR_VERSION			0x800B
#define ERROR_FILE_CRC			0x800C
#define ERROR_FILE_OPEN			0x800D
#define ERROR_MORE_FRESH		0x800E
#define ERROR_NOT_EXIST			0x800F
#define ERROR_ALREADY_EXIST		0x8010

#define ERROR_TOO_MANY_FILES	0x8011

/* ERROR */
#define ERROR_MAKEDIRECTORY		0x8012
#define ERROR_CANNOT_WRITE		0x8013
#define ERROR_HUFFMAN_CODE		0x8014
#define ERROR_COMMENT_HEADER	0x8015
#define ERROR_HEADER_CRC		0x8016
#define ERROR_HEADER_BROKEN		0x8017
#define ERROR_ARC_FILE_OPEN		0x8018
#define ERROR_NOT_ARC_FILE		0x8019
#define ERROR_CANNOT_READ		0x801A
#define ERROR_FILE_STYLE		0x801B
#define ERROR_COMMAND_NAME		0x801C
#define ERROR_MORE_HEAP_MEMORY	0x801D
#define ERROR_ENOUGH_MEMORY		0x801E
#if !defined(ERROR_ALREADY_RUNNING)
#define ERROR_ALREADY_RUNNING	0x801F
#endif
#define ERROR_USER_CANCEL		0x8020
#define ERROR_HARC_ISNOT_OPENED 0x8021
#define ERROR_NOT_SEARCH_MODE	0x8022
#define ERROR_NOT_SUPPORT		0x8023
#define ERROR_TIME_STAMP		0x8024
#define ERROR_TMP_OPEN			0x8025
#define ERROR_LONG_FILE_NAME	0x8026
#define ERROR_ARC_READ_ONLY		0x8027
#define ERROR_SAME_NAME_FILE	0x8028
#define ERROR_NOT_FIND_ARC_FILE 0x8029
#define ERROR_RESPONSE_READ		0x802A
#define ERROR_NOT_FILENAME		0x802B
#define ERROR_TMP_COPY			0x802C
#define ERROR_EOF				0x802D
#define ERROR_ADD_TO_LARC		0x802E
#define ERROR_TMP_BACK_SPACE	0x802F
#define ERROR_SHARING			0x8030
#define ERROR_NOT_FIND_FILE		0x8031
#define ERROR_LOG_FILE			0x8032
#define ERROR_NO_DEVICE			0x8033
#define ERROR_GET_ATTRIBUTES	0x8034
#define ERROR_SET_ATTRIBUTES	0x8035
#define ERROR_GET_INFORMATION	0x8036
#define ERROR_GET_POINT			0x8037
#define ERROR_SET_POINT			0x8038
#define ERROR_CONVERT_TIME		0x8039
#define ERROR_GET_TIME			0x803a
#define ERROR_SET_TIME			0x803b
#define ERROR_CLOSE_FILE		0x803c
#define ERROR_HEAP_MEMORY		0x803d
#define ERROR_HANDLE			0x803e
#define ERROR_TIME_STAMP_RANGE	0x803f

#define ERROR_END	ERROR_TIME_STAMP_RANGE
#endif /* ERROR_START */

#ifdef __cplusplus
}
#endif
#endif	/* __BGA32API_H__ */
