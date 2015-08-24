/*
** UNARJ
** Copyright (c) 1990-93   by Robert K Jung.  All rights reserved.
**
** UNARJ32.H
** Windows DLL Version, Micco,  Nov.10,2007
** Copyright (c) 1996-2007 by Micco.  All rights reserved.
*/

#if !defined(UNARJ32_H)
#define UNARJ32_H

#if !defined(FNAME_MAX32)
#define	FNAME_MAX32		512
#endif
#if !defined(COMMENT_MAX)
#define	COMMENT_MAX	2048
#endif

#ifndef ARC_DECSTRACT
#define ARC_DECSTRACT

#if defined(__BORLANDC__)
#pragma option -a-
#else
#pragma pack(1)
#endif

#if !defined(__BORLANDC__) || __BORLANDC__ >= 0x550
typedef LONGLONG	ULHA_INT64;
#else
typedef struct {
	DWORD	LowPart;
	LONG	HighPart;
} ULHA_INT64, *LPULHA_INT64;
#endif

typedef	HGLOBAL	HARC;

typedef struct {
	DWORD 			dwOriginalSize;		/* ファイルのサイズ */
 	DWORD 			dwCompressedSize;	/* 圧縮後のサイズ */
	DWORD			dwCRC;				/* 格納ファイルのチェックサム */
	UINT			uFlag;				/* 処理結果 */
										/* Status flag */
	UINT			uOSType;			/* 書庫作成に使われた OS */
	WORD			wRatio;				/* 圧縮率 */
	WORD			wDate;				/* 格納ファイルの日付(DOS 形式) */
	WORD 			wTime;				/* 格納ファイルの時刻(〃) */
	char			szFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	char			dummy1[3];
	char			szAttribute[8];		/* 格納ファイルの属性(書庫固有) */
	char			szMode[8];			/* 格納ファイルの格納モード(〃) */
}	INDIVIDUALINFOA, *LPINDIVIDUALINFOA;
typedef struct {
	DWORD 			dwOriginalSize;		/* ファイルのサイズ */
 	DWORD 			dwCompressedSize;	/* 圧縮後のサイズ */
	DWORD			dwCRC;				/* 格納ファイルのチェックサム */
	UINT			uFlag;				/* 処理結果 */
										/* Status flag */
	UINT			uOSType;			/* 書庫作成に使われた OS */
	WORD			wRatio;				/* 圧縮率 */
	WORD			wDate;				/* 格納ファイルの日付(DOS 形式) */
	WORD 			wTime;				/* 格納ファイルの時刻(〃) */
	WCHAR			szFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	WCHAR			dummy1[3];
	WCHAR			szAttribute[8];		/* 格納ファイルの属性(書庫固有) */
	WCHAR			szMode[8];			/* 格納ファイルの格納モード(〃) */
}	INDIVIDUALINFOW, *LPINDIVIDUALINFOW;
#if !defined(UNICODE)
typedef INDIVIDUALINFOA   INDIVIDUALINFO;
typedef LPINDIVIDUALINFOA LPINDIVIDUALINFO;
#else
typedef INDIVIDUALINFOW   INDIVIDUALINFO;
typedef LPINDIVIDUALINFOW LPINDIVIDUALINFO;
#endif

typedef struct {
	DWORD 			dwFileSize;		/* 格納ファイルのサイズ */
	DWORD			dwWriteSize;	/* 書き込みサイズ */
	char			szSourceFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	char			dummy1[3];
	char			szDestFileName[FNAME_MAX32 + 1];
									/* 解凍先または圧縮元パス名 */
	char			dummy[3];
}	EXTRACTINGINFOA, *LPEXTRACTINGINFOA;
typedef struct {
	DWORD 			dwFileSize;		/* 格納ファイルのサイズ */
	DWORD			dwWriteSize;	/* 書き込みサイズ */
	WCHAR			szSourceFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	WCHAR			dummy1[3];
	WCHAR			szDestFileName[FNAME_MAX32 + 1];
									/* 解凍先または圧縮元パス名 */
	WCHAR			dummy[3];
}	EXTRACTINGINFOW, *LPEXTRACTINGINFOW;
#if !defined(UNICODE)
typedef EXTRACTINGINFOA   EXTRACTINGINFO;
typedef LPEXTRACTINGINFOA LPEXTRACTINGINFO;
#else
typedef EXTRACTINGINFOW   EXTRACTINGINFO;
typedef LPEXTRACTINGINFOW LPEXTRACTINGINFO;
#endif

typedef struct {
	EXTRACTINGINFOA		exinfo;
	DWORD				dwCompressedSize;
	DWORD				dwCRC;
	UINT				uOSType;
	WORD				wRatio;
	WORD				wDate;
	WORD				wTime;
	char				szAttribute[8];
	char				szMode[8];
} EXTRACTINGINFOEXA, *LPEXTRACTINGINFOEXA;
typedef struct {
	EXTRACTINGINFOW		exinfo;
	DWORD				dwCompressedSize;
	DWORD				dwCRC;
	UINT				uOSType;
	WORD				wRatio;
	WORD				wDate;
	WORD				wTime;
	WCHAR				szAttribute[8];
	WCHAR				szMode[8];
} EXTRACTINGINFOEXW, *LPEXTRACTINGINFOEXW;
#if !defined(UNICODE)
typedef EXTRACTINGINFOEXA   EXTRACTINGINFOEX;
typedef LPEXTRACTINGINFOEXA LPEXTRACTINGINFOEX;
#else
typedef EXTRACTINGINFOEXW   EXTRACTINGINFOEX;
typedef LPEXTRACTINGINFOEXW LPEXTRACTINGINFOEX;
#endif

