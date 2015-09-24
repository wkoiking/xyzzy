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
// $Id: UnImp32.cpp,v 1.21 2005/06/17 15:45:39 hyperbeat Exp $
//

#include <io.h>
#include <libarcdll.h>
#include <malloc.h>
#include "UnImp.h"
#include "Common.h"
#include "UnImp32.h"

LPVOID CALLBACK OpenArchive(const char *arcname,LPVOID userdata);
BOOL CALLBACK CloseArchive(LPVOID ah,LPVOID userdata);
int CALLBACK ReadHeader(LPVOID ah,FileHeader *hdr,LPVOID userdata);
int CALLBACK Command(LPVOID ah,BYTE command,const FileHeader *hdr,const OpenArchiveInfoDet mi,LPVOID userdata);

int Add(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage);
int Extract(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage,bool path);
int List(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage,bool path);
int Sfx(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage);
int Test(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage);

HINSTANCE g_hInst; // インスタンスハンドル
extern BOOL g_debug; // デバッグモード

extern "C"
{

BOOL APIENTRY DllMain(HANDLE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        g_hInst = (HINSTANCE)hinstDLL;
        g_debug = FALSE;
        libarcdll_Init(g_hInst);
        break;
    case DLL_PROCESS_DETACH:
        libarcdll_Free();
        break;
    }
    return TRUE;
}

libarcdll_CAL_API(UnImp,g_hInst,"UnImp",OpenArchive,CloseArchive,ReadHeader,Command)

// コマンドラインで書庫を作成、操作などを行います。
int WINAPI UnImp(const HWND _hwnd,LPCSTR _szCmdLine,LPSTR _szOutput,const DWORD _dwSize)
{
    int i;
    int r;
    int c;
    char *logfilename;
    char *command;
    HWND window;
    HANDLE cmdline;
    HANDLE message;
    CommandLineOption opt;
    message = libarcdll_InitMessage(_dwSize);
    libarcdll_AddMessage(message,"UnImp Copyright (C) 1999 - 2001 Technelysium Pty Ltd\n");
    libarcdll_AddMessage(message,"UnImp32.dll Copyright (C) 2002 - HyperBeat\n\n");
    window = (_hwnd != NULL) ? _hwnd : GetDesktopWindow();
    opt.canusecommand_len = 7;
    opt.canusecommand[0] = strdup("A");
    opt.canusecommand[1] = strdup("E");
    opt.canusecommand[2] = strdup("L");
    opt.canusecommand[3] = strdup("V");
    opt.canusecommand[4] = strdup("S");
    opt.canusecommand[5] = strdup("T");
    opt.canusecommand[6] = strdup("X");
    opt.defaultcommand = 6;
    opt.useswitch = TRUE;
    opt.switchkey_len = 2;
    opt.switchkey[0] = strdup("-");
    opt.switchkey[1] = strdup("/");
    lstrcpy(opt.endswitchkey,"-");
    opt.useresponsefile = TRUE;
    lstrcpy(opt.responsekey,"@");
    cmdline = libarcdll_CreateCommandLineInfo(_szCmdLine,opt);
    if (cmdline == NULL)
    {
        for (i = 0;i < 7;i++)
        {
            if (i < 2)
            {
                free(opt.switchkey[i]);
            }
            free(opt.canusecommand[i]);
        }
        return ERROR_COMMAND_NAME;
    }
    command = (char*)malloc(10);
    libarcdll_GetCommand(cmdline,command);
    if (lstrcmpi(command,"A") == 0)
    {
        // 格納
        r = Add(_hwnd,cmdline,message);
    }
    else if ((c = lstrcmpi(command,"E")) == 0 || lstrcmpi(command,"X") == 0)
    {
        // 展開
        r = Extract(_hwnd,cmdline,message,(c == 0) ? FALSE : TRUE);
    }
    else if ((c = lstrcmpi(command,"L")) == 0 || lstrcmpi(command,"V") == 0)
    {
        // リスト
        r = List(_hwnd,cmdline,message,(c == 0) ? FALSE : TRUE);
    }
    else if (lstrcmpi(command,"S") == 0)
    {
        // 自己展開書庫作成
        r = Sfx(_hwnd,cmdline,message);
    }
    else if (lstrcmpi(command,"T") == 0)
    {
        // 正当性検査
        r = Test(_hwnd,cmdline,message);
    }
    free(command);
    if (r != 0)
    {
        command = (char*)malloc(100);
        sprintf(command,"\nError: Code = %X\n",r);
        libarcdll_AddMessage(message,command);
        free(command);
    }
    if (_dwSize != 0)
    {
        libarcdll_GetMessage(message,_szOutput,_dwSize);
    }
    logfilename = (char*)malloc(513);
    if (libarcdll_GetSwitch(cmdline,NULL,"gl",NULL,NULL,NULL,"%s",logfilename) == TRUE)
    {
        libarcdll_SaveMessage(message,logfilename);
    }
    libarcdll_FreeMessage(message);
    free(logfilename);
    libarcdll_FreeCommandLineInfo(cmdline);
    return r;
}

