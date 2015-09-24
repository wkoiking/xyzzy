//
//  ish file converter (ish.dll) for Microsoft Windows
//
//---------------------------------------------------------------
//	   (C) Pekin
//	  Public Domain Software
//
//  1987.11.11	  	ken Miyazaki(ken)
//  1987.11.28		O Imaizumi(Gigo)
//  1987.12.05 		Bug Fix
//  1987.12.06 		OS9Ext support.
//  1988.01.31 		V.1.11
//  1988.02.06 		for Human68K(Gigo)
//  1988.05.10 		for UNIX (kondo)
//  1990.07.12 		non-kana (keizo)
//  1994.			for Microsoft Windows (nonki)
//  1995.01			for Visual C and Error Code (asato)
//  1995.02			/Y option (asato)
//  1995.03.26		Fix Default Dir Bug (asato)
//  1995.06.06      Fix Wildcard matching Bug(asato)
//
//  special thanks for M.ishizuka(ish)
//---------------------------------------------------------------
//

#ifndef ISH_DLL_H       
#define ISH_DLL_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
//---------------------------------------------------------------

#define ISH_VERSION             35

/* WARNING */
#define ERROR_DISK_SPACE		0x8005		// Diskの空きがありません(解凍時)
#define ERROR_READ_ONLY			0x8006		// 解凍先のファイルがリードオンリー(Replaceできない)
#define ERROR_USER_SKIP			0x8007		// ユーザーによって解凍をスキップされました
#define ERROR_UNKOWN_TYPE		0x8008
#define ERROR_METHOD			0x8009
#define ERROR_PASSWORD_FILE		0x800A
#define ERROR_VERSION			0x800B
#define ERROR_FILE_CRC			0x800C		// エラー訂正不可
#define ERROR_FILE_OPEN			0x800D		// 解凍時にファイルがオープンできない
#define ERROR_MORE_FRESH		0x800E
#define ERROR_NOT_EXIST			0x800F
#define ERROR_ALREADY_EXIST		0x8010

#define ERROR_TOO_MANY_FILES	0x8011

/* ERROR */
#define ERROR_DIRECTORY			0x8012
#define ERROR_CANNOT_WRITE		0x8013
#define ERROR_HUFFMAN_CODE		0x8014
#define ERROR_COMMENT_HEADER	0x8015
#define ERROR_HEADER_CRC		0x8016
#define ERROR_HEADER_BROKEN		0x8017
#define ERROR_ARCHIVE_FILE_OPEN	0x8018		// Headerがない
#define ERROR_NOT_ARCHIVE_FILE	0x8019
#define ERROR_CANNOT_READ		0x801A
#define ERROR_FILE_STYLE		0x801B
#define ERROR_COMMAND_NAME		0x801C
#define ERROR_MORE_HEAP_MEMORY	0x801D
#define ERROR_ENOUGH_MEMORY		0x801E
#define ERROR_ALREADY_RUNNING	0x801F
#define ERROR_USER_CANCEL		0x8020

#define ERROR_NOT_SUPPORT		0x8023

#define	ERROR_CANNOT_CREATE		0x8030		// ISHファイルが作成できません		
#define	ERROR_CANNOT_OPEN		0x8031		// ファイルがオープンできない(作成時)

// --------------------------------------------------------------
// Ish : Ishの処理を実行します。
//                   
//		hwnd        : ウインドウハンドル
//		lpszCmdLine : コマンドライン
//                    (restoreであればファイル名のみ、
//                     storeであれば"/s ファイル名"でできます。）
//
//         ・復元／作成 共通のオプション
//				/f=<dirname> : 解凍先ディレクトリを指定します
//
//         ・復元時のオプション
//				/r : restore new file(s)
//				/a : restore all file(s)
//				/q : restore with question
//
//         ・作成時のオプション
//
//           ・作成モード
//				/s : store a JIS8 format ish file
//		       	/s8: same as -s
//				/ss: store a shift JIS format ish file  
//				/s7: store a JIS7 format ish file
//				/sn: store a shift JIS non-kana format ish file
//
//           ・対象OS
//				/m : MS-DOS
//				/u : UNIX
//				/c : CP/M
//				/9 : OS-9
//				/? : other OS(s)
//				/* : all OS(s)
//
//           ・エスケープシーケンスの使用
//				/n : no ESC sequence in TITLE line      
//				/e : ESC sequence in TITLE line
//
//           ・タイトルを書き込む行数
//				/tn: title in n line
//
//			 ・Decodeファイル指定(指定ファイルのみDecodeする)
//				/Y <filename>
//				複数指定の場合は、/Y <filename> /Y <filename> ...
//				ワイルドカード指定(?, *)も可
//
//			 ・追加出力(作成時のみ有効)
//				/YA <filename>
//				指定のファイルに追加出力する
//				ただし、既に同じファイルがある場合に注意
//
// -------------------------------------------------------------
//int WINAPI _export Ish(HWND hwnd, LPSTR lpszCmdLine, LPSTR lpszDir);
int WINAPI _export Ish(HWND hwnd, LPSTR lpszCmdLine);