typedef struct {
	DWORD				dwStructSize;
	EXTRACTINGINFOA		exinfo;
	DWORD				dwFileSize;			/* 格納ファイルのサイズ */
	DWORD				dwCompressedSize;
	DWORD				dwWriteSize;		/* 書き込みサイズ */
	DWORD				dwAttributes;
	DWORD 				dwCRC;
	UINT  				uOSType;
	WORD  				wRatio;
	FILETIME			ftCreateTime;
	FILETIME			ftAccessTime;
	FILETIME			ftWriteTime;
	char  				szMode[8];
	char				szSourceFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	char				dummy1[3];
	char				szDestFileName[FNAME_MAX32 + 1];
										/* 解凍先または圧縮元パス名 */
	char				dummy2[3];
} EXTRACTINGINFOEX32A, *LPEXTRACTINGINFOEX32A;
typedef struct {
	DWORD				dwStructSize;
	EXTRACTINGINFOW		exinfo;
	DWORD				dwFileSize;			/* 格納ファイルのサイズ */
	DWORD				dwCompressedSize;
	DWORD				dwWriteSize;		/* 書き込みサイズ */
	DWORD				dwAttributes;
	DWORD 				dwCRC;
	UINT  				uOSType;
	WORD  				wRatio;
	FILETIME			ftCreateTime;
	FILETIME			ftAccessTime;
	FILETIME			ftWriteTime;
	WCHAR  				szMode[8];
	WCHAR				szSourceFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	WCHAR				dummy1[3];
	WCHAR				szDestFileName[FNAME_MAX32 + 1];
										/* 解凍先または圧縮元パス名 */
	WCHAR				dummy2[3];
} EXTRACTINGINFOEX32W, *LPEXTRACTINGINFOEX32W;
#if !defined(UNICODE)
typedef EXTRACTINGINFOEX32A   EXTRACTINGINFOEX32;
typedef LPEXTRACTINGINFOEX32A LPEXTRACTINGINFOEX32;
#else
typedef EXTRACTINGINFOEX32W   EXTRACTINGINFOEX32;
typedef LPEXTRACTINGINFOEX32W LPEXTRACTINGINFOEX32;
#endif

typedef struct {
	DWORD				dwStructSize;
	EXTRACTINGINFOA		exinfo;
	ULHA_INT64			llFileSize;			/* 格納ファイルのサイズ */
	ULHA_INT64			llCompressedSize;
	ULHA_INT64			llWriteSize;		/* 書き込みサイズ */
	DWORD				dwAttributes;
	DWORD 				dwCRC;
	UINT  				uOSType;
	WORD  				wRatio;
	FILETIME			ftCreateTime;
	FILETIME			ftAccessTime;
	FILETIME			ftWriteTime;
	char  				szMode[8];
	char				szSourceFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	char				dummy1[3];
	char				szDestFileName[FNAME_MAX32 + 1];
										/* 解凍先または圧縮元パス名 */
	char				dummy2[3];
} EXTRACTINGINFOEX64A, *LPEXTRACTINGINFOEX64A;
typedef struct {
	DWORD				dwStructSize;
	EXTRACTINGINFOW		exinfo;
	ULHA_INT64			llFileSize;			/* 格納ファイルのサイズ */
	ULHA_INT64			llCompressedSize;
	ULHA_INT64			llWriteSize;		/* 書き込みサイズ */
	DWORD				dwAttributes;
	DWORD 				dwCRC;
	UINT  				uOSType;
	WORD  				wRatio;
	FILETIME			ftCreateTime;
	FILETIME			ftAccessTime;
	FILETIME			ftWriteTime;
	WCHAR  				szMode[8];
	WCHAR				szSourceFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	WCHAR				dummy1[3];
	WCHAR				szDestFileName[FNAME_MAX32 + 1];
										/* 解凍先または圧縮元パス名 */
	WCHAR				dummy2[3];
} EXTRACTINGINFOEX64W, *LPEXTRACTINGINFOEX64W;
#if !defined(UNICODE)
typedef EXTRACTINGINFOEX64A   EXTRACTINGINFOEX64;
typedef LPEXTRACTINGINFOEX64A LPEXTRACTINGINFOEX64;
#else
typedef EXTRACTINGINFOEX64W   EXTRACTINGINFOEX64;
typedef LPEXTRACTINGINFOEX64W LPEXTRACTINGINFOEX64;
#endif

typedef
BOOL
CALLBACK
ARCHIVERPROC(
		HWND _hwnd,
		UINT _uMsg,
		UINT _nState,
		LPVOID _lpEis
	);
typedef ARCHIVERPROC *LPARCHIVERPROC;

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
}	UNLHA_ENUM_MEMBER_INFOA, *LPUNLHA_ENUM_MEMBER_INFOA;
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
	WCHAR			szFileName[FNAME_MAX32 + 1];
	WCHAR			dummy1[3];
	WCHAR			szAddFileName[FNAME_MAX32 + 1];
	WCHAR			dummy2[3];
}	UNLHA_ENUM_MEMBER_INFOW, *LPUNLHA_ENUM_MEMBER_INFOW;
#if !defined(UNICODE)
typedef UNLHA_ENUM_MEMBER_INFOA   UNLHA_ENUM_MEMBER_INFO;
typedef LPUNLHA_ENUM_MEMBER_INFOA LPUNLHA_ENUM_MEMBER_INFO;
#else
typedef UNLHA_ENUM_MEMBER_INFOW   UNLHA_ENUM_MEMBER_INFO;
typedef LPUNLHA_ENUM_MEMBER_INFOW LPUNLHA_ENUM_MEMBER_INFO;
#endif

