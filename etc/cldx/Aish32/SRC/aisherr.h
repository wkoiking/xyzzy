/* main() in aish.cpp が返す戻り値 */

#if !defined(WDN_AISHERR_H)
#define WDN_AISHERR_H

// 警告 - 当該ファイルに対しての処理をスキップするだけで、実行を中止す
//        ることはない
//
// ※ #define 後部の「//」によるコメントは、覚え書き程度のものです。
// 　 より正確な解説は、AISHERR.txt をご覧ください。
//
#define AISH_DISK_SPACE        0x05 // 復元のためのディスクの空きが足りません
#define AISH_READ_ONLY         0x06 // 復元先のファイルはリード・オンリーです
#define AISH_USER_SKIP         0x07 // ユーザによって復元をスキップされました
#define AISH_UNKNOWN_TYPE      0x08 // 格納ファイルの種類がMS-DOSで扱えない形式
#define AISH_METHOD            0x09 // AISH32.DLL で扱えない格納モードでの格納
#define AISH_PASSWORD_FILE     0x0A // 格納ファイルにパスワードがかかっている
#define AISH_ERROR_VERSION     0x0B // 格納ファイルの形式が AISH32.DLL で未対応
#define AISH_FILE_CRC          0x0C // 格納ファイルのチェック・サムが不正
#define AISH_FILE_OPEN         0x0D // 復元時にファイルを開けませんでした
#define AISH_MORE_FRESH        0x0E // より新しいファイルが復元先に存在している
#define AISH_NOT_EXIST         0x0F // ファイルは復元先に存在していません
#define AISH_ALREADY_EXIST     0x10 // ファイルは復元先にすでに存在しています
#define AISH_TOO_MANY_FILES    0x11 // 格納ファイル数が限界を超えている

// エラー - 致命的なエラーで、その時点で実行を中止する
//
// ※ #define 後部の「//」によるコメントは、覚え書き程度のものです。
// 　 より正確な解説は、AISHERR.txt をご覧ください。
//
#define AISH_MAKEDIRECTORY     0x12 // ディレクトリが制作できません
#define AISH_CANNOT_WRITE      0x13 // 復元中に書き込みエラーが生じました
#define AISH_HUFFMAN_CODE      0x14 // ISH file のハフマン・コードが壊れてる
#define AISH_COMMENT_HEADER    0x15 // ISH file のコメント・コードが壊れてる
#define AISH_HEADER_CRC        0x16 // ISH file のヘッダの check sum が壊れてる
#define AISH_HEADER_BROKEN     0x17 // ISH file のヘッダが壊れている
#define AISH_ARC_FILE_OPEN     0x18 // ISH file を開くことができません
#define AISH_NOT_ARC_FILE      0x19 // 指定したファイルは ISH file ではない
#define AISH_CANNOT_READ       0x1A // ISH file の読み込み時にエラーが発生した
#define AISH_FILE_STYLE        0x1B // 指定されたファイルは ISH file ではない
#define AISH_COMMAND_NAME      0x1C // コマンド指定が間違っています
#define AISH_MORE_HEAP_MEMORY  0x1D // 作業用のヒープ・メモリが不足しています
#define AISH_ENOUGH_MEMORY     0x1E // グローバル・メモリが不足しています
#define AISH_ALREADY_RUNNING   0x1F // すでに他のプロセスが AISH32.DLL を使用中
#define AISH_USER_CANCEL       0x20 // ユーザによって復元を中止されました
#define AISH_HARC_ISNOT_OPENED 0x21 // まだ AIshOpenArchive() されてないのに
#define AISH_NOT_SEARCH_MODE   0x22 // 不正な AIshFindNext()/AIshGetFileName()
#define AISH_NOT_SUPPORT       0x23 // AISH32.DLL 未サポートの API 呼び出し
#define AISH_TIME_STAMP        0x24 // 日付/時間の形式が間違えています
#define AISH_TMP_OPEN          0x25 // 作業ファイルが開けません
#define AISH_LONG_FILE_NAME    0x26 // ディレクトリのパスが長すぎます
#define AISH_ARC_READ_ONLY     0x27 // 書き込み専用の ISH file は操作できない
#define AISH_SAME_NAME_FILE    0x28 // すでに同名ファイルが ISH に格納されてる
#define AISH_NOT_FIND_ARC_FILE 0x29 // 指定された dir に ISH file はなかった
#define AISH_RESPONSE_READ     0x2A // レスポンス・ファイル読み込みエラー
#define AISH_NOT_FILENAME      0x2B // ファイル名の指定が必要です
#define AISH_TMP_COPY          0x2C // Tmp ファイルの書庫への書き戻しできず

/* AISH 独自の警告コード（処理を中断しない） */
//                             0x81

/* AISH 独自のエラー・コード（処理を中断する） */
#define AISH_CANT_SPLIT        0xA1 // エンコード時に分割できない
#define AISH_CANT_READ_INFILE  0xA2 // encode するための file が開けません
#define AISH_TOO_MANY_VOLUMES  0xA3 // 分割数の指定が多すぎる


#endif /* aisherr.h */
