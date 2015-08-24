/* =====================================================================
	Jack32.exe -> FPJackSe.dll -> Arc32.dll -> Jack32.dll

	JackApi.h
	Copyright(c) 1995-2001 by Toshinobu Kimura All rights reserved.
===================================================================== */

#ifndef __JACK_API_H__
#define __JACK_API_H__

#define JACK32_DLL_VERSION	   20			/* version 0.20 */


#ifndef FNAME_MAX32
#define FNAME_MAX32		512
#endif

#ifndef ARC_DECSTRACT
#define ARC_DECSTRACT

#if defined(__BORLANDC__)
#pragma option -a-
#else
#pragma pack(1)
#endif

typedef HGLOBAL HARC;

typedef struct {
	DWORD	dwOriginalSize;					/* 復元時のﾌｧｲﾙｻｲｽﾞ */
	DWORD	dwCompressedSize;				/* 切り裂きﾌｧｲﾙのｻｲｽﾞ */
	DWORD	dwCRC;							/* 未使用 */
	UINT	uFlag;							/* 未使用 */
	UINT	uOSType;						/* 未使用 */
	WORD	wRatio;							/* 未使用 */
	WORD	wDate;							/* 復元時のﾌｧｲﾙの日付(DOS 形式) */
	WORD	wTime;							/* 復元時のﾌｧｲﾙの時刻(〃) */
	char	szFileName[FNAME_MAX32 + 1];	/* 復元時のﾌｧｲﾙ名 */
	char	dummy1[3];
	char	szAttribute[8];					/* 未使用 */
	char	szMode[8];						/* 未使用 */
} INDIVIDUALINFO, FAR *LPINDIVIDUALINFO;


#if 0	/* 現在未使用 */
typedef struct {
	DWORD			dwFileSize;		/* 格納ファイルのサイズ */
	DWORD			dwWriteSize;	/* 書き込みサイズ */
	char			szSourceFileName[FNAME_MAX32 + 1];	/* 格納ファイル名 */
	char			dummy1[3];
	char			szDestFileName[FNAME_MAX32 + 1];
									/* 解凍先または圧縮元パス名 */
	char			dummy[3];
}	EXTRACTINGINFO, *LPEXTRACTINGINFO;