typedef struct {
	DWORD			dwStructSize;
	UINT			uCommand;
	ULHA_INT64		llOriginalSize;
	ULHA_INT64		llCompressedSize;
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
}	UNLHA_ENUM_MEMBER_INFO64A, *LPUNLHA_ENUM_MEMBER_INFO64A;
typedef struct {
	DWORD			dwStructSize;
	UINT			uCommand;
	ULHA_INT64		llOriginalSize;
	ULHA_INT64		llCompressedSize;
	DWORD			dwAttributes;
	DWORD			dwCRC;
	UINT			uOSType;
	WORD			wRatio;
	FILETIME		ftCreateTime;
	FILETIME		ftAccessTime;
	FILETIME		ftWriteTime;
	WCHAR			szFileName[FNAME_MAX32 + 1];
	WCHAR			dummy1[3];
	WCHAR			szAddFileName[FNAME_MAX32 + 1];
	WCHAR			dummy2[3];
}	UNLHA_ENUM_MEMBER_INFO64W, *LPUNLHA_ENUM_MEMBER_INFO64W;
#if !defined(UNICODE)
typedef UNLHA_ENUM_MEMBER_INFO64A   UNLHA_ENUM_MEMBER_INFO64;
typedef LPUNLHA_ENUM_MEMBER_INFO64A LPUNLHA_ENUM_MEMBER_INFO64;
#else
typedef UNLHA_ENUM_MEMBER_INFO64W   UNLHA_ENUM_MEMBER_INFO64;
typedef LPUNLHA_ENUM_MEMBER_INFO64W LPUNLHA_ENUM_MEMBER_INFO64;
#endif

typedef BOOL (CALLBACK *UNLHA_WND_ENUMMEMBPROC)(LPVOID);

#if !defined(__BORLANDC__)
#pragma pack()
#else
#pragma option -a.
#endif

#endif //ARC_DECSTRACT

#if !defined(__BORLANDC__)
#define	_export
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ### LHA.DLL Ver 1.1 と互換性のある API です。### */
WORD
WINAPI _export
UnarjGetVersion(
		VOID
	);

BOOL
WINAPI _export
UnarjGetRunning(
		VOID
	);

BOOL
WINAPI _export
UnarjGetBackGroundMode(
		VOID
	);
BOOL
WINAPI _export
UnarjSetBackGroundMode(
		const BOOL	_BackGroundMode
	);
BOOL
WINAPI _export
UnarjGetCursorMode(
		VOID
	);
BOOL
WINAPI _export
UnarjSetCursorMode(
		const BOOL	_CursorMode
	);
WORD
WINAPI _export
UnarjGetCursorInterval(
		VOID
	);
BOOL
WINAPI _export
UnarjSetCursorInterval(
		const WORD	_Interval
	);

int
WINAPI _export
UnarjA(
		const HWND	_hwnd,
		LPCSTR		_szCmdLine,
		LPSTR		_szOutput,
		const DWORD	_dwSize
	);
int
WINAPI _export
UnarjW(
		const HWND	_hwnd,
		LPCWSTR		_szCmdLine,
		LPWSTR		_szOutput,
		const DWORD	_dwSize
	);
#if !defined(UNICODE)
#define Unarj UnarjA
#else
#define Unarj UnarjW
#endif

/* ###『統合アーカイバ API』共通の API です。### */
#if !defined(CHECKARCHIVE_RAPID)
#define	CHECKARCHIVE_RAPID		0	/* 簡易(最初の３ヘッダまで) */
#define	CHECKARCHIVE_BASIC		1	/* 標準(全てのヘッダ) */
#define	CHECKARCHIVE_FULLCRC	2	/* 完全(格納ファイルの CRC チェック) */

/* 以下のフラグは上記と組み合わせて使用。*/
#define CHECKARCHIVE_RECOVERY	4   /* 破損ヘッダを読み飛ばして処理 */
#define CHECKARCHIVE_SFX		8	/* SFX かどうかを返す */
#define CHECKARCHIVE_ALL		16	/* ファイルの最後まで検索する */
#define CHECKARCHIVE_ENDDATA	32	/* 書庫より後ろの余剰データを許可 */
#endif

BOOL
WINAPI _export
UnarjCheckArchiveA(
		LPCSTR		_szFileName,
		const int	_iMode
	);
BOOL
WINAPI _export
UnarjCheckArchiveW(
		LPCWSTR		_szFileName,
		const int	_iMode
	);
#if !defined(UNICODE)
#define UnarjCheckArchive UnarjCheckArchiveA
#else
#define UnarjCheckArchive UnarjCheckArchiveW
#endif

int
WINAPI _export
UnarjGetFileCountA(
		LPCSTR	_szArcFile
	);
int
WINAPI _export
UnarjGetFileCountW(
		LPCWSTR	_szArcFile
	);
#if !defined(UNICODE)
#define UnarjGetFileCount UnarjGetFileCountA
#else
#define UnarjGetFileCount UnarjGetFileCountW
#endif

#if !defined(UNPACK_CONFIG_MODE)
#define	UNPACK_CONFIG_MODE		1	/* 解凍系の設定 */
#define PACK_CONFIG_MODE		2	/* 圧縮系の設定 */
#endif

BOOL
WINAPI _export
UnarjConfigDialogA(
		const HWND	_hwnd,
		LPSTR		_lpszComBuffer,
		const int	_iMode
	);
BOOL
WINAPI _export
UnarjConfigDialogW(
		const HWND	_hwnd,
		LPWSTR		_lpszComBuffer,
		const int	_iMode
	);
