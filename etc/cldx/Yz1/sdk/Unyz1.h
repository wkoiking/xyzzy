/*********************************************
				Unyz1.dll 0.03
*********************************************/

#ifndef _Unyz1_h_
#define _Unyz1_h_

extern "C"
{
//全てのバージョンにある関数
WORD WINAPI UnYz1GetVersion();
int  WINAPI UnYz1s(const HWND hwnd ,char* lpszFile, char* lpszOutPut, char* lpszPass,int iMode);
BOOL WINAPI UnYz1CheckArchive(LPCSTR szFileName,const int iMode);
int WINAPI UnYz1GetFileCount(LPCSTR szArcFile);
int WINAPI UnYz1(const HWND hWnd,LPCSTR szCmdLine,LPSTR szOutput,const DWORD dwSize);

//0.02 Build 12 以降にある関数
WORD WINAPI UnYz1GetBuild();

//0.02 Build 15 以降にある関数
BOOL WINAPI UnYz1ConfigDialog(const HWND hWnd,LPSTR pOption,const int iMode);

//0.03 Build 100 / 0.02 Build 16 (予定) 以降にある関数
WORD WINAPI UnYz1GetRealVersion();
WORD WINAPI UnYz1GetRealBuild();
}

#endif

/*EOF*/