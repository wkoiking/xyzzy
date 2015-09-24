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
// $Id: Common.cpp,v 1.23 2005/06/17 12:22:58 hyperbeat Exp $
//

#include <libarcdll.h>
#include <malloc.h>
#include "Common.h"
#include "resource.h"

typedef unsigned char uchar;
typedef unsigned short uint16;

// 時間を文字列に変換
void time_to_string(WORD date,WORD time,char *str)
{
    FILETIME ft;
    FILETIME ft_local;
    SYSTEMTIME systime;
    DosDateTimeToFileTime(date,time,&ft);
    FileTimeToLocalFileTime(&ft,&ft_local);
    FileTimeToSystemTime(&ft_local,&systime);
    sprintf(str,"%04d.%02d.%02d %02d.%02d.%02d",systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wMinute);
}

// 上書確認ダイアログメッセージ処理
BOOL CALLBACK UpdateDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        libarcdll_CenterWindow(((UpdateInfo*)(lParam))->hWnd,hWnd);
        {
            char *bf = (char*)malloc(50);
            if (lstrlen(((UpdateInfo*)(lParam))->in) > 40)
            {
                libarcdll_AbbreviationPath(((UpdateInfo*)(lParam))->in,bf,40,5,3);
            }
            else
            {
                lstrcpy(bf,((UpdateInfo*)(lParam))->in);
            }
            SetDlgItemText(hWnd,IDC_ARC_FILENAME,bf);
            if (lstrlen(((UpdateInfo*)(lParam))->out) > 40)
            {
                libarcdll_AbbreviationPath(((UpdateInfo*)(lParam))->out,bf,40,5,3);
            }
            else
            {
                lstrcpy(bf,((UpdateInfo*)(lParam))->out);
            }
            SetDlgItemText(hWnd,IDC_LOCAL_FILENAME,bf);
            free(bf);
        }
        if (libarcdll_GetOSType() >= OS_WINNT_XP)
        {
            HMODULE hDll;
            HRESULT (WINAPI *SetWindowTheme)(HWND,LPCWSTR,LPCWSTR);
            if ((hDll = LoadLibrary("UxTheme.dll")) != NULL)
            {
                SetWindowTheme = (HRESULT(WINAPI*)(HWND,LPCWSTR,LPCWSTR))GetProcAddress(hDll,"SetWindowTheme");
                if (SetWindowTheme != NULL)
                {
                    SetWindowTheme(GetDlgItem(hWnd,IDC_UPDATE),L"",L"");
                    SetWindowTheme(GetDlgItem(hWnd,IDC_UPDATE_ALL),L"",L"");
                    SetWindowTheme(GetDlgItem(hWnd,IDC_SKIP),L"",L"");
                    SetWindowTheme(GetDlgItem(hWnd,IDC_SKIP_ALL),L"",L"");
                }
                FreeLibrary(hDll);
            }
        }
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_UPDATE:
            EndDialog(hWnd,0);
            return TRUE;
        case IDC_UPDATE_ALL:
            EndDialog(hWnd,1);
            return TRUE;
        case IDC_SKIP:
            EndDialog(hWnd,2);
            return TRUE;
        case IDC_SKIP_ALL:
            EndDialog(hWnd,3);
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}