#if !defined(UNICODE)
#define UnarjConfigDialog UnarjConfigDialogA
#else
#define UnarjConfigDialog UnarjConfigDialogW
#endif

#if !defined(ISARC_FUNCTION_START)
#define ISARC_FUNCTION_START			0
#define ISARC							0	/* Unarj */
#define ISARC_GET_VERSION				1	/* UnarjGetVersion */
#define ISARC_GET_CURSOR_INTERVAL		2	/* UnarjGetCursorInterval */
#define ISARC_SET_CURSOR_INTERVAL		3	/* UnarjSetCursorInterval */
#define ISARC_GET_BACK_GROUND_MODE		4	/* UnarjGetBackGroundMode */
#define ISARC_SET_BACK_GROUND_MODE		5	/* UnarjSetBackGroundMode */
#define ISARC_GET_CURSOR_MODE			6	/* UnarjGetCursorMode */
#define ISARC_SET_CURSOR_MODE			7	/* UnarjSetCursorMode */
#define ISARC_GET_RUNNING				8	/* UnarjGetRunning */

#define ISARC_CHECK_ARCHIVE				16	/* UnarjCheckArchive */
#define ISARC_CONFIG_DIALOG				17	/* UnarjConfigDialog */
#define ISARC_GET_FILE_COUNT			18	/* UnarjGetFileCount */
#define ISARC_QUERY_FUNCTION_LIST		19	/* UnarjQueryFunctionList */
#define ISARC_HOUT						20	/* (UnarjHOut) */
#define ISARC_STRUCTOUT					21	/* (UnarjStructOut) */
#define ISARC_GET_ARC_FILE_INFO			22	/* UnarjGetArcFileInfo */

#define ISARC_OPEN_ARCHIVE				23	/* UnarjOpenArchive */
#define ISARC_CLOSE_ARCHIVE				24	/* UnarjCloseArchive */
#define ISARC_FIND_FIRST				25	/* UnarjFindFirst */
#define ISARC_FIND_NEXT					26	/* UnarjFindNext */
#define ISARC_EXTRACT					27	/* (UnarjExtract) */
#define ISARC_ADD						28	/* (UnarjAdd) */
#define ISARC_MOVE						29	/* (UnarjMove) */
#define ISARC_DELETE					30	/* (UnarjDelete) */
#define ISARC_SETOWNERWINDOW			31	/* UnarjSetOwnerWindow */
#define ISARC_CLEAROWNERWINDOW			32	/* UnarjClearOwnerWindow */
#define ISARC_SETOWNERWINDOWEX			33	/* UnarjSetOwnerWindowEx */
#define ISARC_KILLOWNERWINDOWEX			34	/* UnarjKillOwnerWindowEx */

