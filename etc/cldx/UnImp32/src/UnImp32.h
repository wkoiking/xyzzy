// The BSD License
// http://www.opensource.org/licenses/bsd-license.php
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (C) 2002- HyperBeat
// All Right Reserved.
//
// $Id: UnImp32.h,v 1.18 2005/06/17 13:15:49 hyperbeat Exp $
//

#ifndef __UNIMP32_H__
#define __UNIMP32_H__

#ifndef FNAME_MAX32
#define FNAME_MAX32 512
#endif

#if !defined(ARC_DECSTRACT)
#define ARC_DECSTRACT

#pragma pack(1)

// 格納ファイルの情報
typedef struct
{
    DWORD dwOriginalSize;                       // ファイルサイズ
    DWORD dwCompressedSize;                     // 圧縮ファイルサイズ
    DWORD dwCRC;                                // チェックサム
    UINT uFlag;                                 // 処理結果
    UINT uOSType;                               // OSの種類
    WORD wRatio;                                // 圧縮率
    WORD wDate;                                 // 日付
    WORD wTime;                                 // 時間
    char szFileName[FNAME_MAX32 + 1];           // ファイル名
    char dummy1[3];
    char szAttribute[8];                        // 属性
    char szMode[8];                             // 圧縮方法
}INDIVIDUALINFO,*LPINDIVIDUALINFO;

typedef struct
{
    DWORD dwFileSize;
    DWORD dwWriteSize;
    char szSourceFileName[FNAME_MAX32 + 1];
    char dummy1[3];
    char szDestFileName[FNAME_MAX32 + 1];
    char dummy[3];
}EXTRACTINGINFO,*LPEXTRACTINGINFO;

typedef struct
{
    EXTRACTINGINFO exinfo;
    DWORD dwCompressedSize;
    DWORD dwCRC;
    UINT uOSType;
    WORD wRatio;
    WORD wDate;
    WORD wTime;
    char szAttribute[8];
    char szMode[8];
}EXTRACTINGINFOEX,*LPEXTRACTINGINFOEX;

typedef struct
{

    DWORD dwStructSize;
    EXTRACTINGINFO exinfo;
    DWORD dwFileSize;
    DWORD dwCompressedSize;
    DWORD dwWriteSize;
    DWORD dwAttributes;
    DWORD dwCRC;
    UINT uOSType;
    WORD wRatio;
    FILETIME ftCreateTime;
    FILETIME ftAccessTime;
    FILETIME ftWriteTime;
    char szMode[8];
    char szSourceFileName[FNAME_MAX32 + 1];
    char dummy1[3];
    char szDestFileName[FNAME_MAX32 + 1];
    char dummy2[3];
}EXTRACTINGINFOEX32,*LPEXTRACTINGINFOEX32;

typedef struct
{
    DWORD dwStructSize;
    EXTRACTINGINFO exinfo;
    LONGLONG llFileSize;
    LONGLONG llCompressedSize;
    LONGLONG llWriteSize;
    DWORD dwAttributes;
    DWORD dwCRC;
    UINT uOSType;
    WORD wRatio;
    FILETIME ftCreateTime;
    FILETIME ftAccessTime;
    FILETIME ftWriteTime;
    char szMode[8];
    char szSourceFileName[FNAME_MAX32 + 1];
    char dummy1[3];
    char szDestFileName[FNAME_MAX32 + 1];
    char dummy2[3];
}EXTRACTINGINFOEX64,*LPEXTRACTINGINFOEX64;

typedef struct
{
    DWORD dwStructSize;
    UINT uCommand;
    DWORD dwOriginalSize;
    DWORD dwCompressedSize;
    DWORD dwAttributes;
    DWORD dwCRC;
    UINT uOSType;
    WORD wRatio;
    FILETIME ftCreateTime;
    FILETIME ftAccessTime;
    FILETIME ftWriteTime;
    char szFileName[FNAME_MAX32 + 1];
    char dummy1[3];
    char szAddFileName[FNAME_MAX32 + 1];
    char dummy2[3];
}ENUM_MEMBER_INFO,*LPENUM_MEMBER_INFO;

typedef struct
{
    DWORD dwStructSize;
    UINT uCommand;
    LONGLONG llOriginalSize;
    LONGLONG llCompressedSize;
    DWORD dwAttributes;
    DWORD dwCRC;
    UINT uOSType;
    WORD wRatio;
    FILETIME ftCreateTime;
    FILETIME ftAccessTime;
    FILETIME ftWriteTime;
    char szFileName[FNAME_MAX32 + 1];
    char dummy1[3];
    char szAddFileName[FNAME_MAX32 + 1];
    char dummy2[3];
}ENUM_MEMBER_INFO64,*LPENUM_MEMBER_INFO64;

#pragma pack()

typedef HGLOBAL HARC;