// DLLの設定
BOOL WINAPI UnImpConfigDialog(const HWND _hwnd,LPSTR _szOutputBuffer,const int _iMode)
{
    MessageBox(_hwnd,"UnImp32.dll Copyright (C) 2002- HyperBeat.","UnImp32.dll",MB_OK);
    return FALSE;
}

// メモリバッファへ展開
int WINAPI UnImpExtractMem(const HWND _hwnd,LPCSTR _szCmdLine,LPBYTE _szBuffer,const DWORD _dwSize,time_t *_lpTime,LPWORD _lpwAttr,LPDWORD _lpdwWriteSize)
{
    return 1;
}

// メモリバッファの内容を圧縮して書庫へ格納
int WINAPI UnImpCompressMem(const HWND _hwnd,LPCSTR _szCmdLine,const LPBYTE _szBuffer,const DWORD _dwSize,const time_t *_lpTime,const LPWORD _lpwAttr,LPDWORD _lpdwWriteSize)
{
    return 1;
}

// Dummy API

libarcdll_CAL_API(Unimp,g_hInst,"Unimp",OpenArchive,CloseArchive,ReadHeader,Command)

// コマンドラインで書庫を作成、操作などを行います。
int WINAPI Unimp(const HWND _hwnd,LPCSTR _szCmdLine,LPSTR _szOutput,const DWORD _dwSize)
{
    return UnImp(_hwnd,_szCmdLine,_szOutput,_dwSize);
}

// DLLの設定
BOOL WINAPI UnimpConfigDialog(const HWND _hwnd,LPSTR _szOutputBuffer,const int _iMode)
{
    MessageBox(_hwnd,"UnImp32.dll Copyright (C) 2002- HyperBeat.","UnImp32.dll",MB_OK);
    return FALSE;
}

// メモリバッファへ展開
int WINAPI UnimpExtractMem(const HWND _hwnd,LPCSTR _szCmdLine,LPBYTE _szBuffer,const DWORD _dwSize,time_t *_lpTime,LPWORD _lpwAttr,LPDWORD _lpdwWriteSize)
{
    return 1;
}

// メモリバッファの内容を圧縮して書庫へ格納
int WINAPI UnimpCompressMem(const HWND _hwnd,LPCSTR _szCmdLine,const LPBYTE _szBuffer,const DWORD _dwSize,const time_t *_lpTime,const LPWORD _lpwAttr,LPDWORD _lpdwWriteSize)
{
    return 1;
}

// デバッグ用API
void WINAPI UnImpSetDebugMode(BOOL _bMode)
{
    if (_bMode == TRUE)
    {
        g_debug = TRUE;
    }
    else
    {
        g_debug = FALSE;
    }
}

}