#define ISARC_GET_ARC_FILE_NAME			40	/* UnarjGetArcFileName */
#define ISARC_GET_ARC_FILE_SIZE			41	/* UnarjGetArcFileSize */
#define ISARC_GET_ARC_ORIGINAL_SIZE		42	/* UnarjArcOriginalSize */
#define ISARC_GET_ARC_COMPRESSED_SIZE	43	/* UnarjGetArcCompressedSize */
#define ISARC_GET_ARC_RATIO				44	/* UnarjGetArcRatio */
#define ISARC_GET_ARC_DATE				45	/* UnarjGetArcDate */
#define ISARC_GET_ARC_TIME				46	/* UnarjGetArcTime */
#define ISARC_GET_ARC_OS_TYPE			47	/* UnarjGetArcOSType */
#define ISARC_GET_ARC_IS_SFX_FILE		48	/* UnarjGetArcIsSFXFile */
#define ISARC_GET_ARC_WRITE_TIME_EX		49	/* UnarjGetArcWriteTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX	50	/* UnarjGetArcCreateTimeEx */
#define	ISARC_GET_ARC_ACCESS_TIME_EX	51	/* UnarjGetArcAccessTimeEx */
#define	ISARC_GET_ARC_CREATE_TIME_EX2	52	/* UnarjGetArcCreateTimeEx2 */
#define ISARC_GET_ARC_WRITE_TIME_EX2	53	/* UnarjGetArcWriteTimeEx2 */
#define ISARC_GET_FILE_NAME				57	/* UnarjGetFileName */
#define ISARC_GET_ORIGINAL_SIZE			58	/* UnarjGetOriginalSize */
#define ISARC_GET_COMPRESSED_SIZE		59	/* UnarjGetCompressedSize */
#define ISARC_GET_RATIO					60	/* UnarjGetRatio */
#define ISARC_GET_DATE					61	/* UnarjGetDate */
#define ISARC_GET_TIME					62	/* UnarjGetTime */
#define ISARC_GET_CRC					63	/* UnarjGetCRC */
#define ISARC_GET_ATTRIBUTE				64	/* UnarjGetAttribute */
#define ISARC_GET_OS_TYPE				65	/* UnarjGetOSType */
#define ISARC_GET_METHOD				66	/* UnarjGetMethod */
#define ISARC_GET_WRITE_TIME			67	/* UnarjGetWriteTime */
#define ISARC_GET_CREATE_TIME			68	/* UnarjGetCreateTime */
#define ISARC_GET_ACCESS_TIME			69	/* UnarjGetAccessTime */
#define ISARC_GET_WRITE_TIME_EX			70	/* UnarjGetWriteTimeEx */
#define ISARC_GET_CREATE_TIME_EX		71	/* UnarjGetCreateTimeEx */
#define ISARC_GET_ACCESS_TIME_EX		72	/* UnarjGetAccessTimeEx */
#define ISARC_SET_ENUM_MEMBERS_PROC		80  /* UnarjSetEnumMembersProc */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC	81	/* UnarjClearEnumMembersProc */
#define ISARC_GET_ARC_FILE_SIZE_EX		82	/* UnarjGetArcFileSizeEx */
#define ISARC_GET_ARC_ORIGINAL_SIZE_EX	83	/* UnarjArcOriginalSizeEx */
#define ISARC_GET_ARC_COMPRESSED_SIZE_EX	84	/* UnarjGetArcCompressedSizeEx */
#define ISARC_GET_ORIGINAL_SIZE_EX		85	/* UnarjGetOriginalSizeEx */
#define ISARC_GET_COMPRESSED_SIZE_EX	86	/* UnarjGetCompressedSizeEx */
#define ISARC_SETOWNERWINDOWEX64		87	/* UnarjSetOwnerWindowEx64 */
#define ISARC_KILLOWNERWINDOWEX64		88	/* UnarjKillOwnerWindowEx64 */
#define ISARC_SET_ENUM_MEMBERS_PROC64	89  /* UnarjSetEnumMembersProc64 */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC64	90	/* UnarjClearEnumMembersProc64 */
#define ISARC_OPEN_ARCHIVE2				91	/* UnarjOpenArchive2 */
#define ISARC_GET_ARC_READ_SIZE			92	/* UnarjGetArcReadSize */
#define ISARC_GET_ARC_READ_SIZE_EX		93	/* UnarjGetArcReadSizeEx*/
#define SET_LANGUE_JAPANESE				94	/* SetLangueJapanese */
#define SET_LANGUE_ENGLISH				95	/* SetLangueEnglish */
#define SET_LANGUE_SPECIFIED			96	/* SetLangueSpecified */
#define ISARC_SET_LANGUE_SPECIFIED		97	/* UnarjSetLangueSpecified */
#define ISARC_SET_LANGUE_JAPANESE		98	/* UnarjSetLangueJapanese */
#define ISARC_SET_LANGUE_ENGLISH		99	/* UnarjSetLangueEnglish */
#define ISARC_SET_PRIORITY				100	/* UnarjSetPriority */
#define ISARC_GET_ATTRIBUTES			101 /* UnarjGetAttributes */
#define ISARC_SET_CP                    102 /* UnarjSetCP */
#define ISARC_GET_CP                    103 /* UnarjGetCP */
#define ISARC_GET_LAST_ERROR            104 /* UnarjGetLastError */
#define ISARC_GET_ARC_WRITE_TIME		105	/* UnarjGetArcWriteTime */
#define ISARC_GET_ARC_CREATE_TIME		106	/* UnarjGetArcCreateTime */
#define ISARC_GET_ARC_ACCESS_TIME		107	/* UnarjGetArcAccessTime */
#define ISARC_GET_ARC_WRITE_TIME_64		108	/* UnarjGetArcWriteTime64 */
#define ISARC_GET_ARC_CREATE_TIME_64	109 /* UnarjGetArcCreateTime64 */
#define ISARC_GET_ARC_ACCESS_TIME_64	110	/* UnarjGetArcAccessTime64 */
#define ISARC_GET_WRITE_TIME_64			111 /* UnarjGetWriteTime64 */
#define ISARC_GET_CREATE_TIME_64		112 /* UnarjGetCreateTime64 */
#define ISARC_GET_ACCESS_TIME_64		113 /* UnarjGetAccessTime64 */
#define ISARC_SET_UNICODE_MODE			114 /* UnarjSetUnicodeMode */
#define ISARC_FUNCTION_END				ISARC_SET_UNICODE_MODE

#endif	/* ISARC_FUNCTION_START */

BOOL
WINAPI _export
UnarjQueryFunctionList(
		const int	_iFunction
	);

#ifndef WM_ARCEXTRACT
#define	WM_ARCEXTRACT	"wm_arcextract"

#define	ARCEXTRACT_BEGIN		0	/* 該当ファイルの処理の開始 */
#define	ARCEXTRACT_INPROCESS	1	/* 該当ファイルの展開中 */
#define	ARCEXTRACT_END			2	/* 処理終了、関連メモリを開放 */
#define ARCEXTRACT_OPEN			3	/* 該当書庫の処理の開始 */
#define ARCEXTRACT_COPY			4	/* ワークファイルの書き戻し */

BOOL
WINAPI _export
UnarjSetOwnerWindowA(
		const HWND	_hwnd
	);
BOOL
WINAPI _export
UnarjSetOwnerWindowW(
		const HWND	_hwnd
	);
#if !defined(UNICODE)
#define UnarjSetOwnerWindow UnarjSetOwnerWindowA
#else
#define UnarjSetOwnerWindow UnarjSetOwnerWindowW
#endif
BOOL
WINAPI _export
UnarjClearOwnerWindow(
		VOID
	);

BOOL
WINAPI _export
UnarjSetOwnerWindowExA(
		const HWND		_hwnd,
		LPARCHIVERPROC	_lpArcProc
	);
BOOL
WINAPI _export
UnarjSetOwnerWindowExW(
		const HWND		_hwnd,
		LPARCHIVERPROC	_lpArcProc
	);
#if !defined(UNICODE)
#define UnarjSetOwnerWindowEx UnarjSetOwnerWindowExA
#else
#define UnarjSetOwnerWindowEx UnarjSetOwnerWindowExW
#endif
BOOL
WINAPI _export
UnarjKillOwnerWindowEx(
		const HWND	_hwnd
	);
BOOL
WINAPI _export
UnarjSetOwnerWindowEx64(
		const HWND		_hwnd,
		LPARCHIVERPROC	_lpArcProc,
		DWORD			_dwStructSize
	);
BOOL
WINAPI _export
UnarjKillOwnerWindowEx64(
		const HWND	_hwnd
	);
