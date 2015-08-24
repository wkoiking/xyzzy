/*--------------------------------------------------------------------------*/
#include <windows.h>
#pragma hdrstop
/*----------------------------------------------------------------------------
 *   以下は, スタティック版のランタイムライブラリを使用する DLL における
 *   メモリ管理の重要な注意事項です。
 *
 *   パラメータや関数の戻り値として String オブジェクトまたはネストした String
 *   を含む構造体/クラスを渡す関数を DLL がエクスポートする場合, DLL プロジェク
 *   トと DLL を使用するすべての EXE プロジェクトの両方にライブラリ MEMMGR.LIB 
 *   を追加しなければなりません。また, DLL からエクスポートされた TObject から
 *   派生していないクラスのオブジェクトを new または delete する DLL を使用
 *   する場合にも,  MEMMGR.LIB を使用する必要があります。MEMMGR.LIB を
 *   プロジェクトに追加することにより, DLL と呼び出し側の EXE の DLL が
 *   メモリマネージャとして BORLNDMM.DLL を使うように変更されます。この場合, 
 *   ファイル BORLNDMM.DLL を DLL と一緒に配布する必要があります。
 *
 *   BORLNDMM.DLL の使用を避けるには, "char *" または ShortString パラメータを
 *   使って文字列のやり取りを行ってください。
 *
 *   ダイナミック RTL を使用する場合には, 暗黙のうちに行われるので,
 *   明示的に MEMMGR.LIB 追加する必要はありません。
---------------------------------------------------------------------------*/
#include <cstdlib>
#include <cstring>
#include <io.h>
#include "aish32.h"

#include "aish.h"
#include "ai_fio.h"

#include "wincur.h"

using namespace std;
/*--------------------------------------------------------------------------*/
struct Status {
	HINSTANCE HinstDll;
	int RunningFlag;
	bool CursorDisplayMode;
	WORD CursorInterval;

	Status()
	{
		HinstDll = 0;
		RunningFlag = 0;
		CursorDisplayMode = 0;
		CursorInterval = 80; // 80 ms
	}
};
/*--------------------------------------------------------------------------*/
static Status GStatus;
/*--------------------------------------------------------------------------*/
inline static bool is_running()
{
	return GStatus.RunningFlag != 0;
}
/*--------------------------------------------------------------------------*/
static const char *
get_a_parameter(char *buf, const char *str)
{
	int in_quote;
	int i;
	int quit;

	in_quote = 0;
	i = 0;
	buf[i] = 0;
	quit = 0;
	while(!quit && *str) {
		if(*str == ' ') {
			if(in_quote) {
				buf[i] = *str;
				++i;
				buf[i] = '\0';
				++str;
			} else {
				if(*(str + 1)) {
					if(*(str + 1) != ' ') {
						quit = 1;
					}
				} else {
					quit = 1;
				}
				++str;
			}
		} else if(*str == '\"') {
			in_quote = !in_quote;
			++str;
		} else {
			buf[i] = *str;
			++i;
			buf[i] = '\0';
			++str;
		}
	}

	return str;
}
/*--------------------------------------------------------------------------*/
static int
cmdline2array(char *argv[], const char *cmdline)
{
	const char *s;
	char buf[8192];
	int argc;

	s = cmdline;
	argc = 0;
	while(*s) {
		s = get_a_parameter(buf, s);
		if(strlen(buf) > 0U) {
			argv[argc] = strdup(buf);
			++argc;
		}
	}
	argv[argc] = NULL;

	return argc;
}
/*--------------------------------------------------------------------------*/
/* 1. コマンド・ラインでの実行 */
int WINAPI
Aish(const HWND _hwnd, LPCSTR _szCmdLine,
	  LPSTR _szOutput, const DWORD _dwSize)
{
	char *argv[4096];
	int argc;
	int ret;

	++GStatus.RunningFlag;

	shimaden::TWinCursor winCursor(_hwnd, 0, true);
	if(GStatus.CursorDisplayMode) {
		winCursor.SetCursor(IDC_WAIT);
	}

	Gstdout_buf = _szOutput;
	*Gstdout_buf = '\0';
	Gstdout_buf_pos = 0;
	Gstdout_buf_size = _dwSize;

	argv[0] = "AISH32.DLL";
	argc = cmdline2array(argv + 1, _szCmdLine) + 1;

	ret = aish_main(argc, argv);
	if(ret > 0)
		ret += 0x8000;

	--GStatus.RunningFlag;

	return ret;
}
/*--------------------------------------------------------------------------*/
/* 2. バージョンの取得 */
WORD WINAPI AishGetVersion(VOID)
{
	return (WORD)AISH_VERSION;
}
/*--------------------------------------------------------------------------*/
/* 3. カーソルの回転速度の取得 */
WORD WINAPI AishGetCursorInterval(VOID)
{
	return GStatus.CursorInterval;
}
/*--------------------------------------------------------------------------*/
/* 4. カーソルの回転速度の設定 */
BOOL WINAPI AishSetCursorInterval(const WORD _Interval)
{
	BOOL ret;

	if(is_running()) {
		ret = FALSE;
	} else {
		GStatus.CursorInterval = _Interval;
		ret = TRUE;
	}

	return ret;
}
/*--------------------------------------------------------------------------*/
/* 5. バックグラウンド・モードであるかどうかの取得 */
BOOL WINAPI AishGetBackGroundMode(VOID)
{
	return FALSE; // 常に非バックグラウンド・モード
}
/*--------------------------------------------------------------------------*/
/* 6. バックグラウンド・モードにするか設定 */
BOOL WINAPI AishSetBackGroundMode(const BOOL _bBackGroundMode)
{
	BOOL ret;

	if(is_running()) {
		// 動作中なので変更できない
		ret = FALSE;
	} else {
		switch(_bBackGroundMode) {
		case TRUE: // バックグラウンド・モードへの変更要求
			ret = FALSE;  // バックグラウンド・モードにはできない
			break;
		case FALSE: // 非バックグラウンド・モードへの変更要求
			ret = TRUE;   // 非バックグラウンド・モードにはできる
			break;
		default:    // 引数の値が不正
			ret = FALSE;
			break;
		}
	}

	return ret;
}
/*--------------------------------------------------------------------------*/
/* 7. カーソルを表示するモードかを取得 */
BOOL WINAPI AishGetCursorMode(VOID)
{
	return GStatus.CursorDisplayMode;
}
/*--------------------------------------------------------------------------*/
/* 8. カーソルを表示するモードかを設定 */
BOOL WINAPI AishSetCursorMode(const BOOL _bCursorMode)
{
	BOOL ret;

	if(is_running()) {
		// 動作中なので変更できない
		ret = FALSE;
	} else {
		switch(_bCursorMode) {
		case TRUE:
			ret = GStatus.CursorDisplayMode = true;
			break;
		case FALSE:
			ret = GStatus.CursorDisplayMode = false;
			break;
		default:
			ret = FALSE;
			break;
		}
	}

	return ret;
}
/*--------------------------------------------------------------------------*/
/* 10. DLL が動作中かを返す */
BOOL WINAPI AishGetRunning(void)
{
	return is_running();
}
/*--------------------------------------------------------------------------*/
/* 11. 指定されたファイルに ISH データが存在し、ISH データとして正しいか
 * どうかを調べる。
 */