typedef struct {
	EXTRACTINGINFO exinfo;
	DWORD dwCompressedSize;
	DWORD dwCRC;
	UINT  uOSType;
	WORD  wRatio;
	WORD  wDate;
	WORD  wTime;
	char  szAttribute[8];
	char  szMode[8];
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
#endif	/* 現在未使用 */


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

/* ### LHA.DLL Ver 1.1 と互換性のある API です。### */
int WINAPI _export Jack(const HWND _hwnd, LPCSTR _szCmdLine,
						LPSTR _szOutput, const DWORD _dwSize);
WORD WINAPI _export JackGetVersion(VOID);
BOOL WINAPI _export JackGetRunning(VOID);

#if 0	/* 現在未実装 */
BOOL WINAPI _export JackGetBackGroundMode(VOID);
BOOL WINAPI _export JackSetBackGroundMode(const BOOL _BackGroundMode);
BOOL WINAPI _export JackGetCursorMode(VOID);
BOOL WINAPI _export JackSetCursorMode(const BOOL _CursorMode);
WORD WINAPI _export JackGetCursorInterval(VOID);
BOOL WINAPI _export JackSetCursorInterval(const WORD _Interval);
#endif	/* 現在未実装 */

/* ###『統合アーカイバ API』共通の API です。### */
#if !defined(CHECKARCHIVE_RAPID)
#define CHECKARCHIVE_RAPID		0	/* 簡易(最初の３ヘッダまで) */
#define CHECKARCHIVE_BASIC		1	/* 標準(全てのヘッダ) */
#define CHECKARCHIVE_FULLCRC	2	/* 完全(格納ファイルの CRC チェック) */

/* 以下のフラグは上記と組み合わせて使用。*/
#define CHECKARCHIVE_RECOVERY	4	/* 破損ヘッダを読み飛ばして処理 */
#define CHECKARCHIVE_SFX		8	/* SFX かどうかを返す */
#define CHECKARCHIVE_ALL		16	/* ファイルの最後まで検索する */
#endif

BOOL WINAPI _export JackCheckArchive(LPCSTR _szFileName, const int _iMode);
int WINAPI _export JackGetFileCount(LPCSTR _szArcFile);

#if !defined(UNPACK_CONFIG_MODE)
#define UNPACK_CONFIG_MODE		1	/* 解凍系の設定 */
#define PACK_CONFIG_MODE		2	/* 圧縮系の設定 */
#endif

BOOL WINAPI _export JackConfigDialog(const HWND _hwnd, LPSTR _lpszComBuffer,
									 const int _iMode);

#if !defined(ISARC_FUNCTION_START)
#define ISARC_FUNCTION_START			0
#define ISARC							0	/* Jack */
#define ISARC_GET_VERSION				1	/* JackGetVersion */
#define ISARC_GET_CURSOR_INTERVAL		2	/* JackGetCursorInterval */
#define ISARC_SET_CURSOR_INTERVAL		3	/* JackSetCursorInterval */
#define ISARC_GET_BACK_GROUND_MODE		4	/* JackGetBackGroundMode */
#define ISARC_SET_BACK_GROUND_MODE		5	/* JackSetBackGroundMode */
#define ISARC_GET_CURSOR_MODE			6	/* JackGetCursorMode */
#define ISARC_SET_CURSOR_MODE			7	/* JackSetCursorMode */
#define ISARC_GET_RUNNING				8	/* JackGetRunning */

#define ISARC_CHECK_ARCHIVE				16	/* JackCheckArchive */
#define ISARC_CONFIG_DIALOG				17	/* JackConfigDialog */
#define ISARC_GET_FILE_COUNT			18	/* JackGetFileCount */
#define ISARC_QUERY_FUNCTION_LIST		19	/* JackQueryFunctionList */
#define ISARC_HOUT						20	/* (JackHOut) */
#define ISARC_STRUCTOUT					21	/* (JackStructOut) */
#define ISARC_GET_ARC_FILE_INFO			22	/* JackGetArcFileInfo */

#define ISARC_OPEN_ARCHIVE				23	/* JackOpenArchive */
#define ISARC_CLOSE_ARCHIVE				24	/* JackCloseArchive */
#define ISARC_FIND_FIRST				25	/* JackFindFirst */
#define ISARC_FIND_NEXT					26	/* JackFindNext */
#define ISARC_EXTRACT					27	/* (JackExtract) */
#define ISARC_ADD						28	/* (JackAdd) */
#define ISARC_MOVE						29	/* (JackMove) */
#define ISARC_DELETE					30	/* (JackDelete) */
#define ISARC_SETOWNERWINDOW			31	/* JackSetOwnerWindow */
#define ISARC_CLEAROWNERWINDOW			32	/* JackClearOwnerWindow */
#define ISARC_SETOWNERWINDOWEX			33	/* JackSetOwnerWindowEx */
#define ISARC_KILLOWNERWINDOWEX			34	/* JackKillOwnerWindowEx */

#define ISARC_GET_ARC_FILE_NAME			40	/* JackGetArcFileName */
#define ISARC_GET_ARC_FILE_SIZE			41	/* JackGetArcFileSize */
#define ISARC_GET_ARC_ORIGINAL_SIZE		42	/* JackArcOriginalSize */
#define ISARC_GET_ARC_COMPRESSED_SIZE	43	/* JackGetArcCompressedSize */
#define ISARC_GET_ARC_RATIO				44	/* JackGetArcRatio */
#define ISARC_GET_ARC_DATE				45	/* JackGetArcDate */
#define ISARC_GET_ARC_TIME				46	/* JackGetArcTime */
#define ISARC_GET_ARC_OS_TYPE			47	/* JackGetArcOSType */
#define ISARC_GET_ARC_IS_SFX_FILE		48	/* JackGetArcIsSFXFile */
#define ISARC_GET_ARC_WRITE_TIME_EX		49	/* JackGetArcWriteTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX	50	/* JackGetArcCreateTimeEx */
#define ISARC_GET_ARC_ACCESS_TIME_EX	51	/* JackGetArcAccessTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX2	52	/* JackGetArcCreateTimeEx2 */
#define ISARC_GET_ARC_WRITE_TIME_EX2	53	/* JackGetArcWriteTimeEx2 */
#define ISARC_GET_FILE_NAME				57	/* JackGetFileName */
#define ISARC_GET_ORIGINAL_SIZE			58	/* JackGetOriginalSize */
#define ISARC_GET_COMPRESSED_SIZE		59	/* JackGetCompressedSize */
#define ISARC_GET_RATIO					60	/* JackGetRatio */
#define ISARC_GET_DATE					61	/* JackGetDate */
#define ISARC_GET_TIME					62	/* JackGetTime */
#define ISARC_GET_CRC					63	/* JackGetCRC */
#define ISARC_GET_ATTRIBUTE				64	/* JackGetAttribute */
#define ISARC_GET_OS_TYPE				65	/* JackGetOSType */
#define ISARC_GET_METHOD				66	/* JackGetMethod */
#define ISARC_GET_WRITE_TIME			67	/* JackGetWriteTime */
#define ISARC_GET_CREATE_TIME			68	/* JackGetCreateTime */
#define ISARC_GET_ACCESS_TIME			69	/* JackGetAccessTime */
#define ISARC_GET_WRITE_TIME_EX			70	/* JackGetWriteTimeEx */
#define ISARC_GET_CREATE_TIME_EX		71	/* JackGetCreateTimeEx */
#define ISARC_GET_ACCESS_TIME_EX		72	/* JackGetAccessTimeEx */
#define ISARC_SET_ENUM_MEMBERS_PROC		80	/* JackSetEnumMembersProc */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC	81	/* JackClearEnumMembersProc */

#endif	/* ISARC_FUNCTION_START */

BOOL WINAPI _export JackQueryFunctionList(const int _iFunction);

#if 0	/* 現在未実装 */
#ifndef WM_ARCEXTRACT
#define WM_ARCEXTRACT	"wm_arcextract"

#define ARCEXTRACT_BEGIN		0	/* 該当ファイルの処理の開始 */
#define ARCEXTRACT_INPROCESS	1	/* 該当ファイルの展開中 */
#define ARCEXTRACT_END			2	/* 処理終了、関連メモリを開放 */
#define ARCEXTRACT_OPEN			3	/* 該当書庫の処理の開始 */
#define ARCEXTRACT_COPY			4	/* ワークファイルの書き戻し */

WINAPI _export JackSetOwnerWindow(const HWND _hwnd);
BOOL WINAPI _export JackClearOwnerWindow(VOID);

typedef BOOL CALLBACK ARCHIVERPROC(HWND _hwnd, UINT _uMsg,
						UINT _nState, LPEXTRACTINGINFOEX _lpEis);
typedef ARCHIVERPROC *LPARCHIVERPROC;

BOOL WINAPI _export JackSetOwnerWindowEx(HWND _hwnd,
		LPARCHIVERPROC _lpArcProc);
BOOL WINAPI _export JackKillOwnerWindowEx(HWND _hwnd);
#endif
#endif	/* 現在未実装 */

/* OpenArchive 系 API です。 */

#if 0	/* 現在未実装 */
#if !defined(EXTRACT_FOUND_FILE)
/* MODE (for UnarjOpenArchive) */
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

#if !defined(SFX_NOT)
#define SFX_NOT					0	/* 通常の書庫 */

#define SFX_DOS_S				1		/* LHA's SFX 系 (small) */
#define SFX_DOS_204S			1		/* LHA's SFX 2.04S 以降 */
#define SFX_DOS_213S			1		/* LHA's SFX 2.04S 以降 */
#define SFX_DOS_250S			2		/* LHA's SFX 2.50S 以降 */
#define SFX_DOS_260S			3		/* LHA's SFX 2.60S 以降 */
#define SFX_DOS_265S			3		/* LHA's SFX 2.60S 以降 */

#define SFX_DOS_L				51		/* LHA's SFX 系 (large) */
#define SFX_DOS_204L			51		/* LHA's SFX 2.04L 以降 */
#define SFX_DOS_213L			51		/* LHA's SFX 2.04L 以降 */
#define SFX_DOS_250L			52		/* LHA's SFX 2.50L 以降 */
#define SFX_DOS_260L			53		/* LHA's SFX 2.60L 以降 */
#define SFX_DOS_265L			53		/* LHA's SFX 2.60L 以降 */

#define SFX_DOS_LARC			201		/* SFX by LARC 系 */
#define SFX_DOS_LARC_S			201		/* SFX by LARC (small) */

#define SFX_DOS_LHARC			301		/* LHarc's SFX 系 */
#define SFX_DOS_LHARC_S			301		/* LHarc's SFX (small) */
#define SFX_DOS_LHARC_L			351		/* LHarc's SFX (large) */

#define SFX_WIN16_213			1001	/* LHA's SFX 2.13.w16 系 */
#define SFX_WIN16_213_1			1001	/* WinSFX 2.13.w16.1 */
#define SFX_WIN16_213_2			1002	/* WinSFX 2.13.w16.2 */
#define SFX_WIN16_213_3			1003	/* WinSFX 2.13.w16.3 以降 */

#define SFX_WIN16_250			1011	/* LHA's SFX 2.50.w16 系 */
#define SFX_WIN16_250_1			1011	/* WinSFXM 2.50.w16.0001 以降 */
#define SFX_WIN16_255_1			1021	/* WinSFXM 2.55.w16.0001 以降 */

#define SFX_WIN32_213			2001	/* LHA's SFX 2.13.w32 系 */
#define SFX_WIN32_213_1			2001	/* WinSFX32 2.13.w32.1 以降 */
#define SFX_WIN32_213_3			2002	/* WinSFX32 2.13.w32.3 以降 */

#define SFX_WIN32_250			2011	/* LHA's SFX 2.50.w32 系 */
#define SFX_WIN32_250_1			2011	/* WinSFX32M 2.50.w32.0001 以降 */
#define SFX_WIN32_250_6			2012	/* WinSFX32M 2.50.w32.0006 以降 */

#define SFX_LZHSFX				2051	/* LZHSFX 系 */
#define SFX_LZHSFX_1002			2051	/* LZHSFX 1.0.0.2 以降 */
#define SFX_LZHSFX_1100			2052	/* LZHSFX 1.1.0.0 以降 */

#define SFX_LZHAUTO				2101	/* LZHAUTO 系 */
#define SFX_LZHAUTO_0002		2101	/* LZHAUTO 0.0.0.2 以降 */
#define SFX_LZHAUTO_1000		2102	/* LZHAUTO 1.0.0.0 以降 */
#define SFX_LZHAUTO_1002		2102	/* LZHAUTO 1.0.0.0 以降 */
#define SFX_LZHAUTO_1100		2103	/* LZHAUTO 1.1.0.0 以降 */

#define SFX_WIN32_LHASA			3001	/* Lhasa インストーラ */

#define SFX_DOS_UNKNOWN			9901	/* 認識できない DOS SFX */
#define SFX_WIN16_UNKNOWN		9911	/* 認識できない Win16 SFX */
#define SFX_WIN32_UNKNOWN		9921	/* 認識できない Win32 SFX */
#endif
#endif	/* 現在未実装 */


HARC WINAPI _export JackOpenArchive(const HWND _hwnd, LPCSTR _szFileName,
									const DWORD _dwMode);
int WINAPI _export JackCloseArchive(HARC _harc);
int WINAPI _export JackFindFirst(HARC _harc, LPCSTR _szWildName,
								 INDIVIDUALINFO *_lpSubInfo);
int WINAPI _export JackFindNext(HARC _harc, INDIVIDUALINFO *_lpSubInfo);
DWORD WINAPI _export JackGetArcOriginalSize(HARC _harc);
DWORD WINAPI _export JackGetArcCompressedSize(HARC _harc);


#if 0	/* 現在未実装 */
int WINAPI _export JackGetArcFileName(HARC _harc, LPSTR _lpBuffer,
							const int _nSize);
DWORD WINAPI _export JackGetArcFileSize(HARC _harc);
WORD WINAPI _export JackGetArcRatio(HARC _harc);
WORD WINAPI _export JackGetArcDate(HARC _harc);
WORD WINAPI _export JackGetArcTime(HARC _harc);
BOOL WINAPI _export JackGetArcWriteTimeEx(HARC _harc,
							FILETIME *_lpftLastWriteTime);
BOOL WINAPI _export JackGetArcCreateTimeEx(HARC _harc,
							FILETIME *_lpftCreationTime);
BOOL WINAPI _export JackGetArcAccessTimeEx(HARC _harc,
							FILETIME *_lpftLastAccessTime);
UINT WINAPI _export JackGetArcOSType(HARC _harc);
int WINAPI _export JackIsSFXFile(HARC _harc);
int WINAPI _export JackGetFileName(HARC _harc, LPSTR _lpBuffer,
							const int _nSize);
int WINAPI _export JackGetMethod(HARC _harc, LPSTR _lpBuffer,
							const int _nSize);
DWORD WINAPI _export JackGetOriginalSize(HARC _harc);
DWORD WINAPI _export JackGetCompressedSize(HARC _harc);
WORD WINAPI _export JackGetRatio(HARC _harc);
WORD WINAPI _export JackGetDate(HARC _harc);
WORD WINAPI _export JackGetTime(HARC _harc);
DWORD WINAPI _export JackGetWriteTime(HARC _harc);
DWORD WINAPI _export JackGetAccessTime(HARC _harc);
DWORD WINAPI _export JackGetCreateTime(HARC _harc);
BOOL WINAPI _export JackGetWriteTimeEx(HARC _harc,
							FILETIME *_lpftLastWriteTime);
BOOL WINAPI _export JackGetCreateTimeEx(HARC _harc,
							FILETIME *_lpftCreationTime);
BOOL WINAPI _export JackGetAccessTimeEx(HARC _harc,
							FILETIME *_lpftLastAccessTime);
DWORD WINAPI _export JackGetCRC(HARC _harc);
int WINAPI _export JackGetAttribute(HARC _harc);
UINT WINAPI _export JackGetOSType(HARC _harc);

#ifndef FA_RDONLY
/* Attribute */
#define FA_RDONLY		0x01			/* 書き込み保護属性 */
#define FA_HIDDEN		0x02			/* 隠し属性 */
#define FA_SYSTEM		0x04			/* システム属性 */
#define FA_LABEL		0x08			/* ボリューム・ラベル */
#define FA_DIREC		0x10			/* ディレクトリ */
#define FA_ARCH			0x20			/* アーカイブ属性 */
#endif

#if !defined(SFXFLAG_AUTO)
#define SFXFLAG_AUTO			0x0001
#define SFXFLAG_OVERWRITE		0x0002
#define SFXFLAG_CHECKCRC		0x0004
#define SFXFLAG_CHECKTIME		0x0008
#define SFXFLAG_ATTRIBUTE		0x0010
#define SFXFLAG_NO_AUTO			0x0100
#define SFXFLAG_NO_OVERWRITE	0x0200
#define SFXFLAG_NO_CHECKCRC		0x0400
#define SFXFLAG_NO_CHECKTIME	0x0800
#define SFXFLAG_NO_ATTRIBUTE	0x1000
#endif
#endif	/* 現在未実装 */


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

#define ERROR_END	ERROR_CLOSE_FILE
#endif /* ERROR_START */


#if 0	/* 現在未実装 */
/* ### UNLHA32.DLL 独自の API です。### */
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

WORD WINAPI _export JackGetSubVersion(VOID);

int WINAPI _export JackExtractMem(const HWND _hwndParent,
		LPCSTR _szCmdLine, LPBYTE _lpBuffer, const DWORD _dwSize,
		time_t *_lpTime, LPWORD _lpwAttr, LPDWORD _lpdwWriteSize);
int WINAPI _export JackCompressMem(const HWND _hwndParent,
		LPCSTR _szCmdLine, const LPBYTE _lpBuffer, const DWORD _dwSize,
		const time_t *_lpTime, const LPWORD _lpwAttr,
		LPDWORD _lpdwWriteSize);

BOOL WINAPI _export JackSetEnumMembersProc(UNLHA_WND_ENUMMEMBPROC
		_lpEnumProc);
BOOL WINAPI _export JackClearEnumMembersProc(VOID);
#endif	/* 現在未実装 */


/* ### JACK32.DLL 独自の API です。### */
DWORD WINAPI _export JackRipper( LPCSTR pszFileName, LPCSTR pszDirName,
								 int iJackType, LPCSTR pszJackSize );
DWORD WINAPI _export JackCombine( LPCSTR pszFileName, LPCSTR pszDirName );

#ifdef __cplusplus
}
#endif

#endif	/* __JACK_API_H__ */