#endif

/* OpenArchive 系 API です。 */

#if !defined(EXTRACT_FOUND_FILE)
/* MODE (for UnarjOpenArchive) */
#define M_INIT_FILE_USE			0x00000001L	/* レジストリの設定を使用 */
#define M_REGARDLESS_INIT_FILE	0x00000002L	/* 〃 を使用しない */
#define M_NO_BACKGROUND_MODE	0x00000004L	/* バックグラウンドを禁止 */
#define M_NOT_USE_TIME_STAMP	0x00000008L
#define M_EXTRACT_REPLACE_FILE	0x00000010L
#define M_EXTRACT_NEW_FILE		0x00000020L
#define M_EXTRACT_UPDATE_FILE	0x00000040L
#define M_CHECK_ALL_PATH		0x00000100L	/* 厳格なファイル名サーチ */
#define M_CHECK_FILENAME_ONLY	0x00000200L	/* 〃を行わない */
#define M_CHECK_DISK_SIZE		0x00000400L
#define M_REGARDLESS_DISK_SIZE	0x00000800L
#define M_USE_DRIVE_LETTER		0x00001000L	/* ドライブ名から格納 */
#define M_NOT_USE_DRIVE_LETTER	0x00002000L	/* 〃 を格納しない */
#define M_INQUIRE_DIRECTORY		0x00004000L
#define M_NOT_INQUIRE_DIRECTORY 0x00008000L
#define M_INQUIRE_WRITE			0x00010000L
#define M_NOT_INQUIRE_WRITE		0x00020000L
#define M_CHECK_READONLY		0x00040000L
#define M_REGARDLESS_READONLY	0x00080000L
#define M_REGARD_E_COMMAND		0x00100000L
#define M_REGARD_X_COMMAND		0x00200000L
#define M_ERROR_MESSAGE_ON		0x00400000L	/* エラーメッセージを表示 */
#define M_ERROR_MESSAGE_OFF		0x00800000L	/* 〃を表示しない */
#define M_BAR_WINDOW_ON			0x01000000L
#define M_BAR_WINDOW_OFF		0x02000000L
#define M_CHECK_PATH			0x04000000L
#define M_RECOVERY_ON			0x08000000L /* 破損ヘッダの読み飛ばし */

#define M_MAKE_INDEX_FILE		0x10000000L
#define M_NOT_MAKE_INDEX_FILE	0x20000000L
#define EXTRACT_FOUND_FILE		0x40000000L	/* 検索されたファイルを解凍 */
#define EXTRACT_NAMED_FILE		0x80000000L	/* 指定したファイルを解凍 */
#endif /* EXTRACT_FOUND_FILE */

HARC
WINAPI _export
UnarjOpenArchiveA(
		const HWND	_hwnd,
		LPCSTR		_szFileName,
		const DWORD	_dwMode
	);
HARC
WINAPI _export
UnarjOpenArchiveW(
		const HWND	_hwnd,
		LPCWSTR		_szFileName,
		const DWORD	_dwMode
	);
#if !defined(UNICODE)
#define UnarjOpenArchive UnarjOpenArchiveA
#else
#define UnarjOpenArchive UnarjOpenArchiveW
#endif
HARC
WINAPI _export
UnarjOpenArchive2A(
		const HWND	_hwnd,
		LPCSTR		_szFileName,
		const DWORD	_dwMode,
		LPCSTR		_szOption
	);
HARC
WINAPI _export
UnarjOpenArchive2W(
		const HWND	_hwnd,
		LPCWSTR		_szFileName,
		const DWORD	_dwMode,
		LPCWSTR		_szOption
	);
#if !defined(UNICODE)
#define UnarjOpenArchive2 UnarjOpenArchive2A
#else
#define UnarjOpenArchive2 UnarjOpenArchive2W
#endif
int
WINAPI _export
UnarjCloseArchive(
		HARC	_harc
	);
int
WINAPI _export
UnarjFindFirstA(
		HARC			_harc,
		LPCSTR			_szWildName,
		INDIVIDUALINFOA	*_lpSubInfo
	);
int
WINAPI _export
UnarjFindFirstW(
		HARC			_harc,
		LPCWSTR			_szWildName,
		INDIVIDUALINFOW	*_lpSubInfo
	);
#if !defined(UNICODE)
#define UnarjFindFirst UnarjFindFirstA
#else
#define UnarjFindFirst UnarjFindFirstW
#endif
int
WINAPI _export
UnarjFindNextA(
		HARC			_harc,
		INDIVIDUALINFOA	*_lpSubInfo
	);
int
WINAPI _export
UnarjFindNextW(
		HARC			_harc,
		INDIVIDUALINFOW	*_lpSubInfo
	);
#if !defined(UNICODE)
#define UnarjFindNext UnarjFindNextA
#else
#define UnarjFindNext UnarjFindNextW
#endif
int
WINAPI _export
UnarjGetArcFileNameA(
		HARC		_harc,
		LPSTR		_lpBuffer,
		const int	_nSize
	);
int
WINAPI _export
UnarjGetArcFileNameW(
		HARC		_harc,
		LPWSTR		_lpBuffer,
		const int	_nSize
	);