typedef BOOL CALLBACK ARCHIVERPROC(HWND hwnd,UINT uMsg,UINT nState,LPVOID lpEis);
typedef ARCHIVERPROC *LPARCHIVERPROC;

typedef BOOL CALLBACK EnumMembersProc(LPVOID lpInfo);

#endif

#if !defined(ERROR_START)
#define ERROR_START             0x8000

/* WARNING */
#define ERROR_DISK_SPACE        0x8005 /* ディスクの空き容量が足りない */
#define ERROR_READ_ONLY         0x8006 /* 読み取り専用属性のファイルが存在 */
#define ERROR_USER_SKIP         0x8007 /* ユーザによってスキップされた */
#define ERROR_UNKNOWN_TYPE      0x8008 /* 特殊属性のファイルが存在 */
#define ERROR_METHOD            0x8009 /* 対応していないメソッド */
#define ERROR_PASSWORD_FILE     0x800A /* パスワードが正しくない */
#define ERROR_VERSION           0x800B /* 対応していないバージョン */
#define ERROR_FILE_CRC          0x800C /* 圧縮イメージが不正 */
#define ERROR_FILE_OPEN         0x800D /* ファイルが開けなかった */
#define ERROR_MORE_FRESH        0x800E /* 上書きに失敗 */
#define ERROR_NOT_EXIST         0x800F /* ファイルが存在しない */
#define ERROR_ALREADY_EXIST     0x8010 /* ファイルが既に存在している */
#define ERROR_TOO_MANY_FILES    0x8011 /* 格納するファイルが多すぎる */

/* ERROR */
#define ERROR_MAKEDIRECTORY     0x8012 /* ディレクトリが作成できない */
#define ERROR_CANNOT_WRITE      0x8013 /* ファイルへの書き込みに失敗 */
#define ERROR_HUFFMAN_CODE      0x8014 /* 書庫の破損によりハフマン圧縮用テーブルが初期化できなかった */
#define ERROR_COMMENT_HEADER    0x8015 /* コメントヘッダが不正 */
#define ERROR_HEADER_CRC        0x8016 /* ヘッダが不正 */
#define ERROR_HEADER_BROKEN     0x8017 /* ヘッダが壊れている */
#define ERROR_ARC_FILE_OPEN     0x8018 /* 書庫が開けない */
#define ERROR_NOT_ARC_FILE      0x8019 /* 書庫ではない */
#define ERROR_CANNOT_READ       0x801A /* ファイルからデータを読み込めない */
#define ERROR_FILE_STYLE        0x801B /* 指定された書庫を書庫として認識できなかった */
#define ERROR_COMMAND_NAME      0x801C /* 不正な命令 */
#define ERROR_MORE_HEAP_MEMORY  0x801D /* 作業用のメモリの確保に失敗 */
#define ERROR_ENOUGH_MEMORY     0x801E /* バッファメモリの確保に失敗 */
#if !defined(ERROR_ALREADY_RUNNING)
#define ERROR_ALREADY_RUNNING   0x801F /* DLLが動作中 */
#endif
#define ERROR_USER_CANCEL       0x8020 /* ユーザによりキャンセルされた */
#define ERROR_HARC_ISNOT_OPENED 0x8021 /* 有効な書庫ハンドルではない */
#define ERROR_NOT_SEARCH_MODE   0x8022 /* 書庫ハンドルの検索対象のファイルが指定されていない*/
#define ERROR_NOT_SUPPORT       0x8023 /* DLLでサポートされていないAPIが使用されている */
#define ERROR_TIME_STAMP        0x8024 /* タイムスタンプが不正 */
#define ERROR_TMP_OPEN          0x8025 /* 一時ファイルを開けない */
#define ERROR_LONG_FILE_NAME    0x8026 /* 書庫に格納できない長さのファイル名 */
#define ERROR_ARC_READ_ONLY     0x8027 /* 書庫が読みとり専用である */
#define ERROR_SAME_NAME_FILE    0x8028 /* 書庫に同名のファイル名で格納しようとしている */
#define ERROR_NOT_FIND_ARC_FILE 0x8029 /* 書庫が見つからない */
#define ERROR_RESPONSE_READ     0x802A /* レスポンスファイルが読み込めない */
#define ERROR_NOT_FILENAME      0x802B /* 対象ファイルが指定されていない */
#define ERROR_TMP_COPY          0x802C /* 一時ファイルをコピーできない */
#define ERROR_EOF               0x802D /* 予期しないEOF */
#define ERROR_ADD_TO_LARC       0x802E /* 指定された書庫が書庫操作の許されていない書庫 */
#define ERROR_TMP_BACK_SPACE    0x802F /* 作業用の書庫をコピーする際にディスク容量が足りない */
#define ERROR_SHARING           0x8030 /* ファイルにアクセスできない */
#define ERROR_NOT_FIND_FILE     0x8031 /* ファイルが見つからない */
#define ERROR_LOG_FILE          0x8032 /* ログファイルへの書き込みに失敗 */
#define	ERROR_NO_DEVICE         0x8033 /* デバイス(ドライブ)へのアクセスに失敗 */
#define ERROR_GET_ATTRIBUTES    0x8034 /* 属性の取得に失敗 */
#define ERROR_SET_ATTRIBUTES    0x8035 /* 属性の保存に失敗 */
#define ERROR_GET_INFORMATION   0x8036 /* ディスク空き容量の取得やファイルの更新時刻の取得に失敗 */
#define ERROR_GET_POINT         0x8037 /* ファイルポインタの取得に失敗 */
#define ERROR_SET_POINT         0x8038 /* ファイルポインタの設定に失敗 */
#define ERROR_CONVERT_TIME      0x8039 /* 時間の変換に失敗 */
#define ERROR_GET_TIME          0x803A /* ファイルスタンプの取得に失敗 */
#define ERROR_SET_TIME          0x803B /* ファイルスタンプの保存に失敗 */
#define ERROR_CLOSE_FILE        0x803C /* ファイルを閉じれない */
#define ERROR_HEAP_MEMORY       0x803D /* メモリを解放できない */
#define ERROR_HANDLE            0x803E /* 検索ハンドルの解放に失敗 */
#define ERROR_TIME_STAMP_RANGE  0x803F /* 不正なタイムスタンプ */
#define ERROR_MAKE_ARCHIVE      0x8040 /* 書庫が作成できない */
#define ERROR_NOT_CONFIRM_NAME  0x8041 /* 書庫に格納されたファイル名の長さと実際のファイル名の長さが一致しない */
#define ERROR_UNEXPECTED_EOF    0x8042 /* 書庫に格納されたサイズのヘッダの読み込みに失敗 */
#define ERROR_INVALID_END_MARK  0x8043 /* 書庫に不正なエンドマークが存在する */
#define ERROR_INVOLVED_LZH      0x8044 /* ネストした書庫を外側から開こうとした */
#define ERROR_NO_END_MARK       0x8045 /* 書庫にエンドマークが記録されていない */
#define ERROR_HDR_INVALID_SIZE  0x8046 /* 格納されたヘッダサイズが不正 */
#define ERROR_UNKNOWN_LEVEL     0x8047 /* 対応していないレベル */
#define ERROR_BROKEN_DATA       0x8048 /* 圧縮イメージが壊れている */
#define ERROR_INVALID_PATH      0x8049 /* 不正なパス */
#define ERROR_TOO_BIG           0x804A /* ファイルサイズがファイルシステムで対応していない */
#define ERROR_UNKNOWN           0x8FFF /* 不明なエラー */
#define ERROR_END               ERROR_UNKNOWN

