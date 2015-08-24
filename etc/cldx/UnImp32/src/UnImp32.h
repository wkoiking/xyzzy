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

// �i�[�t�@�C���̏��
typedef struct
{
    DWORD dwOriginalSize;                       // �t�@�C���T�C�Y
    DWORD dwCompressedSize;                     // ���k�t�@�C���T�C�Y
    DWORD dwCRC;                                // �`�F�b�N�T��
    UINT uFlag;                                 // ��������
    UINT uOSType;                               // OS�̎��
    WORD wRatio;                                // ���k��
    WORD wDate;                                 // ���t
    WORD wTime;                                 // ����
    char szFileName[FNAME_MAX32 + 1];           // �t�@�C����
    char dummy1[3];
    char szAttribute[8];                        // ����
    char szMode[8];                             // ���k���@
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
#define ERROR_DISK_SPACE        0x8005 /* �f�B�X�N�̋󂫗e�ʂ�����Ȃ� */
#define ERROR_READ_ONLY         0x8006 /* �ǂݎ���p�����̃t�@�C�������� */
#define ERROR_USER_SKIP         0x8007 /* ���[�U�ɂ���ăX�L�b�v���ꂽ */
#define ERROR_UNKNOWN_TYPE      0x8008 /* ���ꑮ���̃t�@�C�������� */
#define ERROR_METHOD            0x8009 /* �Ή����Ă��Ȃ����\�b�h */
#define ERROR_PASSWORD_FILE     0x800A /* �p�X���[�h���������Ȃ� */
#define ERROR_VERSION           0x800B /* �Ή����Ă��Ȃ��o�[�W���� */
#define ERROR_FILE_CRC          0x800C /* ���k�C���[�W���s�� */
#define ERROR_FILE_OPEN         0x800D /* �t�@�C�����J���Ȃ����� */
#define ERROR_MORE_FRESH        0x800E /* �㏑���Ɏ��s */
#define ERROR_NOT_EXIST         0x800F /* �t�@�C�������݂��Ȃ� */
#define ERROR_ALREADY_EXIST     0x8010 /* �t�@�C�������ɑ��݂��Ă��� */
#define ERROR_TOO_MANY_FILES    0x8011 /* �i�[����t�@�C������������ */

/* ERROR */
#define ERROR_MAKEDIRECTORY     0x8012 /* �f�B���N�g�����쐬�ł��Ȃ� */
#define ERROR_CANNOT_WRITE      0x8013 /* �t�@�C���ւ̏������݂Ɏ��s */
#define ERROR_HUFFMAN_CODE      0x8014 /* ���ɂ̔j���ɂ��n�t�}�����k�p�e�[�u�����������ł��Ȃ����� */
#define ERROR_COMMENT_HEADER    0x8015 /* �R�����g�w�b�_���s�� */
#define ERROR_HEADER_CRC        0x8016 /* �w�b�_���s�� */
#define ERROR_HEADER_BROKEN     0x8017 /* �w�b�_�����Ă��� */
#define ERROR_ARC_FILE_OPEN     0x8018 /* ���ɂ��J���Ȃ� */
#define ERROR_NOT_ARC_FILE      0x8019 /* ���ɂł͂Ȃ� */
#define ERROR_CANNOT_READ       0x801A /* �t�@�C������f�[�^��ǂݍ��߂Ȃ� */
#define ERROR_FILE_STYLE        0x801B /* �w�肳�ꂽ���ɂ����ɂƂ��ĔF���ł��Ȃ����� */
#define ERROR_COMMAND_NAME      0x801C /* �s���Ȗ��� */
#define ERROR_MORE_HEAP_MEMORY  0x801D /* ��Ɨp�̃������̊m�ۂɎ��s */
#define ERROR_ENOUGH_MEMORY     0x801E /* �o�b�t�@�������̊m�ۂɎ��s */
#if !defined(ERROR_ALREADY_RUNNING)
#define ERROR_ALREADY_RUNNING   0x801F /* DLL�����쒆 */
#endif
#define ERROR_USER_CANCEL       0x8020 /* ���[�U�ɂ��L�����Z�����ꂽ */
#define ERROR_HARC_ISNOT_OPENED 0x8021 /* �L���ȏ��Ƀn���h���ł͂Ȃ� */
#define ERROR_NOT_SEARCH_MODE   0x8022 /* ���Ƀn���h���̌����Ώۂ̃t�@�C�����w�肳��Ă��Ȃ�*/
#define ERROR_NOT_SUPPORT       0x8023 /* DLL�ŃT�|�[�g����Ă��Ȃ�API���g�p����Ă��� */
#define ERROR_TIME_STAMP        0x8024 /* �^�C���X�^���v���s�� */
#define ERROR_TMP_OPEN          0x8025 /* �ꎞ�t�@�C�����J���Ȃ� */
#define ERROR_LONG_FILE_NAME    0x8026 /* ���ɂɊi�[�ł��Ȃ������̃t�@�C���� */
#define ERROR_ARC_READ_ONLY     0x8027 /* ���ɂ��ǂ݂Ƃ��p�ł��� */
#define ERROR_SAME_NAME_FILE    0x8028 /* ���ɂɓ����̃t�@�C�����Ŋi�[���悤�Ƃ��Ă��� */
#define ERROR_NOT_FIND_ARC_FILE 0x8029 /* ���ɂ�������Ȃ� */
#define ERROR_RESPONSE_READ     0x802A /* ���X�|���X�t�@�C�����ǂݍ��߂Ȃ� */
#define ERROR_NOT_FILENAME      0x802B /* �Ώۃt�@�C�����w�肳��Ă��Ȃ� */
#define ERROR_TMP_COPY          0x802C /* �ꎞ�t�@�C�����R�s�[�ł��Ȃ� */
#define ERROR_EOF               0x802D /* �\�����Ȃ�EOF */
#define ERROR_ADD_TO_LARC       0x802E /* �w�肳�ꂽ���ɂ����ɑ���̋�����Ă��Ȃ����� */
#define ERROR_TMP_BACK_SPACE    0x802F /* ��Ɨp�̏��ɂ��R�s�[����ۂɃf�B�X�N�e�ʂ�����Ȃ� */
#define ERROR_SHARING           0x8030 /* �t�@�C���ɃA�N�Z�X�ł��Ȃ� */
#define ERROR_NOT_FIND_FILE     0x8031 /* �t�@�C����������Ȃ� */
#define ERROR_LOG_FILE          0x8032 /* ���O�t�@�C���ւ̏������݂Ɏ��s */
#define	ERROR_NO_DEVICE         0x8033 /* �f�o�C�X(�h���C�u)�ւ̃A�N�Z�X�Ɏ��s */
#define ERROR_GET_ATTRIBUTES    0x8034 /* �����̎擾�Ɏ��s */
#define ERROR_SET_ATTRIBUTES    0x8035 /* �����̕ۑ��Ɏ��s */
#define ERROR_GET_INFORMATION   0x8036 /* �f�B�X�N�󂫗e�ʂ̎擾��t�@�C���̍X�V�����̎擾�Ɏ��s */
#define ERROR_GET_POINT         0x8037 /* �t�@�C���|�C���^�̎擾�Ɏ��s */
#define ERROR_SET_POINT         0x8038 /* �t�@�C���|�C���^�̐ݒ�Ɏ��s */
#define ERROR_CONVERT_TIME      0x8039 /* ���Ԃ̕ϊ��Ɏ��s */
#define ERROR_GET_TIME          0x803A /* �t�@�C���X�^���v�̎擾�Ɏ��s */
#define ERROR_SET_TIME          0x803B /* �t�@�C���X�^���v�̕ۑ��Ɏ��s */
#define ERROR_CLOSE_FILE        0x803C /* �t�@�C�������Ȃ� */
#define ERROR_HEAP_MEMORY       0x803D /* ������������ł��Ȃ� */
#define ERROR_HANDLE            0x803E /* �����n���h���̉���Ɏ��s */
#define ERROR_TIME_STAMP_RANGE  0x803F /* �s���ȃ^�C���X�^���v */
#define ERROR_MAKE_ARCHIVE      0x8040 /* ���ɂ��쐬�ł��Ȃ� */
#define ERROR_NOT_CONFIRM_NAME  0x8041 /* ���ɂɊi�[���ꂽ�t�@�C�����̒����Ǝ��ۂ̃t�@�C�����̒�������v���Ȃ� */
#define ERROR_UNEXPECTED_EOF    0x8042 /* ���ɂɊi�[���ꂽ�T�C�Y�̃w�b�_�̓ǂݍ��݂Ɏ��s */
#define ERROR_INVALID_END_MARK  0x8043 /* ���ɂɕs���ȃG���h�}�[�N�����݂��� */
#define ERROR_INVOLVED_LZH      0x8044 /* �l�X�g�������ɂ��O������J�����Ƃ��� */
#define ERROR_NO_END_MARK       0x8045 /* ���ɂɃG���h�}�[�N���L�^����Ă��Ȃ� */
#define ERROR_HDR_INVALID_SIZE  0x8046 /* �i�[���ꂽ�w�b�_�T�C�Y���s�� */
#define ERROR_UNKNOWN_LEVEL     0x8047 /* �Ή����Ă��Ȃ����x�� */
#define ERROR_BROKEN_DATA       0x8048 /* ���k�C���[�W�����Ă��� */
#define ERROR_INVALID_PATH      0x8049 /* �s���ȃp�X */
#define ERROR_TOO_BIG           0x804A /* �t�@�C���T�C�Y���t�@�C���V�X�e���őΉ����Ă��Ȃ� */
#define ERROR_UNKNOWN           0x8FFF /* �s���ȃG���[ */
#define ERROR_END               ERROR_UNKNOWN

#endif

#if !defined(CHECKARCHIVE_RAPID)
#define CHECKARCHIVE_RAPID       0 /* �Ȉ�(�ŏ���3�w�b�_�܂�) */
#define CHECKARCHIVE_BASIC       1 /* �W��(�S�Ẵw�b�_) */
#define CHECKARCHIVE_FULLCRC     2 /* ���S(�i�[�t�@�C����CRC�`�F�b�N) */

/* �ȉ��̃t���O�͏�L�Ƒg�ݍ��킹�Ďg�p */
#define CHECKARCHIVE_RECOVERY    4 /* �j���w�b�_��ǂݔ�΂��ď��� */
#define CHECKARCHIVE_SFX         8 /* SFX���ǂ�����Ԃ� */
#define CHECKARCHIVE_ALL        16 /* �t�@�C���̍Ō�܂Ō������� */
#define CHECKARCHIVE_ENDDATA    32 /* ���ɂ����̗]��f�[�^������ */
#endif

#if !defined(UNPACK_CONFIG_MODE)
#define	UNPACK_CONFIG_MODE       1 /* �𓀌n�̐ݒ� */
#define PACK_CONFIG_MODE         2 /* ���k�n�̐ݒ� */
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

#define ARCEXTRACT_BEGIN        0 /* �Y���t�@�C���̏����̊J�n */
#define ARCEXTRACT_INPROCESS    1 /* �Y���t�@�C���̓W�J�� */
#define ARCEXTRACT_END          2 /* �����I���A�֘A���������J�� */
#define ARCEXTRACT_OPEN         3 /* �Y�����ɂ̏����̊J�n */
#define ARCEXTRACT_COPY         4 /* ���[�N�t�@�C���̏����߂� */
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
#define FA_RDONLY       0x01 /* �������ݕی쑮�� */
#define FA_HIDDEN       0x02 /* �B������ */
#define FA_SYSTEM       0x04 /* �V�X�e������ */
#define FA_LABEL        0x08 /* �{�����[���E���x�� */
#define FA_DIREC        0x10 /* �f�B���N�g�� */
#define FA_ARCH         0x20 /* �A�[�J�C�u���� */
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

// �R�}���h���C���ŏ��ɂ𑀍삵�܂�
int WINAPI UnImp(const HWND _hwnd,LPCSTR _szCmdLine,LPSTR _szOutput,const DWORD _dwSize);
// �o�[�W�������擾���܂�
WORD WINAPI UnImpGetVersion(VOID);
// �o�[�W�������擾���܂�
WORD WINAPI UnImpGetSubVersion(VOID);
// ���쒆�ł��邩�ۂ����擾���܂�
BOOL WINAPI UnImpGetRunning(VOID);
// �o�b�N�O���E���h���[�h�ł��邩�ۂ����擾���܂�
BOOL WINAPI UnImpGetBackGroundMode(VOID);
// �o�b�N�O���E���h���[�h�̐ݒ�����܂�
BOOL WINAPI UnImpSetBackGroundMode(const BOOL _bBackGroundMode);
// �J�[�\����\�����邩�ۂ����擾���܂�
BOOL WINAPI UnImpGetCursorMode(VOID);
// �J�[�\����\�����邩�ۂ���ݒ肵�܂�
BOOL WINAPI UnImpSetCursorMode(const BOOL _CursorMode);
// �J�[�\���̉�]���x���擾���܂�
WORD WINAPI UnImpGetCursorInterval(VOID);
// �J�[�\���̉�]���x��ݒ肵�܂�
BOOL WINAPI UnImpSetCursorInterval(const WORD _Interval);
// ���ɂ̐������������s���܂�
BOOL WINAPI UnImpCheckArchive(LPCSTR _szFileName,const int _iMode);
// ���ɂɊ܂܂��i�[�t�@�C���̐����擾���܂�
int WINAPI UnImpGetFileCount(LPCSTR _szArcFile);
// �w�肵��API���g�p�\�����f���܂�
BOOL WINAPI UnImpQueryFunctionList(const int _iFunction);
// DLL�̐ݒ�
BOOL WINAPI UnImpConfigDialog(const HWND _hwnd,LPSTR _szOptionBuffer,const int _iMode);
// ���ɂ��J���܂�
HARC WINAPI UnImpOpenArchive(const HWND _hwnd,LPCSTR _szFileName,const DWORD _dwMode);
// ���ɂ��J���܂�
HARC WINAPI UnImpOpenArchive2(const HWND _hwnd,LPCSTR _szFileName,const DWORD _dwMode,LPCSTR _szOption);
// ���ɂ���܂�
int WINAPI UnImpCloseArchive(HARC _harc);
// �ŏ��̊i�[�t�@�C�������擾���܂�
int WINAPI UnImpFindFirst(HARC _harc,LPCSTR _szWildName,INDIVIDUALINFO *_lpSubInfo);
// 2�Ԗڈȍ~�̊i�[�t�@�C�������擾���܂�
int WINAPI UnImpFindNext(HARC _harc,INDIVIDUALINFO *_lpSubInfo);
// �w�肵���t�@�C�����i�[���܂�
int WINAPI UnImpAdd(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
// �w�肵���t�@�C����W�J���܂�
int WINAPI UnImpExtract(HARC _harc,LPCSTR _szFileName,LPCSTR _szDirName,DWORD _dwMode);
// �w�肵���t�@�C�����ړ����܂�
int WINAPI UnImpMove(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
// �w�肵���t�@�C�����폜���܂�
int WINAPI UnImpDelete(HARC _harc,LPCSTR _szFileName,DWORD _dwMode);
// ���Ƀt�@�C�������擾���܂�
int WINAPI UnImpGetArcFileName(HARC _harc,LPSTR _lpBuffer,const int _nSize);
// ���ɂ̃t�@�C���T�C�Y���擾���܂�
DWORD WINAPI UnImpGetArcFileSize(HARC _harc);
// ���ɂ̃t�@�C���T�C�Y���擾���܂�(64�r�b�g)
BOOL WINAPI UnImpGetArcFileSizeEx(HARC _harc,LONGLONG *_lpllSize);
// �����Ƀ}�b�`�����t�@�C���T�C�Y�̍��v���擾���܂�
DWORD WINAPI UnImpGetArcOriginalSize(HARC _harc);
// �����Ƀ}�b�`�����t�@�C���T�C�Y�̍��v���擾���܂�(64�r�b�g)
BOOL WINAPI UnImpGetArcOriginalSizeEx(HARC _harc,LONGLONG *_lpllSize);
// �����Ƀ}�b�`�������k�t�@�C���T�C�Y�̍��v���擾���܂�
DWORD WINAPI UnImpGetArcCompressedSize(HARC _harc);
// �����Ƀ}�b�`�������k�t�@�C���T�C�Y�̍��v���擾���܂�(64�r�b�g)
BOOL WINAPI UnImpGetArcCompressedSizeEx(HARC _harc,LONGLONG *_lpllSize);
// �����Ƀ}�b�`�����t�@�C���S�̂̈��k�����擾���܂�
WORD WINAPI UnImpGetArcRatio(HARC _harc);
// ���ɂ̓��t���擾���܂�
WORD WINAPI UnImpGetArcDate(HARC _harc);
// ���ɂ̎��Ԃ��擾���܂�
WORD WINAPI UnImpGetArcTime(HARC _harc);
// ���ɂ̍쐬���Ԃ��擾���܂�
DWORD WINAPI UnImpGetArcCreateTime(HARC _harc);
// ���ɂ̍쐬���Ԃ��擾���܂�
BOOL WINAPI UnImpGetArcCreateTimeEx(HARC _harc,FILETIME *_lpftCreationTime);
// ���ɂ̎Q�Ǝ��Ԃ��擾���܂�
DWORD WINAPI UnImpGetArcAccessTime(HARC _harc);
// ���ɂ̎Q�Ǝ��Ԃ��擾���܂�
BOOL WINAPI UnImpGetArcAccessTimeEx(HARC _harc,FILETIME *_lpftLastAccessTime);
// ���ɂ̍X�V���Ԃ��擾���܂�
DWORD WINAPI UnImpGetArcWriteTime(HARC _harc);
// ���ɂ̍X�V���Ԃ��擾���܂�
BOOL WINAPI UnImpGetArcWriteTimeEx(HARC _harc,FILETIME *_lpftLastWriteTime);
// ���ɂ��쐬���ꂽOS�̎�ނ��擾���܂�
UINT WINAPI UnImpGetArcOSType(HARC _harc);
// ���ɂ����ȓW�J���ɂł��邩�ۂ����擾���܂�
int WINAPI UnImpIsSFXFile(HARC _harc);
// UnImpFindFirst/Next�œǂݍ��񂾃T�C�Y���擾���܂�
DWORD WINAPI UnImpGetArcReadSize(HARC _harc);
// UnImpFindFirst/Next�œǂݍ��񂾃T�C�Y���擾���܂�(64�r�b�g)
BOOL WINAPI UnImpGetArcReadSizeEx(HARC _harc,LONGLONG *_lpllSize);
// �i�[�t�@�C�������擾���܂�
int WINAPI UnImpGetFileName(HARC _harc,LPSTR _lpBuffer,const int _nSize);
// �i�[�t�@�C���̈��k���@���擾���܂�
int WINAPI UnImpGetMethod(HARC _harc,LPSTR _lpBuffer,const int _nSize);
// �i�[�t�@�C���̃t�@�C���T�C�Y���擾���܂�
DWORD WINAPI UnImpGetOriginalSize(HARC _harc);
// �i�[�t�@�C���̃t�@�C���T�C�Y���擾���܂�(64�r�b�g)
BOOL WINAPI UnImpGetOriginalSizeEx(HARC _harc,LONGLONG *_lpllSize);
// �i�[�t�@�C���̈��k�t�@�C���T�C�Y���擾���܂�
DWORD WINAPI UnImpGetCompressedSize(HARC _harc);
// �i�[�t�@�C���̈��k�t�@�C���T�C�Y���擾���܂�(64�r�b�g)
BOOL WINAPI UnImpGetCompressedSizeEx(HARC _harc,LONGLONG *_lpllSize);
// �i�[�t�@�C���̈��k�����擾���܂�
WORD WINAPI UnImpGetRatio(HARC _harc);
// �i�[�t�@�C���̓��t���擾���܂�
WORD WINAPI UnImpGetDate(HARC _harc);
// �i�[�t�@�C���̎��Ԃ��擾���܂�
WORD WINAPI UnImpGetTime(HARC _harc);
// �i�[�t�@�C���̍쐬���Ԃ��擾���܂�
DWORD WINAPI UnImpGetCreateTime(HARC _harc);
// �i�[�t�@�C���̍쐬���Ԃ��擾���܂�
BOOL WINAPI UnImpGetCreateTimeEx(HARC _harc,FILETIME *_lpftCreationTime);
// �i�[�t�@�C���̎Q�Ǝ��Ԃ��擾���܂�
DWORD WINAPI UnImpGetAccessTime(HARC _harc);
// �i�[�t�@�C���̎Q�Ǝ��Ԃ��擾���܂�
BOOL WINAPI UnImpGetAccessTimeEx(HARC _harc,FILETIME *_lpftLastAccessTime);
// �i�[�t�@�C���̍X�V���Ԃ��擾���܂�
DWORD WINAPI UnImpGetWriteTime(HARC _harc);
// �i�[�t�@�C���̍X�V���Ԃ��擾���܂�
BOOL WINAPI UnImpGetWriteTimeEx(HARC _harc,FILETIME *_lpftLastWriteTime);
// �i�[�t�@�C���̃`�F�b�N�T�����擾���܂�
DWORD WINAPI UnImpGetCRC(HARC _harc);
// �i�[�t�@�C���̑������擾���܂�
int WINAPI UnImpGetAttribute(HARC _harc);
// �i�[�t�@�C�����쐬���ꂽOS�̎�ނ��擾���܂�
UINT WINAPI UnImpGetOSType(HARC _harc);
// DLL���Ăяo���R�[���o�b�N�֐���o�^
BOOL WINAPI UnImpSetEnumMembersProc(EnumMembersProc _lpEnumProc);
// SetEnumMembersProc�œo�^�����R�[���o�b�N�֐�������
BOOL WINAPI UnImpClearEnumMembersProc(VOID);
// DLL���Ăяo���R�[���o�b�N�֐���o�^
BOOL WINAPI UnImpSetEnumMembersProc64(EnumMembersProc _lpEnumProc,DWORD _dwStructSize);
// SetEnumMembersProc64�œo�^�����R�[���o�b�N�֐�������
BOOL WINAPI UnImpClearEnumMembersProc64(VOID);
// DLL�̃��b�Z�[�W���M��E�B���h�E��o�^
BOOL WINAPI UnImpSetOwnerWindow(HWND _hwnd);
// SetOwnerWindow�œo�^�����E�B���h�E������
BOOL WINAPI UnImpClearOwnerWindow(VOID);
// �W�J�󋵂Ȃǂ��󂯎��R�[���o�b�N�֐���o�^
BOOL WINAPI UnImpSetOwnerWindowEx(HWND _hwnd,ARCHIVERPROC _lpArcProc);
// SetOwnerWindowEx�œo�^�����R�[���o�b�N�֐�������
BOOL WINAPI UnImpKillOwnerWindowEx(HWND _hwnd);
// �W�J�󋵂Ȃǂ��󂯎��R�[���o�b�N�֐���o�^
BOOL WINAPI UnImpSetOwnerWindowEx64(HWND _hwnd,ARCHIVERPROC _lpArcProc,DWORD _dwStructSize);
// SetOwnerWindowEx64�œo�^�����R�[���o�b�N�֐�������
BOOL WINAPI UnImpKillOwnerWindowEx64(HWND _hwnd);
// �f�o�b�O�pAPI
void WINAPI UnImpSetDebugMode(BOOL _bMode);

// �Â��o�[�W������API
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
