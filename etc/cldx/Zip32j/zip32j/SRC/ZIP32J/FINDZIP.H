/* 
	findzip.h
	by �g�� �P�v(email:QWF00133@niftyserve.or.jp)
	�R�s�[�A�����A���p�A�Ĕz�z�����R
	(PDS�Ɠ����Ɉ����Ă�����Č��\�ł�)

	�ړI�F
	unzip32.dll��UnZip()API�𗘗p����
	UnZipFindFirst/UnZipFindNext����API������ 
	# unzip32.dll��������API���T�|�[�g��������͂�p�Ȃ��ł��B(^^;
	
	UnZip()�R�}���h�� -l �I�v�V������������擾�B
														*/

#define ISARC_FIND_FIRST			25
typedef HGLOBAL HARC;
#define FNAME_MAX32	512


typedef struct {
	DWORD	dwOriginalSize;		/* �t�@�C���̃T�C�Y�B		*/
	DWORD	dwCompressedSize;	/* ���k��̃T�C�Y�B		*/
	DWORD	dwCRC;			/* �i�[�t�@�C���̃`�F�b�N�T��/CRC */
	UINT	uFlag;			/* �𓀂�e�X�g�̏�������	*/
	UINT	uOSType;		/* ���̃t�@�C���̍쐬�Ɏg��ꂽ�n�r�B*/
	WORD	wRatio;			/* ���k���i�p�[�~��)		*/
	WORD	wDate;			/* �i�[�t�@�C���̓��t�B		*/
	WORD	wTime;			/* �i�[�t�@�C���̎����B		*/
	char	szFileName[FNAME_MAX32 + 1];/* �A�[�J�C�u�t�@�C����*/
	char	dummy1[3];
	char	szAttribute[8];		/* �i�[�t�@�C���̑����B		*/
	char	szMode[8];		/* �i�[�t�@�C���̊i�[���[�h�B	*/
} INDIVIDUALINFO, FAR *LPINDIVIDUALINFO;

#ifndef UNZIP32_H
int WINAPI UnZip(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize);
// BOOL UnZipQueryFunctionList(const int iFunction);
WORD WINAPI UnZipGetVersion(VOID);
HARC WINAPI UnZipOpenArchive(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode);
int WINAPI UnZipFindFirst(HARC hArc,
			LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo);
int WINAPI UnZipCloseArchive(HARC hArc);
int WINAPI UnZipFindNext(HARC hArc,INDIVIDUALINFO *lpSubInfo);
BOOL WINAPI UnZipQueryFunctionList(const int iFunction);
#endif

int WINAPI ZipUnZip(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize);
// BOOL UnZipQueryFunctionList(const int iFunction);
WORD WINAPI ZipUnZipGetVersion(VOID);
HARC WINAPI ZipUnZipOpenArchive(const HWND hWnd,LPCSTR szFileName,const DWORD dwMode);
int WINAPI ZipUnZipFindFirst(HARC hArc,
			LPCSTR szWildName,INDIVIDUALINFO *lpSubInfo);
int WINAPI ZipUnZipCloseArchive(HARC hArc);
int WINAPI ZipUnZipFindNext(HARC hArc,INDIVIDUALINFO *lpSubInfo);
BOOL WINAPI ZipUnZipQueryFunctionList(const int iFunction);

int finddll_init();
int finddll_end();




