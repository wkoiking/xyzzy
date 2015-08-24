/*--------------------------------------------------------------------------*/
#include <windows.h>
#pragma hdrstop
/*----------------------------------------------------------------------------
 *   �ȉ���, �X�^�e�B�b�N�ł̃����^�C�����C�u�������g�p���� DLL �ɂ�����
 *   �������Ǘ��̏d�v�Ȓ��ӎ����ł��B
 *
 *   �p�����[�^��֐��̖߂�l�Ƃ��� String �I�u�W�F�N�g�܂��̓l�X�g���� String
 *   ���܂ލ\����/�N���X��n���֐��� DLL ���G�N�X�|�[�g����ꍇ, DLL �v���W�F�N
 *   �g�� DLL ���g�p���邷�ׂĂ� EXE �v���W�F�N�g�̗����Ƀ��C�u���� MEMMGR.LIB 
 *   ��ǉ����Ȃ���΂Ȃ�܂���B�܂�, DLL ����G�N�X�|�[�g���ꂽ TObject ����
 *   �h�����Ă��Ȃ��N���X�̃I�u�W�F�N�g�� new �܂��� delete ���� DLL ���g�p
 *   ����ꍇ�ɂ�,  MEMMGR.LIB ���g�p����K�v������܂��BMEMMGR.LIB ��
 *   �v���W�F�N�g�ɒǉ����邱�Ƃɂ��, DLL �ƌĂяo������ EXE �� DLL ��
 *   �������}�l�[�W���Ƃ��� BORLNDMM.DLL ���g���悤�ɕύX����܂��B���̏ꍇ, 
 *   �t�@�C�� BORLNDMM.DLL �� DLL �ƈꏏ�ɔz�z����K�v������܂��B
 *
 *   BORLNDMM.DLL �̎g�p�������ɂ�, "char *" �܂��� ShortString �p�����[�^��
 *   �g���ĕ�����̂������s���Ă��������B
 *
 *   �_�C�i�~�b�N RTL ���g�p����ꍇ�ɂ�, �Öق̂����ɍs����̂�,
 *   �����I�� MEMMGR.LIB �ǉ�����K�v�͂���܂���B
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
/* 1. �R�}���h�E���C���ł̎��s */
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
/* 2. �o�[�W�����̎擾 */
WORD WINAPI AishGetVersion(VOID)
{
	return (WORD)AISH_VERSION;
}
/*--------------------------------------------------------------------------*/
/* 3. �J�[�\���̉�]���x�̎擾 */
WORD WINAPI AishGetCursorInterval(VOID)
{
	return GStatus.CursorInterval;
}
/*--------------------------------------------------------------------------*/
/* 4. �J�[�\���̉�]���x�̐ݒ� */
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
/* 5. �o�b�N�O���E���h�E���[�h�ł��邩�ǂ����̎擾 */
BOOL WINAPI AishGetBackGroundMode(VOID)
{
	return FALSE; // ��ɔ�o�b�N�O���E���h�E���[�h
}
/*--------------------------------------------------------------------------*/
/* 6. �o�b�N�O���E���h�E���[�h�ɂ��邩�ݒ� */
BOOL WINAPI AishSetBackGroundMode(const BOOL _bBackGroundMode)
{
	BOOL ret;

	if(is_running()) {
		// ���쒆�Ȃ̂ŕύX�ł��Ȃ�
		ret = FALSE;
	} else {
		switch(_bBackGroundMode) {
		case TRUE: // �o�b�N�O���E���h�E���[�h�ւ̕ύX�v��
			ret = FALSE;  // �o�b�N�O���E���h�E���[�h�ɂ͂ł��Ȃ�
			break;
		case FALSE: // ��o�b�N�O���E���h�E���[�h�ւ̕ύX�v��
			ret = TRUE;   // ��o�b�N�O���E���h�E���[�h�ɂ͂ł���
			break;
		default:    // �����̒l���s��
			ret = FALSE;
			break;
		}
	}

	return ret;
}
/*--------------------------------------------------------------------------*/
/* 7. �J�[�\����\�����郂�[�h�����擾 */
BOOL WINAPI AishGetCursorMode(VOID)
{
	return GStatus.CursorDisplayMode;
}
/*--------------------------------------------------------------------------*/
/* 8. �J�[�\����\�����郂�[�h����ݒ� */
BOOL WINAPI AishSetCursorMode(const BOOL _bCursorMode)
{
	BOOL ret;

	if(is_running()) {
		// ���쒆�Ȃ̂ŕύX�ł��Ȃ�
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
/* 10. DLL �����쒆����Ԃ� */
BOOL WINAPI AishGetRunning(void)
{
	return is_running();
}
/*--------------------------------------------------------------------------*/
/* 11. �w�肳�ꂽ�t�@�C���� ISH �f�[�^�����݂��AISH �f�[�^�Ƃ��Đ�������
 * �ǂ����𒲂ׂ�B
 */
/* !!! ���ӁF�@���̃o�[�W�����ł̓t�@�C�������݂��邩��Ԃ��̂� !!! */
BOOL WINAPI AishCheckArchive(LPCSTR _szFileName,
	const int _iMode)
{
	BOOL exists;

	++GStatus.RunningFlag;

	/* !!! ���ꂩ���� */
	exists = (access(_szFileName, 0) == 0);

	--GStatus.RunningFlag;

	return exists;
}
/*--------------------------------------------------------------------------*/
/* 18. �w�肳�ꂽ API ���g�p�\����Ԃ� */
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
