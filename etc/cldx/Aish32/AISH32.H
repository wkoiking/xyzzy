#if !defined(WDN_AISH32_H)
#define WDN_AISH32_H

#include <windows.h>

#ifdef __BUILDING_THE_DLL
#define __EXPORT_TYPE __declspec(dllexport)
#else
#define __EXPORT_TYPE __declspec(dllimport)
#endif /* __BUILDING_THE_DLL */

#define AISH_VERSION (4)  /* Version 0.04 */

#if defined(__cplusplus)
extern "C" {
#endif


#if !defined(AISH_ERR_CODES)
#define AISH_ERR_CODES

/* エラー・コードは AISH32.DLL、AISHMV32.DLL 共通 */

/* エラー - 致命的なエラーで、その時点で実行を中止する */
#define AISH_ERROR_FILE_OPEN         0x800D // 復元時にファイルを開けませんでした
#define AISH_ERROR_ARC_FILE_OPEN     0x8018 // ISH file を開くことができません
#define AISH_ERROR_TMP_OPEN          0x8025 // 作業ファイルが開けません

/* AISH 独自の警告コード（処理を中断しない） */
/*                             0x8081 -      */

/* AISH 独自のエラー・コード（処理を中断する） */
#define AISH_ERROR_CANT_SPLIT        0x80A1 /* エンコード時に分割できない */
#define AISH_ERROR_CANT_READ_INFILE  0x80A2 /* encode するための file が開けません */
#define AISH_ERROR_TOO_MANY_VOLUMES  0x80A3 /* 分割数の指定が多すぎる */

#endif /* AISH_ERR_CODES */



/* 1. コマンド・ラインでの実行 */
int WINAPI __EXPORT_TYPE Aish(const HWND _hwnd, LPCSTR _szCmdLine,
	LPSTR _szOutput, const DWORD _dwSize);

/* 2. バージョンの取得 */
WORD WINAPI __EXPORT_TYPE AishGetVersion(VOID);

/* 3. カーソルの回転速度の取得 */
WORD WINAPI __EXPORT_TYPE AishGetCursorInterval(VOID);

/* 4. カーソルの回転速度の設定 */
BOOL WINAPI __EXPORT_TYPE AishSetCursorInterval(const WORD _Interval);

/* 5. バックグラウンド・モードであるかどうかの取得 */
BOOL WINAPI __EXPORT_TYPE AishGetBackGroundMode(VOID);

/* 6. バックグラウンド・モードにするか設定 */
BOOL WINAPI __EXPORT_TYPE AishSetBackGroundMode(const BOOL _bBackGroundMode);

/* 7. カーソルを表示するモードかを取得 */
BOOL WINAPI __EXPORT_TYPE AishGetCursorMode(VOID);

/* 8. カーソルを表示するモードかを設定 */
BOOL WINAPI __EXPORT_TYPE AishSetCursorMode(const BOOL _bCursorMode);

/* 10. DLL が動作中かを返す */
BOOL WINAPI __EXPORT_TYPE AishGetRunning(VOID);

/* 11. 指定されたファイルに ISH データが存在し、ISH データとして正しいか
 * どうかを調べる。
 */
/* !!! 注意：　このバージョンではファイルが存在するかを返すのみ !!! */
BOOL WINAPI __EXPORT_TYPE AishCheckArchive(LPCSTR _szFileName,
	const int _iMode);

/* 18. 指定された API が使用可能かを返す */
BOOL WINAPI __EXPORT_TYPE AishQueryFunctionList(const int _iFunction);


#if !defined(ISARC_FUNCTION_START)

/* AishQueryFunctionList() API が返す値 */

#define ISARC_FUNCTION_START			0
#define ISARC							0	/* Aish */
#define ISARC_GET_VERSION				1	/* AishGetVersion */
#define ISARC_GET_CURSOR_INTERVAL		2	/* AishGetCursorInterval */
#define ISARC_SET_CURSOR_INTERVAL		3	/* AishSetCursorInterval */
#define ISARC_GET_BACK_GROUND_MODE		4	/* AishGetBackGroundMode */
#define ISARC_SET_BACK_GROUND_MODE		5	/* AishSetBackGroundMode */
#define ISARC_GET_CURSOR_MODE			6	/* AishGetCursorMode */
#define ISARC_SET_CURSOR_MODE			7	/* AishSetCursorMode */
#define ISARC_GET_RUNNING				8	/* AishGetRunning */

#define ISARC_CHECK_ARCHIVE				16	/* AishCheckArchive */
#define ISARC_CONFIG_DIALOG				17	/* AishConfigDialog */
#define ISARC_GET_FILE_COUNT			18	/* AishGetFileCount */
#define ISARC_QUERY_FUNCTION_LIST		19	/* AishQueryFunctionList */
#define ISARC_HOUT						20	/* (AishHOut) */
#define ISARC_STRUCTOUT					21	/* (AishStructOut) */
#define ISARC_GET_ARC_FILE_INFO			22	/* AishGetArcFileInfo */

#define ISARC_OPEN_ARCHIVE				23	/* AishOpenArchive */
#define ISARC_CLOSE_ARCHIVE				24	/* AishCloseArchive */
#define ISARC_FIND_FIRST				25	/* AishFindFirst */
#define ISARC_FIND_NEXT					26	/* AishFindNext */
#define ISARC_EXTRACT					27	/* (AishExtract) */
#define ISARC_ADD						28	/* (AishAdd) */
#define ISARC_MOVE						29	/* (AishMove) */
#define ISARC_DELETE					30	/* (AishDelete) */
#define ISARC_SETOWNERWINDOW			31	/* AishSetOwnerWindow */
#define ISARC_CLEAROWNERWINDOW			32	/* AishClearOwnerWindow */
#define ISARC_SETOWNERWINDOWEX			33	/* AishSetOwnerWindowEx */
#define ISARC_KILLOWNERWINDOWEX			34	/* AishKillOwnerWindowEx */

#define ISARC_GET_ARC_FILE_NAME			40	/* AishGetArcFileName */
#define ISARC_GET_ARC_FILE_SIZE			41	/* AishGetArcFileSize */
#define ISARC_GET_ARC_ORIGINAL_SIZE		42	/* AishArcOriginalSize */
#define ISARC_GET_ARC_COMPRESSED_SIZE	43	/* AishGetArcCompressedSize */
#define ISARC_GET_ARC_RATIO				44	/* AishGetArcRatio */
#define ISARC_GET_ARC_DATE				45	/* AishGetArcDate */
#define ISARC_GET_ARC_TIME				46	/* AishGetArcTime */
#define ISARC_GET_ARC_OS_TYPE			47	/* AishGetArcOSType */
#define ISARC_GET_ARC_IS_SFX_FILE		48	/* AishGetArcIsSFXFile */
#define ISARC_GET_ARC_WRITE_TIME_EX		49	/* AishGetArcWriteTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX	50	/* AishGetArcCreateTimeEx */
#define	ISARC_GET_ARC_ACCESS_TIME_EX	51	/* AishGetArcAccessTimeEx */
#define	ISARC_GET_ARC_CREATE_TIME_EX2	52	/* AishGetArcCreateTimeEx2 */
#define ISARC_GET_FILE_NAME				57	/* AishGetFileName */
#define ISARC_GET_ORIGINAL_SIZE			58	/* AishGetOriginalSize */
#define ISARC_GET_COMPRESSED_SIZE		59	/* AishGetCompressedSize */
#define ISARC_GET_RATIO					60	/* AishGetRatio */
#define ISARC_GET_DATE					61	/* AishGetDate */
#define ISARC_GET_TIME					62	/* AishGetTime */
#define ISARC_GET_CRC					63	/* AishGetCRC */
#define ISARC_GET_ATTRIBUTE				64	/* AishGetAttribute */
#define ISARC_GET_OS_TYPE				65	/* AishGetOSType */
#define ISARC_GET_METHOD				66	/* AishGetMethod */
#define ISARC_GET_WRITE_TIME			67	/* AishGetWriteTime */
#define ISARC_GET_CREATE_TIME			68	/* AishGetCreateTime */
#define ISARC_GET_ACCESS_TIME			69	/* AishGetAccessTime */
#define ISARC_GET_WRITE_TIME_EX			70	/* AishGetWriteTimeEx */
#define ISARC_GET_CREATE_TIME_EX		71	/* AishGetCreateTimeEx */
#define ISARC_GET_ACCESS_TIME_EX		72	/* AishGetAccessTimeEx */
#define ISARC_SET_ENUM_MEMBERS_PROC		80  /* AishSetEnumMembersProc */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC	81	/* AishClearEnumMembersProc */

#endif	/* ISARC_FUNCTION_START */


/* 内部で使用している: AISHMV32.DLL が AISH32.DLL を呼び出すときに使う型 */
typedef int WINAPI (*AishType)(const HWND _hwnd, LPCSTR _szCmdLine,
	LPSTR _szOutput, const DWORD _dwSize);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* WDN_AISH32_H */
