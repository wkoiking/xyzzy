#if !defined(WDN_AISHMV32_H)
#define WDN_AISHMV32_H

#include <windows.h>

#ifdef __BUILDING_THE_DLL
#define __EXPORT_TYPE __declspec(dllexport)
#else
#define __EXPORT_TYPE __declspec(dllimport)
#endif /* __BUILDING_THE_DLL */

#if defined(__cplusplus)
extern "C" {
#endif

#define AISHMV_VERSION (4) /* Version 0.04 */


#if !defined(AISH_ERR_CODES)
#define AISH_ERR_CODES

/* �G���[�E�R�[�h�� AISH32.DLL�AAISHMV32.DLL ���� */

/* �G���[ - �v���I�ȃG���[�ŁA���̎��_�Ŏ��s�𒆎~���� */
#define AISH_ERROR_FILE_OPEN         0x800D // �������Ƀt�@�C�����J���܂���ł���
#define AISH_ERROR_ARC_FILE_OPEN     0x8018 // ISH file ���J�����Ƃ��ł��܂���
#define AISH_ERROR_TMP_OPEN          0x8025 // ��ƃt�@�C�����J���܂���

/* AISH �Ǝ��̌x���R�[�h�i�����𒆒f���Ȃ��j */
/*                             0x8081 -      */

/* AISH �Ǝ��̃G���[�E�R�[�h�i�����𒆒f����j */
#define AISH_ERROR_CANT_SPLIT        0x80A1 // �G���R�[�h���ɕ����ł��Ȃ�
#define AISH_ERROR_CANT_READ_INFILE  0x80A2 // encode ���邽�߂� file ���J���܂���
#define AISH_ERROR_TOO_MANY_VOLUMES  0x80A3 // �������̎w�肪��������

#endif /* AISH_ERR_CODES */



/* 1. �R�}���h�E���C���ł̎��s */
int WINAPI __EXPORT_TYPE Aishmv(const HWND _hwnd, LPCSTR _szCmdLine,
	LPSTR _szOutput, const DWORD _dwSize);

/* 2. �o�[�W�����̎擾 */
WORD WINAPI __EXPORT_TYPE AishmvGetVersion(VOID);

/* 3. �J�[�\���̉�]���x�̎擾 */
WORD WINAPI __EXPORT_TYPE AishmvGetCursorInterval(VOID);

/* 4. �J�[�\���̉�]���x�̐ݒ� */
BOOL WINAPI __EXPORT_TYPE AishmvSetCursorInterval(const WORD _Interval);

/* 5. �o�b�N�O���E���h�E���[�h�ł��邩�ǂ����̎擾 */
BOOL WINAPI __EXPORT_TYPE AishmvGetBackGroundMode(VOID);

/* 6. �o�b�N�O���E���h�E���[�h�ɂ��邩�ݒ� */
BOOL WINAPI __EXPORT_TYPE AishmvSetBackGroundMode(const BOOL _bBackGroundMode);

/* 7. �J�[�\����\�����郂�[�h�����擾 */
BOOL WINAPI __EXPORT_TYPE AishmvGetCursorMode(VOID);

/* 8. �J�[�\����\�����郂�[�h����ݒ� */
BOOL WINAPI __EXPORT_TYPE AishmvSetCursorMode(const BOOL _bCursorMode);

/* 10. DLL �����쒆����Ԃ� */
BOOL WINAPI __EXPORT_TYPE AishmvGetRunning(VOID);

/* 11. �w�肳�ꂽ�t�@�C���� ISH �f�[�^�����݂��AISH �f�[�^�Ƃ��Đ�������
 * �ǂ����𒲂ׂ�B
 */
/* !!! ���ӁF�@���̃o�[�W�����ł̓t�@�C�������݂��邩��Ԃ��̂� !!! */
BOOL WINAPI __EXPORT_TYPE AishmvCheckArchive(LPCSTR _szFileName,
	const int _iMode);

/* 18. �w�肳�ꂽ API ���g�p�\����Ԃ� */
BOOL WINAPI __EXPORT_TYPE AishmvQueryFunctionList(const int _iFunction);


#if !defined(ISARC_FUNCTION_START)

/* AishmvQueryFunctionList() API ���Ԃ��l */

#define ISARC_FUNCTION_START			0
#define ISARC							0	/* Aishmv */
#define ISARC_GET_VERSION				1	/* AishmvGetVersion */
#define ISARC_GET_CURSOR_INTERVAL		2	/* AishmvGetCursorInterval */
#define ISARC_SET_CURSOR_INTERVAL		3	/* AishmvSetCursorInterval */
#define ISARC_GET_BACK_GROUND_MODE		4	/* AishmvGetBackGroundMode */
#define ISARC_SET_BACK_GROUND_MODE		5	/* AishmvSetBackGroundMode */
#define ISARC_GET_CURSOR_MODE			6	/* AishmvGetCursorMode */
#define ISARC_SET_CURSOR_MODE			7	/* AishmvSetCursorMode */
#define ISARC_GET_RUNNING				8	/* AishmvGetRunning */

#define ISARC_CHECK_ARCHIVE				16	/* AishmvCheckArchive */
#define ISARC_CONFIG_DIALOG				17	/* AishmvConfigDialog */
#define ISARC_GET_FILE_COUNT			18	/* AishmvGetFileCount */
#define ISARC_QUERY_FUNCTION_LIST		19	/* AishmvQueryFunctionList */
#define ISARC_HOUT						20	/* (AishmvHOut) */
#define ISARC_STRUCTOUT					21	/* (AishmvStructOut) */
#define ISARC_GET_ARC_FILE_INFO			22	/* AishmvGetArcFileInfo */

#define ISARC_OPEN_ARCHIVE				23	/* AishmvOpenArchive */
#define ISARC_CLOSE_ARCHIVE				24	/* AishmvCloseArchive */
#define ISARC_FIND_FIRST				25	/* AishmvFindFirst */
#define ISARC_FIND_NEXT					26	/* AishmvFindNext */
#define ISARC_EXTRACT					27	/* (AishmvExtract) */
#define ISARC_ADD						28	/* (AishmvAdd) */
#define ISARC_MOVE						29	/* (AishmvMove) */
#define ISARC_DELETE					30	/* (AishmvDelete) */
#define ISARC_SETOWNERWINDOW			31	/* AishmvSetOwnerWindow */
#define ISARC_CLEAROWNERWINDOW			32	/* AishmvClearOwnerWindow */
#define ISARC_SETOWNERWINDOWEX			33	/* AishmvSetOwnerWindowEx */
#define ISARC_KILLOWNERWINDOWEX			34	/* AishmvKillOwnerWindowEx */

#define ISARC_GET_ARC_FILE_NAME			40	/* AishmvGetArcFileName */
#define ISARC_GET_ARC_FILE_SIZE			41	/* AishmvGetArcFileSize */
#define ISARC_GET_ARC_ORIGINAL_SIZE		42	/* AishmvArcOriginalSize */
#define ISARC_GET_ARC_COMPRESSED_SIZE	43	/* AishmvGetArcCompressedSize */
#define ISARC_GET_ARC_RATIO				44	/* AishmvGetArcRatio */
#define ISARC_GET_ARC_DATE				45	/* AishmvGetArcDate */
#define ISARC_GET_ARC_TIME				46	/* AishmvGetArcTime */
#define ISARC_GET_ARC_OS_TYPE			47	/* AishmvGetArcOSType */
#define ISARC_GET_ARC_IS_SFX_FILE		48	/* AishmvGetArcIsSFXFile */
#define ISARC_GET_ARC_WRITE_TIME_EX		49	/* AishmvGetArcWriteTimeEx */
#define ISARC_GET_ARC_CREATE_TIME_EX	50	/* AishmvGetArcCreateTimeEx */
#define	ISARC_GET_ARC_ACCESS_TIME_EX	51	/* AishmvGetArcAccessTimeEx */
#define	ISARC_GET_ARC_CREATE_TIME_EX2	52	/* AishmvGetArcCreateTimeEx2 */
#define ISARC_GET_FILE_NAME				57	/* AishmvGetFileName */
#define ISARC_GET_ORIGINAL_SIZE			58	/* AishmvGetOriginalSize */
#define ISARC_GET_COMPRESSED_SIZE		59	/* AishmvGetCompressedSize */
#define ISARC_GET_RATIO					60	/* AishmvGetRatio */
#define ISARC_GET_DATE					61	/* AishmvGetDate */
#define ISARC_GET_TIME					62	/* AishmvGetTime */
#define ISARC_GET_CRC					63	/* AishmvGetCRC */
#define ISARC_GET_ATTRIBUTE				64	/* AishmvGetAttribute */
#define ISARC_GET_OS_TYPE				65	/* AishmvGetOSType */
#define ISARC_GET_METHOD				66	/* AishmvGetMethod */
#define ISARC_GET_WRITE_TIME			67	/* AishmvGetWriteTime */
#define ISARC_GET_CREATE_TIME			68	/* AishmvGetCreateTime */
#define ISARC_GET_ACCESS_TIME			69	/* AishmvGetAccessTime */
#define ISARC_GET_WRITE_TIME_EX			70	/* AishmvGetWriteTimeEx */
#define ISARC_GET_CREATE_TIME_EX		71	/* AishmvGetCreateTimeEx */
#define ISARC_GET_ACCESS_TIME_EX		72	/* AishmvGetAccessTimeEx */
#define ISARC_SET_ENUM_MEMBERS_PROC		80  /* AishmvSetEnumMembersProc */
#define ISARC_CLEAR_ENUM_MEMBERS_PROC	81	/* AishmvClearEnumMembersProc */

#endif	/* ISARC_FUNCTION_START */


#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* WDN_AISHMV32_H */
