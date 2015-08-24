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
// $Id: Extract.cpp,v 1.30 2005/06/17 08:12:18 hyperbeat Exp $
//

#include <io.h>
#include <libarcdll.h>
#include <malloc.h>
#include <process.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <commctrl.h>
#include <shellapi.h>
#include "UnImp.h"
#include "Common.h"
#include "UnImp32.h"
#include "resource.h"

#define WM_TASKCALLBACK (WM_USER + 1)

#pragma pack(1)

typedef struct
{
    int mode;
    FILETIME ft;
}FILETIMEINFO;

#pragma pack()

DWORD GetAttributes(WORD src);

// インスタンスハンドル
extern HINSTANCE g_hInst;

// 中止フラグ
bool bCancel;

// ステータスメッセージ処理
BOOL CALLBACK StatusProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        bCancel = false;
        libarcdll_CenterWindow((HWND)lParam,hWnd);
        SendMessage(hWnd,WM_SETICON,ICON_BIG,(LPARAM)LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_MAIN)));
        SendMessage(hWnd,WM_SETICON,ICON_SMALL,(LPARAM)LoadIcon(g_hInst,MAKEINTRESOURCE(IDI_MAIN)));
        if (libarcdll_GetCursorMode() == TRUE)
        {
            SetClassLong(hWnd,GCL_HCURSOR,(LONG)LoadCursor((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDC_MAIN)));
            SetClassLong(GetDlgItem(hWnd,IDC_PROGRESS),GCL_HCURSOR,(LONG)LoadCursor((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDC_MAIN)));
            SetClassLong(GetDlgItem(hWnd,IDC_CANCEL),GCL_HCURSOR,(LONG)LoadCursor((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),MAKEINTRESOURCE(IDC_MAIN)));
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
                    SetWindowTheme(GetDlgItem(hWnd,IDC_CANCEL),L"",L"");
                    SetWindowTheme(GetDlgItem(hWnd,IDC_INTASK),L"",L"");
                    SetWindowTheme(GetDlgItem(hWnd,IDC_PROGRESS),L"",L"");
                }
                FreeLibrary(hDll);
            }
        }
        return TRUE;
    case WM_DESTROY:
        return TRUE;
    case WM_SYSCOMMAND:
        switch (wParam)
        {
        case SC_CLOSE:
            if (MessageBox(hWnd,"展開を中止しますか？","UnImp32.dll",MB_YESNO) == IDYES)
            {
                bCancel = true;
            }
            return TRUE;
        }
        return FALSE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_CANCEL:
            SendMessage(hWnd,WM_SYSCOMMAND,SC_CLOSE,0);
            return TRUE;
        }
        return FALSE;
    case WM_TASKCALLBACK:
        return TRUE;
    }
    return FALSE;
}

// 絶対パスを相対パスへ変換
void ConvertAbsPathToRelPath(char *src,char *dest)
{
    int i;
    int j;
    int cut_size = 0;
    char _src[MAX_PATH * 2];
    char _dest[MAX_PATH * 2];
    lstrcpy(_src,src);
    if (lstrlen(_src) > 2)
    {
        if (_src[0] == '\\')
        {
            cut_size = 1;
            if (_src[1] == '\\')
            {
                cut_size = 2;
            }
        }
    }
    if (lstrlen(_src) > 3)
    {
        if (_src[1] == ':' && _src[2] == '\\')
        {
            cut_size = 3;
        }
    }
    for (i = cut_size,j = 0;i < lstrlen(_src);i++,j++)
    {
        _dest[j] = _src[i];
    }
    _dest[j] = '\0';
    lstrcpy(dest,_dest);
}

// ステータスウィンドウハンドル
HWND g_window;
// UnImpクラス
CUnImp g_Imp;
// 書庫情報データ
IMP_ARCHIVE_INFO g_iai;
// 情報番号
uint32 g_iai_number;
// ステータス
int g_nStatus;

// 展開スレッド
void Extracting_Thread(void *p)
{
    char outfilename[MAX_PATH * 2];
    lstrcpy(outfilename,(LPCTSTR)p);
    if (g_Imp.extract_file(g_iai.ifh[g_iai_number].dirent,outfilename) == false)
    {
        g_nStatus = g_Imp.get_last_error();
        _endthread();
    }
    g_nStatus = 0;
    _endthread();
}