/* !!! 注意：　このバージョンではファイルが存在するかを返すのみ !!! */
BOOL WINAPI AishCheckArchive(LPCSTR _szFileName,
	const int _iMode)
{
	BOOL exists;

	++GStatus.RunningFlag;

	/* !!! これから作る */
	exists = (access(_szFileName, 0) == 0);

	--GStatus.RunningFlag;

	return exists;
}
/*--------------------------------------------------------------------------*/
/* 18. 指定された API が使用可能かを返す */
BOOL WINAPI AishQueryFunctionList(const int _iFunction)
{
	BOOL ret;

	switch(_iFunction) {
	case ISARC:                      //  1. Aish()
	case ISARC_GET_VERSION:          //  2. AishGetVersion()
	case ISARC_GET_CURSOR_INTERVAL:  //  3. AishGetCursorInterval()
	case ISARC_SET_CURSOR_INTERVAL:  //  4. AishSetCursorInterval()
	case ISARC_GET_BACK_GROUND_MODE: //  5. AishGetBackGroundMode()
	case ISARC_SET_BACK_GROUND_MODE: //  6. AishSetBackGroundMode()
	case ISARC_GET_CURSOR_MODE:      //  7. AishGetCursorMode()
	case ISARC_SET_CURSOR_MODE:      //  8. AishSetCursorMode()
	case ISARC_GET_RUNNING:          // 10. AishGetRunning()
	case ISARC_CHECK_ARCHIVE:        // 16. AishCheckArchive()
	case ISARC_QUERY_FUNCTION_LIST:  // 18. AishQueryFunctionList()
		ret = TRUE;
		break;
	default:
		ret = FALSE;
		break;
	}

	return ret;
}
/*--------------------------------------------------------------------------*/
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void *reserved)
{
	BOOL ret;

	if(reason == DLL_PROCESS_ATTACH) {
		GStatus.HinstDll = hinst;
		ret = TRUE;
	} else {
		ret = TRUE;
	}

	return ret;
}
/*--------------------------------------------------------------------------*/
