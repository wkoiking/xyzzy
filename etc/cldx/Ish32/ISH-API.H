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
#define ERROR_DISK_SPACE		0x8005		// Disk�̋󂫂�����܂���(�𓀎�)
#define ERROR_READ_ONLY			0x8006		// �𓀐�̃t�@�C�������[�h�I�����[(Replace�ł��Ȃ�)
#define ERROR_USER_SKIP			0x8007		// ���[�U�[�ɂ���ĉ𓀂��X�L�b�v����܂���
#define ERROR_UNKOWN_TYPE		0x8008
#define ERROR_METHOD			0x8009
#define ERROR_PASSWORD_FILE		0x800A
#define ERROR_VERSION			0x800B
#define ERROR_FILE_CRC			0x800C		// �G���[�����s��
#define ERROR_FILE_OPEN			0x800D		// �𓀎��Ƀt�@�C�����I�[�v���ł��Ȃ�
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
#define ERROR_ARCHIVE_FILE_OPEN	0x8018		// Header���Ȃ�
#define ERROR_NOT_ARCHIVE_FILE	0x8019
#define ERROR_CANNOT_READ		0x801A
#define ERROR_FILE_STYLE		0x801B
#define ERROR_COMMAND_NAME		0x801C
#define ERROR_MORE_HEAP_MEMORY	0x801D
#define ERROR_ENOUGH_MEMORY		0x801E
#define ERROR_ALREADY_RUNNING	0x801F
#define ERROR_USER_CANCEL		0x8020

#define ERROR_NOT_SUPPORT		0x8023

#define	ERROR_CANNOT_CREATE		0x8030		// ISH�t�@�C�����쐬�ł��܂���		
#define	ERROR_CANNOT_OPEN		0x8031		// �t�@�C�����I�[�v���ł��Ȃ�(�쐬��)

// --------------------------------------------------------------
// Ish : Ish�̏��������s���܂��B
//                   
//		hwnd        : �E�C���h�E�n���h��
//		lpszCmdLine : �R�}���h���C��
//                    (restore�ł���΃t�@�C�����̂݁A
//                     store�ł����"/s �t�@�C����"�łł��܂��B�j
//
//         �E�����^�쐬 ���ʂ̃I�v�V����
//				/f=<dirname> : �𓀐�f�B���N�g�����w�肵�܂�
//
//         �E�������̃I�v�V����
//				/r : restore new file(s)
//				/a : restore all file(s)
//				/q : restore with question
//
//         �E�쐬���̃I�v�V����
//
//           �E�쐬���[�h
//				/s : store a JIS8 format ish file
//		       	/s8: same as -s
//				/ss: store a shift JIS format ish file  
//				/s7: store a JIS7 format ish file
//				/sn: store a shift JIS non-kana format ish file
//
//           �E�Ώ�OS
//				/m : MS-DOS
//				/u : UNIX
//				/c : CP/M
//				/9 : OS-9
//				/? : other OS(s)
//				/* : all OS(s)
//
//           �E�G�X�P�[�v�V�[�P���X�̎g�p
//				/n : no ESC sequence in TITLE line      
//				/e : ESC sequence in TITLE line
//
//           �E�^�C�g�����������ލs��
//				/tn: title in n line
//
//			 �EDecode�t�@�C���w��(�w��t�@�C���̂�Decode����)
//				/Y <filename>
//				�����w��̏ꍇ�́A/Y <filename> /Y <filename> ...
//				���C���h�J�[�h�w��(?, *)����
//
//			 �E�ǉ��o��(�쐬���̂ݗL��)
//				/YA <filename>
//				�w��̃t�@�C���ɒǉ��o�͂���
//				�������A���ɓ����t�@�C��������ꍇ�ɒ���
//
// -------------------------------------------------------------
//int WINAPI _export Ish(HWND hwnd, LPSTR lpszCmdLine, LPSTR lpszDir);
int WINAPI _export Ish(HWND hwnd, LPSTR lpszCmdLine);

// --------------------------------------------------------------
// IshGetVersion : ish.dll�̃o�[�W������Ԃ��܂��B
//                   
//    ���ʂ�int�Ŗ߂��܂��B�Ⴆ��
//	    001��Ver0.01
//      100��Ver1.00
//    �ƂȂ�܂��B
// --------------------------------------------------------------
int WINAPI _export IshGetVersion(void);

// --------------------------------------------------------------
// IshGetRunning : ish.dll�����쒆����Ԃ��܂��B
//                   
//    TRUE�ł���΁A���쒆�Ȃ̂Ŏg�p�ł��܂���B
//	  FALSE�ł���΁A�g�p�ł��܂��B
// --------------------------------------------------------------
BOOL WINAPI _export IshGetRunning(void);