// 展開
// [E,X] [ArchiveName] [BaseDirectory] [Path] ...
// 基準ディレクトリが指定されてない場合はカレントディレクトリに展開
int Extract(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage,bool path)
{
    char *p;
    short per;
    int j;
    int k;
    int f;
    int mfc = 0;
    int length;
    int path_start;
    int update_file = 1;
    int write_skip = 0;
    int answer_yes = 0;
    int recursive = 0;
    int unjust_path = 1;
    int save_attribute = 0;
    int check_disk = 1;
    int extract_dos_name = 0;
    int show_dialog = 0;
    int check_disk_size = 0;
    int user_set = 0;
    int delete_root = 1;
    int exclusion_file_count;
    char *exclusion_file[MAX_PATH * 2];
    bool current_update_mode;
    bool check;
    uint32 i;
    char lm = '0';
    char temp[513];
    char _basedirectory[MAX_PATH * 2];
    char basedirectory[MAX_PATH * 2];
    char *filepath;
    char ap[MAX_PATH * 2];
    char arcname[MAX_PATH * 2];
    char message[4096];
    char filename[MAX_PATH * 2];
    char outfilename[MAX_PATH * 2];
    char relpath[MAX_PATH * 2];
    DWORD dwAttribute;
    char name83[MAX_PATH * 2];
    HWND wnd;
    FILETIME ctime;
    FILETIME ctime_local;
    FILETIME mtime;
    FILETIME mtime_local;
    SYSTEMTIME cf_systime;
    SYSTEMTIME tf_systime;
    UpdateInfo ui;
    struct _stat fdata;
    FILETIMEINFO tfi;
    ENUM_MEMBER_INFO64 param64;
    BOOL cs;
    BOOL retval;
    FILETIME lft;
    FILETIME cft;
    FILETIME aft;
    FILETIME mft;
    if (libarcdll_GetOtherDataCount(hCmdLine) < 2)
    {
        return ERROR_COMMAND_NAME;
    }
    libarcdll_GetSwitch(hCmdLine,NULL,"a",NULL,NULL,NULL,"%d",&save_attribute);
    libarcdll_GetSwitch(hCmdLine,NULL,"n",NULL,NULL,NULL,"%d",&show_dialog);
    libarcdll_GetSwitch(hCmdLine,NULL,"f",NULL,NULL,NULL,"%d",&check_disk);
    libarcdll_GetSwitch(hCmdLine,NULL,"jb",NULL,NULL,NULL,"%d",&check_disk_size);
    libarcdll_GetSwitch(hCmdLine,NULL,"jp",NULL,NULL,NULL,"%d",&unjust_path);
    libarcdll_GetSwitch(hCmdLine,NULL,"jo",NULL,NULL,NULL,"%c%s",&lm,temp);
    libarcdll_GetSwitch(hCmdLine,NULL,"r",NULL,NULL,NULL,"%d",&recursive);
    libarcdll_GetSwitch(hCmdLine,NULL,"m",NULL,NULL,NULL,"%d",&answer_yes);
    libarcdll_GetSwitch(hCmdLine,NULL,"s",NULL,NULL,NULL,"%d",&write_skip);
    libarcdll_GetSwitch(hCmdLine,NULL,"u",NULL,NULL,NULL,"%d",&update_file);
    libarcdll_GetSwitch(hCmdLine,NULL,"ji",NULL,NULL,NULL,"%d",&extract_dos_name);
    libarcdll_GetSwitch(hCmdLine,NULL,"jf",NULL,NULL,NULL,"%d",&delete_root);
    tfi.mode = lm - '0';
    tfi.ft.dwHighDateTime = 0;
    tfi.ft.dwLowDateTime = 0;
    libarcdll_GetOtherData(hCmdLine,0,"%s",arcname);
    libarcdll_GetOtherData(hCmdLine,1,"%s",_basedirectory);
    if (g_Imp.open_archive(arcname,&g_iai) == false)
    {
        return g_Imp.get_last_error();
    }
    if (show_dialog == 0)
    {
        wnd = (hwnd != NULL) ? hwnd : GetDesktopWindow();
        g_window = CreateDialogParam(g_hInst,MAKEINTRESOURCE(IDD_STATUS),hwnd,(DLGPROC)StatusProc,(LPARAM)wnd);
    }
    else
    {
        g_window = NULL;
    }
    if (g_window != NULL)
    {
        ShowWindow(g_window,SW_SHOWNORMAL);
        SetFocus(g_window);
        _fullpath(temp,arcname,513);
        libarcdll_AbbreviationPath(temp,ap,40,0,3);
        SetDlgItemText(g_window,IDC_ARCNAME,ap);
        libarcdll_doevents();
    }
    if (_basedirectory[lstrlen(_basedirectory) - 1] != '\\' && _basedirectory[lstrlen(_basedirectory) - 1] != '/')
    {
        GetCurrentDirectory(MAX_PATH * 2,basedirectory);
        path_start = 1;
    }
    else
    {
        lstrcpy(basedirectory,_basedirectory);
        path_start = 2;
    }
    length = lstrlen(basedirectory);
    for (f = 0;f < length;f++)
    {
        if (libarcdll_isJMS(basedirectory,f) == 0 && (basedirectory[f] == '\\' || basedirectory[f] == '/'))
        {
            basedirectory[f] = '\\';
        }
    }
    if (basedirectory[length - 1] != '\\')
    {
        lstrcat(basedirectory,"\\");
    }
    if (CUnImp::mkdir2(basedirectory) == false)
    {
        if (g_window != NULL)
        {
            DestroyWindow(g_window);
        }
        return ERROR_MAKEDIRECTORY;
    }
    for (i = 0,length = libarcdll_GetOtherDataCount(hCmdLine);i < g_iai.file_count;i++)
    {
        check = false;
        if (check_disk == 1)
        {
            if (libarcdll_CheckDiskSize(basedirectory,g_iai.ifh[i].dirent.esize) == FALSE)
            {
                goto check_end;
            }
        }
        if (check_disk_size != 0)
        {
            if (libarcdll_CheckDiskSize(basedirectory,check_disk_size) == FALSE)
            {
                goto check_end;
            }
        }
        if (save_attribute == 0 && (g_iai.ifh[i].dirent.attrib & _A_HIDDEN || g_iai.ifh[i].dirent.attrib & _A_SYSTEM))
        {
            goto check_end;
        }
        DosDateTimeToFileTime(g_iai.ifh[i].dirent.mdate,g_iai.ifh[i].dirent.mtime,&mtime);
        switch (tfi.mode)
        {
        case 0:
            break;
        case 1:
            FileTimeToSystemTime(&mtime,&cf_systime);
            FileTimeToSystemTime(&tfi.ft,&tf_systime);
            if (cf_systime.wYear != tf_systime.wYear)
            {
                goto check_end;
            }
            if (cf_systime.wMonth != tf_systime.wMonth)
            {
                goto check_end;
            }
            if (cf_systime.wMonth != tf_systime.wMonth)
            {
                goto check_end;
            }
            break;
        case 2:
            if (CompareFileTime(&tfi.ft,&mtime) > 0)
            {
                goto check_end;
            }
            break;
        case 3:
            if (CompareFileTime(&tfi.ft,&mtime) < 0)
            {
                goto check_end;
            }
            break;
        }
        if ((length - path_start) > 0)
        {
            filepath = (char*)malloc(513);
            for (j = path_start;j < length;j++)
            {
                libarcdll_GetOtherData(hCmdLine,j,"%s",filepath);
                if (recursive == 0)
                {
                    if (libarcdll_ComparisonWildcard2(filepath,g_iai.ifh[i].filename,COMPARISON_MODE_SAMEDIRECTORY) == TRUE)
                    {
                        check = true;
                    }
                }
                else
                {
                    if (libarcdll_ComparisonWildcard2(filepath,g_iai.ifh[i].filename,COMPARISON_MODE_RECURSIVE) == TRUE)
                    {
                        check = true;
                    }
                }
            }
            free(filepath);
        }
        else
        {
            if (recursive == 1)
            {
                check = true;
            }
            else if (recursive == 0 && libarcdll_GetCharCount(g_iai.ifh[i].filename,'\\') == 0)
            {
                check = true;
            }
        }
        if (check == false)
        {
            goto check_end;
        }
        if (libarcdll_IsValidFileName(g_iai.ifh[i].filename) == FALSE)
        {
            if (unjust_path == 0 || answer_yes == 1)
            {
                goto check_end;
            }
            if (MessageBox(g_window,"指定したディレクトリ以外の場所に展開されようとしています。\n展開しますか？","UnImp32.dll",MB_YESNO) == IDNO)
            {
                goto check_end;
            }
        }
        libarcdll_GetSwitch(hCmdLine,NULL,"jx",&exclusion_file_count,exclusion_file,NULL,NULL);
        for (j = 0;j < exclusion_file_count;j++)
        {
            if (libarcdll_ComparisonWildcard(exclusion_file[j],g_iai.ifh[i].filename) == TRUE)
            {
                check = false;
            }
        }
        for (j = 0;j < exclusion_file_count;j++)
        {
            free(exclusion_file[j]);
        }
        if (check == true)
        {
            lstrcpy(filename,g_iai.ifh[i].filename);
            if (path == false)
            {
                p = strrchr(g_iai.ifh[i].filename,'\\');
                if (p != NULL)
                {
                    for (k = 0;!(libarcdll_isJMS(p,k + 1) == 0 && *(p + k + 1) == '\0');k++)
                    {
                        filename[k] = *(p + k + 1);
                    }
                    filename[k] = '\0';
                }
            }
            if (delete_root == 1)
            {
                ConvertAbsPathToRelPath(filename,relpath);
            }
            else
            {
                lstrcpy(relpath,filename);
            }
            sprintf(temp,"%s%s",basedirectory,relpath);
            _fullpath(outfilename,temp,513);
            cs = FALSE;
            retval = FALSE;
            DosDateTimeToFileTime(g_iai.ifh[i].dirent.cdate,g_iai.ifh[i].dirent.ctime,&lft);
            FileTimeToLocalFileTime(&lft,&cft);
            DosDateTimeToFileTime(g_iai.ifh[i].dirent.mdate,g_iai.ifh[i].dirent.mtime,&lft);
            FileTimeToLocalFileTime(&lft,&aft);
            DosDateTimeToFileTime(g_iai.ifh[i].dirent.mdate,g_iai.ifh[i].dirent.mtime,&lft);
            FileTimeToLocalFileTime(&lft,&mft);
            {
                param64.dwStructSize = sizeof(ENUM_MEMBER_INFO64);
                param64.uCommand = UNIMP_EXTRACT_COMMAND;
                param64.llOriginalSize = g_iai.ifh[i].orgsize;
                param64.llCompressedSize = g_iai.ifh[i].compsize;
                param64.dwAttributes = GetAttributes(g_iai.ifh[i].dirent.attrib);
                param64.dwCRC = 0;
                param64.uOSType = 0;
                param64.wRatio = 0;
                param64.ftCreateTime = cft;
                param64.ftAccessTime = aft;
                param64.ftWriteTime = mft;
                lstrcpy(param64.szFileName,g_iai.ifh[i].filename);
                lstrcpy(param64.szAddFileName,outfilename);
                cs = libarcdll_SendEnumMembersProc2(&param64,&retval);
            }
            if (cs == TRUE && retval == FALSE)
            {
                check = false;
            }
            if (check == true)
            {
                lstrcpy(temp,param64.szAddFileName);
                _fullpath(outfilename,temp,513);
            }
        }
check_end:
        if (check == true)
        {
            g_nStatus = -1;
            g_iai_number = i;
            if (g_window != NULL)
            {
                libarcdll_AbbreviationPath(g_iai.ifh[i].filename,ap,40,0,3);
                SetDlgItemText(g_window,IDC_FILENAME,ap);
                libarcdll_AbbreviationPath(outfilename,ap,40,0,3);
                SetDlgItemText(g_window,IDC_OUTFILENAME,ap);
            }
            current_update_mode = true;
            if (_stat(outfilename,&fdata) == 0)
            {
                if (answer_yes == 1)
                {
                    user_set = 1;
                }
                switch (user_set)
                {
                case 0:
                    libarcdll_GetFileTime(outfilename,NULL,NULL,&ctime,TRUE);
                    switch (update_file)
                    {
                    case 0:
                        current_update_mode = true;
                        break;
                    case 1:
                        if (CompareFileTime(&mtime,&ctime) > 0)
                        {
                            current_update_mode = true;
                        }
                        break;
                    case 2:
                        if (CompareFileTime(&mtime,&ctime) < 0)
                        {
                            current_update_mode = true;
                        }
                        break;
                    case 3:
                        if (CompareFileTime(&mtime,&ctime) != 0)
                        {
                            current_update_mode = true;
                        }
                        break;
                    case 4:
                        user_set = 2;
                        current_update_mode = false;
                        break;
                    case 5:
                        lstrcpy(ui.in,relpath);
                        lstrcpy(ui.out,outfilename);
                        ui.hWnd = g_window;
                        switch (DialogBoxParam(g_hInst,MAKEINTRESOURCE(IDD_UPDATE),g_window,(DLGPROC)UpdateDialogProc,(LPARAM)&ui))
                        {
                        case 1:
                            user_set = 1;
                        case 0:
                            current_update_mode = true;
                            break;
                        case 3:
                            user_set = 2;
                        case 2:
                            current_update_mode = false;
                            break;
                        }
                        break;
                    }
                    break;
                case 1:
                    current_update_mode = true;
                    break;
                case 2:
                    current_update_mode = false;
                    break;
                }
            }
            if (current_update_mode == true)
            {
                remove(outfilename);
                sprintf(message,"Extracting %s\n",g_iai.ifh[i].filename);
                libarcdll_AddMessage(hMessage,message);
                _beginthread(Extracting_Thread,0,(void*)outfilename);
                while (g_nStatus == -1)
                {
                    libarcdll_doevents();
                }
                if (g_nStatus != 0)
                {
                    remove(outfilename);
                    g_Imp.close_archive();
                    if (g_window != NULL)
                    {
                        DestroyWindow(g_window);
                    }
                    free(g_iai.ifh);
                    return g_Imp.get_last_error();
                }
                mfc++;
                if (save_attribute == 0)
                {
                    SetFileAttributes(outfilename,FILE_ATTRIBUTE_ARCHIVE);
                }
                else
                {
                    dwAttribute = GetAttributes(g_iai.ifh[i].dirent.attrib);
                    SetFileAttributes(outfilename,dwAttribute);
                }
                DosDateTimeToFileTime(g_iai.ifh[i].dirent.cdate,g_iai.ifh[i].dirent.ctime,&ctime);
                FileTimeToLocalFileTime(&ctime,&ctime_local);
                DosDateTimeToFileTime(g_iai.ifh[i].dirent.cdate,g_iai.ifh[i].dirent.ctime,&mtime);
                FileTimeToLocalFileTime(&mtime,&mtime_local);
                libarcdll_SetFileTime(outfilename,ctime_local,mtime_local,mtime_local);
                if (extract_dos_name == 1)
                {
                    GetShortPathName(outfilename,name83,MAX_PATH * 2);
                    rename(outfilename,name83);
                }
            }
            else
            {
                if (write_skip == 0)
                {
                    sprintf(message,"Skipping %s\n",g_iai.ifh[i].filename);
                    libarcdll_AddMessage(hMessage,message);
                }
            }
            if (g_window != NULL)
            {
                per = i * 100 / g_iai.file_count;
                SendDlgItemMessage(g_window,IDC_PROGRESS,PBM_SETPOS,per,0L);
            }
            libarcdll_doevents();
        }
        else
        {
            if (write_skip == 0)
            {
                sprintf(message,"Skipping %s\n",g_iai.ifh[i].filename);
                libarcdll_AddMessage(hMessage,message);
            }
        }
        if (bCancel == true)
        {
            i++;
            for (;i < g_iai.file_count;i++)
            {
                sprintf(message,"Canceled %s\n",g_iai.ifh[i].filename);
                libarcdll_AddMessage(hMessage,message);
            }
            break;
        }
    }
    g_Imp.close_archive();
    if (g_window != NULL)
    {
        DestroyWindow(g_window);
    }
    free(g_iai.ifh);
    if (bCancel == false && mfc == 0)
    {
        return ERROR_NOT_FIND_FILE;
    }
    return (bCancel == true) ? ERROR_USER_CANCEL : 0;
}
