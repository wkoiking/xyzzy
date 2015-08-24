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
// $Id: ImpSfx.cpp,v 1.5 2005/04/28 13:36:16 hyperbeat Exp $
//

#include <io.h>
#include <malloc.h>
#include <process.h>
#include <windows.h>
#include <shlobj.h>
#include "../UnImp.h"
#include "resource.h"

BROWSEINFO BI;

bool extract_flag;

BOOL SetFileTimeEx(char *filename,FILETIME ctime,FILETIME atime,FILETIME mtime)
{
    BOOL r;
    HANDLE hFile;
    hFile = CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
    r = SetFileTime(hFile,&ctime,&atime,&mtime);
    CloseHandle(hFile);
    return r;
}

int CALLBACK BrowseCallbackProc(HWND hWnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
    char Path[MAX_PATH * 2];
    if (BI.ulFlags & BIF_STATUSTEXT)
    {
        if (uMsg == BFFM_SELCHANGED)
        {
            SHGetPathFromIDList((LPCITEMIDLIST)lParam,Path);
            SendMessage(hWnd,BFFM_SETSTATUSTEXT,0,(LPARAM)Path);
        }
    }
    return 0;
}

bool DirectoryRefDialog(HWND hWnd,char *Result)
{
    bool r = false;
    LPMALLOC Memory;
    LPCSTR Buffer;
    LPITEMIDLIST Ret;
    LPITEMIDLIST Root;
    SHGetMalloc(&Memory);
    Buffer = (LPSTR)Memory->Alloc(1000);
    SHGetSpecialFolderLocation(hWnd,CSIDL_DESKTOP,&Root);
    ZeroMemory(&BI,sizeof(BROWSEINFO));
    BI.hwndOwner = hWnd;
    BI.pidlRoot = Root;
    BI.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS;
    BI.lpszTitle = "ìWäJêÊÇéwíËÇµÇƒÇ≠ÇæÇ≥Ç¢";
    BI.pszDisplayName = (LPSTR)Buffer;
    BI.lpfn = BrowseCallbackProc;
    Ret = SHBrowseForFolder(&BI);
    if (SHGetPathFromIDList(Ret,(LPSTR)Buffer))
    {
        lstrcpy(Result,Buffer);
        r = true;
    }
    Memory->Free(Ret);
    Memory->Free(Root);
    Memory->Free((void*)Buffer);
    return r;
}

bool check_archive(void)
{
    char arcname[MAX_PATH * 2];
    CUnImp Imp;
    IMP_ARCHIVE_INFO iai;
    GetModuleFileName(GetModuleHandle(NULL),arcname,MAX_PATH * 2);
    if (Imp.open_archive(arcname,&iai) == false)
    {
        return false;
    }
    Imp.close_archive();
    free(iai.ifh);
    return true;
}