// --------------------------------------------------------------
// IshConfigDialog : ish.dll�̊��ݒ�_�C�A���O��\�����܂��B
//                   �߂��lpszOpt�ɃR�}���h���C���̌`����
//                   �Ԃ���܂��B
//
//	hwnd 	: �E�C���h�E�n���h��
//	lpszOpt : �_�C�A���O�Őݒ肵���R�}���h���C�����i�[���܂��B
//            lpszOpt�́A�K��128�o�C�g�ȏ�̑傫����p�ӂ��Ă��������B
//            �܂��A�Ăяo�����Ƀp�����[�^��ݒ肷��΃_�C�A���O�̏���
//            �l�����̃p�����[�^�ɂȂ�܂��B
//            ""��n���΁Aarchive.ini�ɐݒ肳�ꂽ�p�����[�^�������l��
//            �Ȃ�܂��B
//  mode    : �����i�𓀁j�n�̃R�}���h���C����ݒ肷��ꍇ��
//            UNPACK_CONFIG_MODE���A�쐬�i���k�j�n�̃R�}���h���C����
//            �ݒ肷��ꍇ��PACK_CONFIG_MODE��ݒ肵�Ă��������B
//  �߂�l  : �ݒ���s�����ꍇ��TRUE���A�L�����Z�������ꍇ��FALSE��
//            �߂��܂��B
//
// --------------------------------------------------------------
#define UNPACK_CONFIG_MODE	1
#define PACK_CONFIG_MODE	2

BOOL WINAPI _export IshConfigDialog(HWND hwnd, LPSTR lpszOpt, int mode);

// --------------------------------------------------------------
// IshCheckArchive : lpszFileName�Ŏw�肵���t�@�C����ish�f�[�^��
//                   �܂܂�Ă��邩�𒲂ׂ܂��B
//                   �������A���݂̎d�l�ł̓t�@�C�������݂����TRUE��
//					 �Ԃ��Ă��܂��B
// --------------------------------------------------------------
BOOL WINAPI _export IshCheckArchive(LPSTR lpszFileName, int iMode);

// --------------------------------------------------------------
// �ȉ��̊֐��͖������ł����ALHA.DLL�����̃A�[�J�C�uDLL�Ƃ�
// �݊����̂��߂ɃG���g�������p�ӂ��Ă���܂��B
BOOL WINAPI _export IshGetBackGroundMode(void);
BOOL WINAPI _export IshSetBackGroundMode(BOOL bBackGroundMode);
BOOL WINAPI _export IshGetCursorMode(void);
BOOL WINAPI _export IshSetCursorMode(BOOL bCursorMode);
unsigned int WINAPI _export IshGetCursorInterval(void);
BOOL WINAPI _export IshSetCursorInterval(unsigned int uInterval);

// --------------------------------------------------------------
// IshEx : Ish�̏��������s���܂��B
//                   
//  LHA.DLL�݊��̃C���^�[�t�F�C�X�ł��B
//	�R�}���h���C���̎w��́AIsh()�Ɠ����ł��B
//  ���s���ʂ��AszOutput�ɕԂ��܂��B
//                   
//	lpszCmdLine : �R�}���h���C��
//                    (restore�ł���΃t�@�C�����̂݁A
//                     store�ł����"/s �t�@�C����"�łł��܂��B�j
//  lpszOutput   : ���ʂ�Ԃ��o�b�t�@���w�肵�܂��B
//  wSize        : �o�b�t�@�̃T�C�Y���w�肵�܂��B�Ԃ����ʂ̃T�C�Y��菬�����ꍇ��
//                 �w��T�C�Y�܂ŕԂ����B
//  �߂�l       : ����I���̏ꍇ��0�A�G���[�̂Ƃ��̓G���[�R�[�h
// -------------------------------------------------------------
int WINAPI _export IshEx(LPSTR lpszCmdLine, LPSTR lpszOutput, WORD wSize);


// --------------------------------------------------------------
// IshListFile : lpszFileName�Ŏw�肵���t�@�C������ish�f�[�^�t�@�C��
//               �ꗗ��szOutput�Ŏw�肳�ꂽ�̈�Ɏ��̌`���ŕԂ��܂��B
//               (1:,2:�͊܂܂�܂���B�e�s�̖�����LF(0Ah)�ƂȂ�܂��B)
// 1:
// 2:Listing of Ish : <file��>
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
//	lpszFileName : ISH�f�[�^���܂ރt�@�C�������w�肵�܂��B
//  lpszOutput   : ���ʂ�Ԃ��o�b�t�@���w�肵�܂��B
//  wSize        : �o�b�t�@�̃T�C�Y���w�肵�܂��B�Ԃ����ʂ̃T�C�Y��菬�����ꍇ��
//                 �w��T�C�Y�܂ŕԂ����B
//  �߂�l       : ����I���̏ꍇ��0�A�G���[�̂Ƃ��̓G���[�R�[�h
//                 ISH�f�[�^���܂܂�Ȃ��ꍇ�́AERROR_NOT_ARCHIVE_FILE��Ԃ��B
//  ���̑�       : �G���[���b�Z�[�W�͏o�͂��Ȃ�
//---------------------------------------------------------------
int WINAPI _export IshFileList(LPSTR lpszFileName, LPSTR lpszOutput, WORD wSize);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  // __cplusplus

#endif	// ISH_DLL_H