#endif

#if !defined(CHECKARCHIVE_RAPID)
#define CHECKARCHIVE_RAPID       0 /* 簡易(最初の3ヘッダまで) */
#define CHECKARCHIVE_BASIC       1 /* 標準(全てのヘッダ) */
#define CHECKARCHIVE_FULLCRC     2 /* 完全(格納ファイルのCRCチェック) */

/* 以下のフラグは上記と組み合わせて使用 */
#define CHECKARCHIVE_RECOVERY    4 /* 破損ヘッダを読み飛ばして処理 */
#define CHECKARCHIVE_SFX         8 /* SFXかどうかを返す */
#define CHECKARCHIVE_ALL        16 /* ファイルの最後まで検索する */
#define CHECKARCHIVE_ENDDATA    32 /* 書庫より後ろの余剰データを許可 */
#endif

#if !defined(UNPACK_CONFIG_MODE)
#define	UNPACK_CONFIG_MODE       1 /* 解凍系の設定 */
#define PACK_CONFIG_MODE         2 /* 圧縮系の設定 */
#endif

#if !defined(ISARC_FUNCTION_START)
#define ISARC_FUNCTION_START                 0
#define ISARC                                0 /* UnImp */
#define ISARC_GET_VERSION                    1 /* UnImpGetVersion */
#define ISARC_GET_CURSOR_INTERVAL            2 /* UnImpGetCursorInterval */
#define ISARC_SET_CURSOR_INTERVAL            3 /* UnImpSetCursorInterval */
#define ISARC_GET_BACK_GROUND_MODE           4 /* UnImpGetBackGroundMode */
#define ISARC_SET_BACK_GROUND_MODE           5 /* UnImpSetBackGroundMode */
#define ISARC_GET_CURSOR_MODE                6 /* UnImpGetCursorMode */
#define ISARC_SET_CURSOR_MODE                7 /* UnImpSetCursorMode */
#define ISARC_GET_RUNNING                    8 /* UnImpGetRunning */
#define ISARC_CHECK_ARCHIVE                 16 /* UnImpCheckArchive */
#define ISARC_CONFIG_DIALOG                 17 /* UnImpConfigDialog */
#define ISARC_GET_FILE_COUNT                18 /* UnImpGetFileCount */
#define ISARC_QUERY_FUNCTION_LIST           19 /* UnImpQueryFunctionList */
#define ISARC_HOUT                          20 /* UnImpHOut */
#define ISARC_STRUCTOUT                     21 /* UnImpStructOut */
#define ISARC_GET_ARC_FILE_INFO             22 /* UnImpGetArcFileInfo */
#define ISARC_OPEN_ARCHIVE                  23 /* UnImpOpenArchive */
#define ISARC_CLOSE_ARCHIVE                 24 /* UnImpCloseArchive */
#define ISARC_FIND_FIRST                    25 /* UnImpFindFirst */
#define ISARC_FIND_NEXT                     26 /* UnImpFindNext */
#define ISARC_EXTRACT                       27 /* UnImpExtract */
#define ISARC_ADD                           28 /* UnImpAdd */
#define ISARC_MOVE                          29 /* UnImpMove */
#define ISARC_DELETE                        30 /* UnImpDelete */
#define ISARC_SETOWNERWINDOW                31 /* UnImpSetOwnerWindow */
#define ISARC_CLEAROWNERWINDOW              32 /* UnImpClearOwnerWindow */
#define ISARC_SETOWNERWINDOWEX              33 /* UnImpSetOwnerWindowEx */
#define ISARC_KILLOWNERWINDOWEX             34 /* UnImpKillOwnerWindowEx */
#define ISARC_GET_ARC_FILE_NAME             40 /* UnImpGetArcFileName */
#define ISARC_GET_ARC_FILE_SIZE             41 /* UnImpGetArcFileSize */
#define ISARC_GET_ARC_ORIGINAL_SIZE         42 /* UnImpArcOriginalSize */
#define ISARC_GET_ARC_COMPRESSED_SIZE       43 /* UnImpGetArcCompressedSize */
#define ISARC_GET_ARC_RATIO                 44 /* UnImpGetArcRatio */
#define ISARC_GET_ARC_DATE                  45 /* UnImpGetArcDate */
#define ISARC_GET_ARC_TIME                  46 /* UnImpGetArcTime */
#define ISARC_GET_ARC_OS_TYPE               47 /* UnImpGetArcOSType */
#define ISARC_GET_ARC_IS_SFX_FILE           48 /* UnImpGetArcIsSFXFile */
#define ISARC_GET_ARC_WRITE_TIME_EX         49 /* UnImpGetArcWriteTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX        50 /* UnImpGetArcCreateTimeEx */
#define ISARC_GET_ARC_ACCESS_TIME_EX        51 /* UnImpGetArcAccessTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX2       52 /* UnImpGetArcCreateTimeEx2 */
#define ISARC_GET_ARC_WRITE_TIME_EX2        53 /* UnImpGetArcWriteTimeEx2 */
#define ISARC_GET_FILE_NAME                 57 /* UnImpGetFileName */
#define ISARC_GET_ORIGINAL_SIZE             58 /* UnImpGetOriginalSize */
#define ISARC_GET_COMPRESSED_SIZE           59 /* UnImpGetCompressedSize */
#define ISARC_GET_RATIO                     60 /* UnImpGetRatio */
#define ISARC_GET_DATE                      61 /* UnImpGetDate */
#define ISARC_GET_TIME                      62 /* UnImpGetTime */
#define ISARC_GET_CRC                       63 /* UnImpGetCRC */
#define ISARC_GET_ATTRIBUTE                 64 /* UnImpGetAttribute */
#define ISARC_GET_OS_TYPE                   65 /* UnImpGetOSType */
#define ISARC_GET_METHOD                    66 /* UnImpGetMethod */
#define ISARC_GET_WRITE_TIME                67 /* UnImpGetWriteTime */
#define ISARC_GET_CREATE_TIME               68 /* UnImpGetCreateTime */
#define ISARC_GET_ACCESS_TIME               69 /* UnImpGetAccessTime */
#define ISARC_GET_WRITE_TIME_EX             70 /* UnImpGetWriteTimeEx */
#define ISARC_GET_CREATE_TIME_EX            71 /* UnImpGetCreateTimeEx */
#define ISARC_GET_ACCESS_TIME_EX            72 /* UnImpGetAccessTimeEx */
#define ISARC_SET_ENUM_MEMBERS_PROC         80 /* UnImpSetEnumMembersProc */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC       81 /* UnImpClearEnumMembersProc */
#define ISARC_GET_ARC_FILE_SIZE_EX          82 /* UnImpGetArcFileSizeEx */
#define ISARC_GET_ARC_ORIGINAL_SIZE_EX      83 /* UnImpArcOriginalSizeEx */
#define ISARC_GET_ARC_COMPRESSED_SIZE_EX    84 /* UnImpGetArcCompressedSizeEx */
#define ISARC_GET_ORIGINAL_SIZE_EX          85 /* UnImpGetOriginalSizeEx */
#define ISARC_GET_COMPRESSED_SIZE_EX        86 /* UnImpGetCompressedSizeEx */
#define ISARC_SETOWNERWINDOWEX64            87 /* UnImpSetOwnerWindowEx64 */
#define ISARC_KILLOWNERWINDOWEX64           88 /* UnImpKillOwnerWindowEx64 */
#define ISARC_SET_ENUM_MEMBERS_PROC64       89 /* UnImpSetEnumMembersProc64 */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC64     90 /* UnImpClearEnumMembersProc64 */
#define ISARC_OPEN_ARCHIVE2                 91 /* UnImpOpenArchive2 */
#define ISARC_GET_ARC_READ_SIZE             92 /* UnImpGetArcReadSize */
#define ISARC_GET_ARC_READ_SIZE_EX          93 /* UnImpGetArcReadSizeEx*/
#define ISARC_FUNCTION_END                  ISARC_GET_ARC_READ_SIZE_EX