void extract_archive(void *hWnd)
{
    uint i;
    char arcname[MAX_PATH * 2];
    char outdir[MAX_PATH * 2];
    char outfilename[MAX_PATH * 2];
    CUnImp Imp;
    IMP_ARCHIVE_INFO iai;
    DWORD dwAttribute;
    FILETIME ctime;
    FILETIME ctime_local;
    FILETIME mtime;
    FILETIME mtime_local;
    GetModuleFileName(GetModuleHandle(NULL),arcname,MAX_PATH * 2);
    if (Imp.open_archive(arcname,&iai) == false)
    {
        MessageBox((HWND)hWnd,"é©å»ìWäJèëå…Ç≈ÇÕÇ†ÇËÇ‹ÇπÇÒÅB","IMP Self Extract Archiver",MB_OK);
        extract_flag = false;
        _endthread();
    }
    GetDlgItemText((HWND)hWnd,IDC_OUTDIR,outdir,MAX_PATH * 2);
    if (outdir[lstrlen(outdir) - 1] != '\\')
    {
        lstrcat(outdir,"\\");
    }
    for (i = 0;i < iai.file_count;i++)
    {
        sprintf(outfilename,"%s%s",outdir,iai.ifh[i].filename);
        SetDlgItemText((HWND)hWnd,IDC_FILENAME,iai.ifh[i].filename);
        if (Imp.extract_file(iai.ifh[i].dirent,outfilename) == false)
        {
            MessageBox((HWND)hWnd,"ìWäJÇ…é∏îsÇµÇ‹ÇµÇΩÅB","IMP Self Extract Archiver",MB_OK);
            Imp.close_archive();
            free(iai.ifh);
            extract_flag = false;
            _endthread();
        }
        dwAttribute = 0;
        if (iai.ifh[i].dirent.attrib & _A_ARCH)
        {
            dwAttribute |= FILE_ATTRIBUTE_ARCHIVE;
        }
        if (iai.ifh[i].dirent.attrib & _A_HIDDEN)
        {
            dwAttribute |= FILE_ATTRIBUTE_HIDDEN;
        }
        if (iai.ifh[i].dirent.attrib & _A_RDONLY)
        {
            dwAttribute |= FILE_ATTRIBUTE_READONLY;
        }
        if (iai.ifh[i].dirent.attrib & _A_SUBDIR)
        {
            dwAttribute |= FILE_ATTRIBUTE_DIRECTORY;
        }
        if (iai.ifh[i].dirent.attrib & _A_SYSTEM)
        {
            dwAttribute |= FILE_ATTRIBUTE_SYSTEM;
        }
        SetFileAttributes(outfilename,dwAttribute);
        DosDateTimeToFileTime(iai.ifh[i].dirent.cdate,iai.ifh[i].dirent.ctime,&ctime);
        FileTimeToLocalFileTime(&ctime,&ctime_local);
        DosDateTimeToFileTime(iai.ifh[i].dirent.cdate,iai.ifh[i].dirent.ctime,&mtime);
        FileTimeToLocalFileTime(&mtime,&mtime_local);
        SetFileTimeEx(outfilename,ctime_local,mtime_local,mtime_local);
    }
    Imp.close_archive();
    free(iai.ifh);
    MessageBox((HWND)hWnd,"ìWäJÇ…ê¨å˜ÇµÇ‹ÇµÇΩÅB","IMP Self Extract Archiver",MB_OK);
    extract_flag = false;
    _endthread();
}

BOOL CALLBACK MainDialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        if (check_archive() == false)
        {
            MessageBox(hWnd,"é©å»ìWäJèëå…Ç≈ÇÕÇ†ÇËÇ‹ÇπÇÒÅB","IMP Self Extract Archiver",MB_OK);
            EndDialog(hWnd,TRUE);
            return TRUE;
        }
        {
            char dir[MAX_PATH * 2];
            GetCurrentDirectory(MAX_PATH * 2,dir);
            SetDlgItemText(hWnd,IDC_OUTDIR,dir);
        }
        return TRUE;
    case WM_SYSCOMMAND:
        if (extract_flag == false && wParam == SC_CLOSE)
        {
            EndDialog(hWnd,TRUE);
            return TRUE;
        }
        return FALSE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_FINDDIR:
            if (extract_flag == false)
            {
                char dir[MAX_PATH * 2];
                if (DirectoryRefDialog(hWnd,dir) == true)
                {
                    SetDlgItemText(hWnd,IDC_OUTDIR,dir);
                }
            }
            return TRUE;
        case IDC_EXTRACT:
            if (extract_flag == false)
            {
                extract_flag = true;
                EnableWindow(GetDlgItem(hWnd,IDC_FINDDIR),FALSE);
                EnableWindow(GetDlgItem(hWnd,IDC_EXTRACT),FALSE);
                _beginthread(extract_archive,0,(void*)hWnd);
                while (extract_flag == true)
                {
                    MSG msg;
                    while (PeekMessage(&msg,(HWND)NULL,0,0,PM_REMOVE))
                    {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
                EnableWindow(GetDlgItem(hWnd,IDC_FINDDIR),TRUE);
                EnableWindow(GetDlgItem(hWnd,IDC_EXTRACT),TRUE);
                SetDlgItemText(hWnd,IDC_FILENAME,"");
                EndDialog(hWnd,TRUE);
            }
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

BOOL APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hInstPrev,LPSTR lpCmdLine,int nShowCmd)
{
    extract_flag = false;
    DialogBox(hInst,MAKEINTRESOURCE(IDD_MAIN),NULL,(DLGPROC)MainDialogProc);
    return TRUE;
}