// --------------------------------------------------------------
// IshGetVersion : ish.dllのバージョンを返します。
//                   
//    結果はintで戻します。例えば
//	    001はVer0.01
//      100はVer1.00
//    となります。
// --------------------------------------------------------------
int WINAPI _export IshGetVersion(void);

// --------------------------------------------------------------
// IshGetRunning : ish.dllが動作中かを返します。
//                   
//    TRUEであれば、動作中なので使用できません。
//	  FALSEであれば、使用できます。
// --------------------------------------------------------------
BOOL WINAPI _export IshGetRunning(void);

// --------------------------------------------------------------
// IshConfigDialog : ish.dllの環境設定ダイアログを表示します。
//                   戻りはlpszOptにコマンドラインの形式で
//                   返されます。
//
//	hwnd 	: ウインドウハンドル
//	lpszOpt : ダイアログで設定したコマンドラインを格納します。
//            lpszOptは、必ず128バイト以上の大きさを用意してください。
//            また、呼び出し時にパラメータを設定すればダイアログの初期
//            値がそのパラメータになります。
//            ""を渡せば、archive.iniに設定されたパラメータが初期値と
//            なります。
//  mode    : 復元（解凍）系のコマンドラインを設定する場合は
//            UNPACK_CONFIG_MODEを、作成（圧縮）系のコマンドラインを
//            設定する場合はPACK_CONFIG_MODEを設定してください。
//  戻り値  : 設定を行った場合はTRUEを、キャンセルした場合はFALSEを
//            戻します。
//
// --------------------------------------------------------------
#define UNPACK_CONFIG_MODE	1
#define PACK_CONFIG_MODE	2

BOOL WINAPI _export IshConfigDialog(HWND hwnd, LPSTR lpszOpt, int mode);

// --------------------------------------------------------------
// IshCheckArchive : lpszFileNameで指定したファイルにishデータが
//                   含まれているかを調べます。
//                   ただし、現在の仕様ではファイルが存在すればTRUEを
//					 返しています。
// --------------------------------------------------------------
BOOL WINAPI _export IshCheckArchive(LPSTR lpszFileName, int iMode);

// --------------------------------------------------------------
// 以下の関数は未実装ですが、LHA.DLL等他のアーカイブDLLとの
// 互換性のためにエントリだけ用意してあります。
BOOL WINAPI _export IshGetBackGroundMode(void);
BOOL WINAPI _export IshSetBackGroundMode(BOOL bBackGroundMode);
BOOL WINAPI _export IshGetCursorMode(void);
BOOL WINAPI _export IshSetCursorMode(BOOL bCursorMode);
unsigned int WINAPI _export IshGetCursorInterval(void);
BOOL WINAPI _export IshSetCursorInterval(unsigned int uInterval);

// --------------------------------------------------------------
// IshEx : Ishの処理を実行します。
//                   
//  LHA.DLL互換のインターフェイスです。
//	コマンドラインの指定は、Ish()と同じです。
//  実行結果を、szOutputに返します。
//                   
//	lpszCmdLine : コマンドライン
//                    (restoreであればファイル名のみ、
//                     storeであれば"/s ファイル名"でできます。）
//  lpszOutput   : 結果を返すバッファを指定します。
//  wSize        : バッファのサイズを指定します。返す結果のサイズより小さい場合は
//                 指定サイズまで返される。
//  戻り値       : 正常終了の場合は0、エラーのときはエラーコード
// -------------------------------------------------------------
int WINAPI _export IshEx(LPSTR lpszCmdLine, LPSTR lpszOutput, WORD wSize);


// --------------------------------------------------------------
// IshListFile : lpszFileNameで指定したファイル内のishデータファイル
//               一覧をszOutputで指定された領域に次の形式で返します。
//               (1:,2:は含まれません。各行の末尾はLF(0Ah)となります。)
// 1:
// 2:Listing of Ish : <file名>
// 3:
// 4:
// 5:  Name            Size      Date     Time    Type  Vol
// 6:
// 7:--------------  --------  -------- -------- ------ ---
// 8:
// 9:<file1>
//10:                 1571168  94-12-05 17:19:36 MS-DOS 
//11:<file2>
//12:                 1571168  94-12-05 17:19:36 OTHER  
//  :
//  :......................................................
//  :
// n:--------------  --------  -------- -------- ------ ---
//
//	lpszFileName : ISHデータを含むファイル名を指定します。
//  lpszOutput   : 結果を返すバッファを指定します。
//  wSize        : バッファのサイズを指定します。返す結果のサイズより小さい場合は
//                 指定サイズまで返される。
//  戻り値       : 正常終了の場合は0、エラーのときはエラーコード
//                 ISHデータが含まれない場合は、ERROR_NOT_ARCHIVE_FILEを返す。
//  その他       : エラーメッセージは出力しない
//---------------------------------------------------------------
int WINAPI _export IshFileList(LPSTR lpszFileName, LPSTR lpszOutput, WORD wSize);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  // __cplusplus

#endif	// ISH_DLL_H