#endif

#ifndef WM_ARCEXTRACT
#define	WM_ARCEXTRACT "wm_arcextract"

#define ARCEXTRACT_BEGIN        0 /* 該当ファイルの処理の開始 */
#define ARCEXTRACT_INPROCESS    1 /* 該当ファイルの展開中 */
#define ARCEXTRACT_END          2 /* 処理終了、関連メモリを開放 */
#define ARCEXTRACT_OPEN         3 /* 該当書庫の処理の開始 */
#define ARCEXTRACT_COPY         4 /* ワークファイルの書き戻し */
#define ARCEXTRACT_SEARCH       5
#define ARCEXTRACT_ADDEND       6
#define ARCEXTRACT_CHECK        7

#endif

#define UNIMP_LIST_COMMAND        1
#define UNIMP_ADD_COMMAND         2
#define UNIMP_FRESH_COMMAND       3
#define UNIMP_DELETE_COMMAND      4
#define UNIMP_EXTRACT_COMMAND     5
#define UNIMP_PRINT_COMMAND       6
#define UNIMP_TEST_COMMAND        7
#define UNIMP_MAKESFX_COMMAND     8
#define UNIMP_JOINT_COMMAND       9
#define UNIMP_CONVERT_COMMAND    10
#define UNIMP_RENAME_COMMAND     11