#if !defined(UNICODE)
#define UnarjGetArcFileName UnarjGetArcFileNameA
#else
#define UnarjGetArcFileName UnarjGetArcFileNameW
#endif
DWORD
WINAPI _export
UnarjGetArcFileSize(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetArcOriginalSize(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetArcCompressedSize(
		HARC	_harc
	);
BOOL
WINAPI _export
UnarjGetArcFileSizeEx(
		HARC		_harc,
		ULHA_INT64	*_lpllSize
	);
BOOL
WINAPI _export
UnarjGetArcOriginalSizeEx(
		HARC		_harc,
		ULHA_INT64	*_lpllSize
	);
BOOL
WINAPI _export
UnarjGetArcCompressedSizeEx(
		HARC		_harc,
		ULHA_INT64	*_lpllSize
	);
DWORD
WINAPI _export
UnarjGetArcReadSize(
		HARC	_harc
	);
BOOL
WINAPI _export
UnarjGetArcReadSizeEx(
		HARC		_harc,
		ULHA_INT64	*_lpllSize
	);
WORD
WINAPI _export
UnarjGetArcRatio(
		HARC	_harc
	);
WORD
WINAPI _export
UnarjGetArcDate(
		HARC	_harc
	);
WORD
WINAPI _export
UnarjGetArcTime(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetArcWriteTime(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetArcCreateTime(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetArcAccessTime(
		HARC	_harc
	);
BOOL
WINAPI _export
UnarjGetArcWriteTimeEx(
		HARC		_harc,
		FILETIME	*_lpftLastWriteTime
	);
BOOL
WINAPI _export
UnarjGetArcCreateTimeEx(
		HARC		_harc,
		FILETIME	*_lpftCreationTime
	);
BOOL
WINAPI _export
UnarjGetArcAccessTimeEx(
		HARC		_harc,
		FILETIME	*_lpftLastAccessTime
	);
BOOL
WINAPI _export
UnarjGetArcWriteTime64(
		HARC		_harc,
		ULHA_INT64	*_lpllLastWriteTime
	);
BOOL
WINAPI _export
UnarjGetArcCreateTime64(
		HARC		_harc,
		ULHA_INT64	*_lpllCreationTime
	);
BOOL
WINAPI _export
UnarjGetArcAccessTime64(
		HARC		_harc,
		ULHA_INT64	*_lpllLastAccessTime
	);
BOOL
WINAPI _export
UnarjGetArcCreateTimeEx2(
		HARC		_harc,
		FILETIME	*_lpftCreationTime
	);
BOOL
WINAPI _export
UnarjGetArcWriteTimeEx2(
		HARC		_harc,
		FILETIME	*_lpftLastWriteTime
	);
UINT
WINAPI _export
UnarjGetArcOSType(
		HARC	_harc
	);
int
WINAPI _export
UnarjIsSFXFile(
		HARC	_harc
	);

int
WINAPI _export
UnarjGetFileNameA(
		HARC		_harc,
		LPSTR		_lpBuffer,
		const int	_nSize
	);
int
WINAPI _export
UnarjGetFileNameW(
		HARC		_harc,
		LPWSTR		_lpBuffer,
		const int	_nSize
	);
#if !defined(UNICODE)
#define UnarjGetFileName UnarjGetFileNameA
#else
#define UnarjGetFileName UnarjGetFileNameW
#endif
int
WINAPI _export
UnarjGetMethodA(
		HARC		_harc,
		LPSTR		_lpBuffer,
		const int	_nSize
	);
int
WINAPI _export
UnarjGetMethodW(
		HARC		_harc,
		LPWSTR		_lpBuffer,
		const int	_nSize
	);
#if !defined(UNICODE)
#define UnarjGetMethod UnarjGetMethodA
#else
#define UnarjGetMethod UnarjGetMethodW
#endif
DWORD
WINAPI _export
UnarjGetOriginalSize(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetCompressedSize(
		HARC	_harc
	);
BOOL
WINAPI _export
UnarjGetOriginalSizeEx(
		HARC		_harc,
		ULHA_INT64	*_lpllSize
	);
BOOL
WINAPI _export
UnarjGetCompressedSizeEx(
		HARC		_harc,
		ULHA_INT64	*_lpllSize
	);
WORD
WINAPI _export
UnarjGetRatio(
		HARC	_harc
	);
WORD
WINAPI _export
UnarjGetDate(
		HARC	_harc
	);
WORD
WINAPI _export
UnarjGetTime(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetWriteTime(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetAccessTime(
		HARC	_harc
	);
DWORD
WINAPI _export
UnarjGetCreateTime(
		HARC	_harc
	);
BOOL
WINAPI _export
UnarjGetWriteTimeEx(
		HARC		_harc,
		FILETIME	*_lpftLastWriteTime
	);
BOOL
WINAPI _export
UnarjGetCreateTimeEx(
		HARC		_harc,
		FILETIME	*_lpftCreationTime
	);
BOOL
WINAPI _export
UnarjGetAccessTimeEx(
		HARC		_harc,
		FILETIME	*_lpftLastAccessTime
	);
BOOL
WINAPI _export
UnarjGetWriteTime64(
		HARC		_harc,
		ULHA_INT64	*_lpllLastWriteTime
	);
BOOL
WINAPI _export
UnarjGetCreateTime64(
		HARC		_harc,
		ULHA_INT64	*_lpllCreationTime
	);
BOOL
WINAPI _export
UnarjGetAccessTime64(
		HARC		_harc,
		ULHA_INT64	*_lpllLastAccessTime
	);
DWORD
WINAPI _export
UnarjGetCRC(
		HARC	_harc
	);
int
WINAPI _export
UnarjGetAttribute(
		HARC	_harc
	);
int
WINAPI _export
UnarjGetAttributes(
		HARC	_harc
	);
UINT
WINAPI _export
UnarjGetOSType(
		HARC	_harc
	);

#ifndef FA_RDONLY
/* Attribute */
#define FA_RDONLY       0x01            /* 書き込み保護属性 */
#define FA_HIDDEN       0x02            /* 隠し属性 */
#define FA_SYSTEM       0x04            /* システム属性 */
#define FA_LABEL        0x08            /* ボリューム・ラベル */
#define FA_DIREC        0x10            /* ディレクトリ */
#define FA_ARCH         0x20            /* アーカイブ属性 */
#define FA_COMPRESSED	0x0800			/* 圧縮属性 */
#define FA_ENCRYPTED	0x4000			/* 暗号化属性 */
#endif

#if !defined(BPL_NORMAL)
#define BPL_NORMAL					0
#define BPL_DENY_TOO_MANY_PARENTS	1
#define BPL_DENY_ABS_PATH			2
#define BPL_DENY_PARENTS			4
#define BPL_DENY_EXECUTABLE			8
#define BPL_DENY_CTRLCHARS			16
#define BPL_DENY_LEVEL_0			0
#define BPL_DENY_LEVEL_1			1
#define BPL_DENY_LEVEL_2			3
#define BPL_DENY_LEVEL_3			7
#endif

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
#define ERROR_HARC_ISNOT_OPENED	0x8021
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
#define ERROR_MAKE_ARCHIVE		0x8040
#define ERROR_NOT_CONFIRM_NAME	0x8041
#define ERROR_UNEXPECTED_EOF	0x8042
#define ERROR_INVALID_END_MARK	0x8043
#define ERROR_INVOLVED_LZH		0x8044
#define ERROR_NO_END_MARK		0x8045
#define ERROR_HDR_INVALID_SIZE	0x8046
#define ERROR_UNKNOWN_LEVEL		0x8047
#define ERROR_BROKEN_DATA		0x8048
#define ERROR_INVALID_PATH		0x8049
#define ERROR_TOO_BIG			0x804A
#define ERROR_EXECUTABLE_FILE	0x804B
#define ERROR_INVALID_VALUE		0x804C
#define ERROR_HDR_EXPLOIT		0x804D

#define ERROR_END	ERROR_HDR_EXPLOIT
#endif /* ERROR_START */

/* ### UNARJ32.DLL 独自の API です。### */
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

WORD
WINAPI _export
UnarjGetSubVersion(
		VOID
	);

int
WINAPI _export
UnarjExtractMemA(
		const HWND		_hwndParent,
		LPCSTR			_szCmdLine,
		LPBYTE			_lpBuffer,
		const DWORD		_dwSize,
		time_t			*_lpTime,
		LPWORD			_lpwAttr,
		LPDWORD			_lpdwWriteSize
	);
int
WINAPI _export
UnarjExtractMemW(
		const HWND		_hwndParent,
		LPCWSTR			_szCmdLine,
		LPBYTE			_lpBuffer,
		const DWORD		_dwSize,
		time_t			*_lpTime,
		LPWORD			_lpwAttr,
		LPDWORD			_lpdwWriteSize
	);
#if !defined(UNICODE)
#define UnarjExtractMem UnarjExtractMemA
#else
#define UnarjExtractMem UnarjExtractMemW
#endif
int
WINAPI _export
UnarjCompressMemA(
		const HWND		_hwndParent,
		LPCSTR			_szCmdLine,
		const LPBYTE	_lpBuffer,
		const DWORD		_dwSize,
		const time_t	*_lpTime,
		const LPWORD	_lpwAttr,
		LPDWORD			_lpdwWriteSize
	);
int
WINAPI _export
UnarjCompressMemW(
		const HWND		_hwndParent,
		LPCWSTR			_szCmdLine,
		const LPBYTE	_lpBuffer,
		const DWORD		_dwSize,
		const time_t	*_lpTime,
		const LPWORD	_lpwAttr,
		LPDWORD			_lpdwWriteSize
	);
#if !defined(UNICODE)
#define UnarjCompressMem UnarjCompressMemA
#else
#define UnarjCompressMem UnarjCompressMemW
#endif

BOOL
WINAPI _export
UnarjSetEnumMembersProcA(
		UNLHA_WND_ENUMMEMBPROC	_lpEnumProc
	);
BOOL
WINAPI _export
UnarjSetEnumMembersProcW(
		UNLHA_WND_ENUMMEMBPROC	_lpEnumProc
	);
#if !defined(UNICODE)
#define UnarjSetEnumMembersProc UnarjSetEnumMembersProcA
#else
#define UnarjSetEnumMembersProc UnarjSetEnumMembersProcW
#endif
BOOL
WINAPI _export
UnarjClearEnumMembersProc(
		VOID
	);
BOOL
WINAPI _export
UnarjSetEnumMembersProc64(
		UNLHA_WND_ENUMMEMBPROC	_lpEnumProc,
		DWORD					_dwStructSize
	);
BOOL
WINAPI _export
UnarjClearEnumMembersProc64(
		VOID
	);

BOOL
WINAPI _export
SetLangueSpecified(
		LANGID	_wLanguage
	);
BOOL
WINAPI _export
SetLangueJapanese(
		VOID
	);
BOOL
WINAPI _export
SetLangueEnglish(
		VOID
	);
BOOL
WINAPI _export
UnarjSetLangueSpecified(
		LANGID	_wLanguage
	);
BOOL
WINAPI _export
UnarjSetLangueJapanese(
		VOID
	);
BOOL
WINAPI _export
UnarjSetLangueEnglish(
		VOID
	);

BOOL
WINAPI _export
UnarjSetCP(
		UINT	_uCodePage
	);
UINT
WINAPI _export
UnarjGetCP(
		VOID
	);

int
WINAPI _export
UnarjGetLastError(
		LPDWORD  _lpdwSystemError
	);

#ifdef __cplusplus
}
#endif

#endif	/* UNARJ32_H */