#ifndef FA_RDONLY
#define FA_RDONLY       0x01 /* 書き込み保護属性 */
#define FA_HIDDEN       0x02 /* 隠し属性 */
#define FA_SYSTEM       0x04 /* システム属性 */
#define FA_LABEL        0x08 /* ボリューム・ラベル */
#define FA_DIREC        0x10 /* ディレクトリ */
#define FA_ARCH         0x20 /* アーカイブ属性 */
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

// コマンドラインで書庫を操作します
int WINAPI UnImp(const HWND _hwnd,LPCSTR _szCmdLine,LPSTR _szOutput,const DWORD _dwSize);
// バージョンを取得します
WORD WINAPI UnImpGetVersion(VOID);
// バージョンを取得します
WORD WINAPI UnImpGetSubVersion(VOID);
// 動作中であるか否かを取得します
BOOL WINAPI UnImpGetRunning(VOID);
// バックグラウンドモードであるか否かを取得します
BOOL WINAPI UnImpGetBackGroundMode(VOID);
// バックグラウンドモードの設定をします
BOOL WINAPI UnImpSetBackGroundMode(const BOOL _bBackGroundMode);
// カーソルを表示するか否かを取得します
BOOL WINAPI UnImpGetCursorMode(VOID);
// カーソルを表示するか否かを設定します
BOOL WINAPI UnImpSetCursorMode(const BOOL _CursorMode);
// カーソルの回転速度を取得します
WORD WINAPI UnImpGetCursorInterval(VOID);
// カーソルの回転速度を設定します
BOOL WINAPI UnImpSetCursorInterval(const WORD _Interval);
// 書庫の正当性検査を行います
BOOL WINAPI UnImpCheckArchive(LPCSTR _szFileName,const int _iMode);
// 書庫に含まれる格納ファイルの数を取得します
int WINAPI UnImpGetFileCount(LPCSTR _szArcFile);
// 指定したAPIが使用可能か判断します
BOOL WINAPI UnImpQueryFunctionList(const int _iFunction);
// DLLの設定
BOOL WINAPI UnImpConfigDialog(const HWND _hwnd,LPSTR _szOptionBuffer,const int _iMode);
// 書庫を開きます
HARC WINAPI UnImpOpenArchive(const HWND _hwnd,LPCSTR _szFileName,const DWORD _dwMode);
// 書庫を開きます
HARC WINAPI UnImpOpenArchive2(const HWND _hwnd,LPCSTR _szFileName,const DWORD _dwMode,LPCSTR _szOption);
// 書庫を閉じます
int WINAPI UnImpCloseArchive(HARC _harc);
// 最初の格納ファイル情報を取得します
int WINAPI UnImpFindFirst(HARC _harc,LPCSTR _szWildName,INDIVIDUALINFO *_lpSubInfo);
// 2番目以降の格納ファイル情報を取得します
int WINAPI UnImpFindNext(HARC _harc,INDIVIDUALINFO *_lpSubInfo);
// 指定したファイルを格納します
int WINAPI UnImpAdd(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
// 指定したファイルを展開します
int WINAPI UnImpExtract(HARC _harc,LPCSTR _szFileName,LPCSTR _szDirName,DWORD _dwMode);
// 指定したファイルを移動します
int WINAPI UnImpMove(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
// 指定したファイルを削除します
int WINAPI UnImpDelete(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
// 書庫ファイル名を取得します
int WINAPI UnImpGetArcFileName(HARC _harc,LPSTR _lpBuffer,const int _nSize);
// 書庫のファイルサイズを取得します
DWORD WINAPI UnImpGetArcFileSize(HARC _harc);
// 書庫のファイルサイズを取得します(64ビット)
BOOL WINAPI UnImpGetArcFileSizeEx(HARC _harc,LONGLONG *_lpllSize);
// 検索にマッチしたファイルサイズの合計を取得します
DWORD WINAPI UnImpGetArcOriginalSize(HARC _harc);
// 検索にマッチしたファイルサイズの合計を取得します(64ビット)
BOOL WINAPI UnImpGetArcOriginalSizeEx(HARC _harc,LONGLONG *_lpllSize);
// 検索にマッチした圧縮ファイルサイズの合計を取得します
DWORD WINAPI UnImpGetArcCompressedSize(HARC _harc);
// 検索にマッチした圧縮ファイルサイズの合計を取得します(64ビット)
BOOL WINAPI UnImpGetArcCompressedSizeEx(HARC _harc,LONGLONG *_lpllSize);
// 検索にマッチしたファイル全体の圧縮率を取得します
WORD WINAPI UnImpGetArcRatio(HARC _harc);
// 書庫の日付を取得します
WORD WINAPI UnImpGetArcDate(HARC _harc);
// 書庫の時間を取得します
WORD WINAPI UnImpGetArcTime(HARC _harc);
// 書庫の作成時間を取得します
DWORD WINAPI UnImpGetArcCreateTime(HARC _harc);
// 書庫の作成時間を取得します
BOOL WINAPI UnImpGetArcCreateTimeEx(HARC _harc,FILETIME *_lpftCreationTime);
// 書庫の参照時間を取得します
DWORD WINAPI UnImpGetArcAccessTime(HARC _harc);
// 書庫の参照時間を取得します
BOOL WINAPI UnImpGetArcAccessTimeEx(HARC _harc,FILETIME *_lpftLastAccessTime);
// 書庫の更新時間を取得します
DWORD WINAPI UnImpGetArcWriteTime(HARC _harc);
// 書庫の更新時間を取得します
BOOL WINAPI UnImpGetArcWriteTimeEx(HARC _harc,FILETIME *_lpftLastWriteTime);
// 書庫が作成されたOSの種類を取得します
UINT WINAPI UnImpGetArcOSType(HARC _harc);
// 書庫が自己展開書庫であるか否かを取得します
int WINAPI UnImpIsSFXFile(HARC _harc);
// UnImpFindFirst/Nextで読み込んだサイズを取得します
DWORD WINAPI UnImpGetArcReadSize(HARC _harc);
// UnImpFindFirst/Nextで読み込んだサイズを取得します(64ビット)
BOOL WINAPI UnImpGetArcReadSizeEx(HARC _harc,LONGLONG *_lpllSize);
// 格納ファイル名を取得します
int WINAPI UnImpGetFileName(HARC _harc,LPSTR _lpBuffer,const int _nSize);
// 格納ファイルの圧縮方法を取得します
int WINAPI UnImpGetMethod(HARC _harc,LPSTR _lpBuffer,const int _nSize);
// 格納ファイルのファイルサイズを取得します
DWORD WINAPI UnImpGetOriginalSize(HARC _harc);
// 格納ファイルのファイルサイズを取得します(64ビット)
BOOL WINAPI UnImpGetOriginalSizeEx(HARC _harc,LONGLONG *_lpllSize);
// 格納ファイルの圧縮ファイルサイズを取得します
DWORD WINAPI UnImpGetCompressedSize(HARC _harc);
// 格納ファイルの圧縮ファイルサイズを取得します(64ビット)
BOOL WINAPI UnImpGetCompressedSizeEx(HARC _harc,LONGLONG *_lpllSize);
// 格納ファイルの圧縮率を取得します
WORD WINAPI UnImpGetRatio(HARC _harc);
// 格納ファイルの日付を取得します
WORD WINAPI UnImpGetDate(HARC _harc);
// 格納ファイルの時間を取得します
WORD WINAPI UnImpGetTime(HARC _harc);
// 格納ファイルの作成時間を取得します
DWORD WINAPI UnImpGetCreateTime(HARC _harc);
// 格納ファイルの作成時間を取得します
BOOL WINAPI UnImpGetCreateTimeEx(HARC _harc,FILETIME *_lpftCreationTime);
// 格納ファイルの参照時間を取得します
DWORD WINAPI UnImpGetAccessTime(HARC _harc);
// 格納ファイルの参照時間を取得します
BOOL WINAPI UnImpGetAccessTimeEx(HARC _harc,FILETIME *_lpftLastAccessTime);
// 格納ファイルの更新時間を取得します
DWORD WINAPI UnImpGetWriteTime(HARC _harc);
// 格納ファイルの更新時間を取得します
BOOL WINAPI UnImpGetWriteTimeEx(HARC _harc,FILETIME *_lpftLastWriteTime);
// 格納ファイルのチェックサムを取得します
DWORD WINAPI UnImpGetCRC(HARC _harc);
// 格納ファイルの属性を取得します
int WINAPI UnImpGetAttribute(HARC _harc);
// 格納ファイルが作成されたOSの種類を取得します
UINT WINAPI UnImpGetOSType(HARC _harc);
// DLLが呼び出すコールバック関数を登録
BOOL WINAPI UnImpSetEnumMembersProc(EnumMembersProc _lpEnumProc);
// SetEnumMembersProcで登録したコールバック関数を解除
BOOL WINAPI UnImpClearEnumMembersProc(VOID);
// DLLが呼び出すコールバック関数を登録
BOOL WINAPI UnImpSetEnumMembersProc64(EnumMembersProc _lpEnumProc,DWORD _dwStructSize);
// SetEnumMembersProc64で登録したコールバック関数を解除
BOOL WINAPI UnImpClearEnumMembersProc64(VOID);
// DLLのメッセージ送信先ウィンドウを登録
BOOL WINAPI UnImpSetOwnerWindow(HWND _hwnd);
// SetOwnerWindowで登録したウィンドウを解除
BOOL WINAPI UnImpClearOwnerWindow(VOID);
// 展開状況などを受け取るコールバック関数を登録
BOOL WINAPI UnImpSetOwnerWindowEx(HWND _hwnd,ARCHIVERPROC _lpArcProc);
// SetOwnerWindowExで登録したコールバック関数を解除
BOOL WINAPI UnImpKillOwnerWindowEx(HWND _hwnd);
// 展開状況などを受け取るコールバック関数を登録
BOOL WINAPI UnImpSetOwnerWindowEx64(HWND _hwnd,ARCHIVERPROC _lpArcProc,DWORD _dwStructSize);
// SetOwnerWindowEx64で登録したコールバック関数を解除
BOOL WINAPI UnImpKillOwnerWindowEx64(HWND _hwnd);
// デバッグ用API
void WINAPI UnImpSetDebugMode(BOOL _bMode);

// 古いバージョンのAPI
int WINAPI Unimp(const HWND _hwnd,LPCSTR _szCmdLine,LPSTR _szOutput,const DWORD _dwSize);
WORD WINAPI UnimpGetVersion(VOID);
WORD WINAPI UnimpGetSubVersion(VOID);
BOOL WINAPI UnimpGetRunning(VOID);
BOOL WINAPI UnimpGetBackGroundMode(VOID);
BOOL WINAPI UnimpSetBackGroundMode(const BOOL _bBackGroundMode);
BOOL WINAPI UnimpGetCursorMode(VOID);
BOOL WINAPI UnimpSetCursorMode(const BOOL _CursorMode);
WORD WINAPI UnimpGetCursorInterval(VOID);
BOOL WINAPI UnimpSetCursorInterval(const WORD _Interval);
BOOL WINAPI UnimpCheckArchive(LPCSTR _szFileName,const int _iMode);
int WINAPI UnimpGetFileCount(LPCSTR _szArcFile);
BOOL WINAPI UnimpQueryFunctionList(const int _iFunction);
BOOL WINAPI UnimpConfigDialog(const HWND _hwnd,LPSTR _szOptionBuffer,const int _iMode);
HARC WINAPI UnimpOpenArchive(const HWND _hwnd,LPCSTR _szFileName,const DWORD _dwMode);
HARC WINAPI UnimpOpenArchive2(const HWND _hwnd,LPCSTR _szFileName,const DWORD _dwMode,LPCSTR _szOption);
int WINAPI UnimpCloseArchive(HARC _harc);
int WINAPI UnimpFindFirst(HARC _harc,LPCSTR _szWildName,INDIVIDUALINFO *lpSubInfo);
int WINAPI UnimpFindNext(HARC _harc,INDIVIDUALINFO *_lpSubInfo);
int WINAPI UnimpAdd(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
int WINAPI UnimpExtract(HARC _harc,LPCSTR _szFileName,LPCSTR _szDirName,DWORD _dwMode);
int WINAPI UnimpMove(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
int WINAPI UnimpDelete(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
int WINAPI UnimpGetArcFileName(HARC _harc,LPSTR _lpBuffer,const int _nSize);
DWORD WINAPI UnimpGetArcFileSize(HARC _harc);
BOOL WINAPI UnimpGetArcFileSizeEx(HARC _harc,LONGLONG *_lpllSize);
DWORD WINAPI UnimpGetArcOriginalSize(HARC _harc);
BOOL WINAPI UnimpGetArcOriginalSizeEx(HARC _harc,LONGLONG *_lpllSize);
DWORD WINAPI UnimpGetArcCompressedSize(HARC _harc);
BOOL WINAPI UnimpGetArcCompressedSizeEx(HARC _harc,LONGLONG *_lpllSize);
WORD WINAPI UnimpGetArcRatio(HARC _harc);
WORD WINAPI UnimpGetArcDate(HARC _harc);
WORD WINAPI UnimpGetArcTime(HARC _harc);
BOOL WINAPI UnimpGetArcCreateTimeEx(HARC _harc,FILETIME *_lpftCreationTime);
BOOL WINAPI UnimpGetArcAccessTimeEx(HARC _harc,FILETIME *_lpftLastAccessTime);
BOOL WINAPI UnimpGetArcWriteTimeEx(HARC _harc,FILETIME *_lpftLastWriteTime);
UINT WINAPI UnimpGetArcOSType(HARC _harc);
int WINAPI UnimpIsSFXFile(HARC _harc);
DWORD WINAPI UnimpGetArcReadSize(HARC _harc);
BOOL WINAPI UnimpGetArcReadSizeEx(HARC _harc,LONGLONG *_lpllSize);
int WINAPI UnimpGetFileName(HARC _harc,LPSTR _lpBuffer,const int _nSize);
int WINAPI UnimpGetMethod(HARC _harc,LPSTR _lpBuffer,const int _nSize);
DWORD WINAPI UnimpGetOriginalSize(HARC _harc);
BOOL WINAPI UnimpGetOriginalSizeEx(HARC _harc,LONGLONG *_lpllSize);
DWORD WINAPI UnimpGetCompressedSize(HARC _harc);
BOOL WINAPI UnimpGetCompressedSizeEx(HARC _harc,LONGLONG *_lpllSize);
WORD WINAPI UnimpGetRatio(HARC _harc);
WORD WINAPI UnimpGetDate(HARC _harc);
WORD WINAPI UnimpGetTime(HARC _harc);
DWORD WINAPI UnimpGetCreateTime(HARC _harc);
BOOL WINAPI UnimpGetCreateTimeEx(HARC _harc,FILETIME *_lpftCreationTime);
DWORD WINAPI UnimpGetAccessTime(HARC _harc);
BOOL WINAPI UnimpGetAccessTimeEx(HARC _harc,FILETIME *_lpftLastAccessTime);
DWORD WINAPI UnimpGetWriteTime(HARC _harc);
BOOL WINAPI UnimpGetWriteTimeEx(HARC _harc,FILETIME *_lpftLastWriteTime);
DWORD WINAPI UnimpGetCRC(HARC _harc);
int WINAPI UnimpGetAttribute(HARC _harc);
UINT WINAPI UnimpGetOSType(HARC _harc);
BOOL WINAPI UnimpSetEnumMembersProc(EnumMembersProc _lpEnumProc);
BOOL WINAPI UnimpClearEnumMembersProc(VOID);
BOOL WINAPI UnimpSetEnumMembersProc64(EnumMembersProc _lpEnumProc,DWORD _dwStructSize);
BOOL WINAPI UnimpClearEnumMembersProc64(VOID);
BOOL WINAPI UnimpSetOwnerWindow(HWND _hwnd);
BOOL WINAPI UnimpClearOwnerWindow(VOID);
BOOL WINAPI UnimpSetOwnerWindowEx(HWND _hwnd,ARCHIVERPROC _lpArcProc);
BOOL WINAPI UnimpKillOwnerWindowEx(HWND _hwnd);
BOOL WINAPI UnimpSetOwnerWindowEx64(HWND _hwnd,ARCHIVERPROC _lpArcProc,DWORD _dwStructSize);
BOOL WINAPI UnimpKillOwnerWindowEx64(HWND _hwnd);

#if defined(__cplusplus)
}
#endif

#endif
