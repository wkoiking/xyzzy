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
// $Id: Sfx.cpp,v 1.12 2005/05/22 20:41:18 hyperbeat Exp $
//

#include <libarcdll.h>
#include <malloc.h>
#include "UnImp.h"
#include "Common.h"
#include "UnImp32.h"
#include "resource.h"

#define BUFFERSIZE 131072

extern HINSTANCE g_hInst; // インスタンスハンドル

// 自己展開書庫作成
// [S] [sfxname] [arcname] [sfxmodule]
int Sfx(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage)
{
    int ccc;
    uint rsize;
    char *arcname;
    char *sfxname;
    char *sfxmodulename;
    char *sfxmodule;
    char *message;
    BYTE *buffer;
    CUnImp Imp;
    FILE *ofp;
    FILE *ifp;
    DWORD resource_size;
    IMP_ARCHIVE_INFO iai;
    ccc = libarcdll_GetOtherDataCount(hCmdLine);
    if (ccc != 2 && ccc != 3)
    {
        return ERROR_COMMAND_NAME;
    }
    message = (char*)malloc(4098);
    arcname = (char*)malloc(513);
    sfxname = (char*)malloc(513);
    sfxmodule = (char*)malloc(513);
    libarcdll_GetOtherData(hCmdLine,0,"%s",sfxname);
    libarcdll_GetOtherData(hCmdLine,1,"%s",arcname);
    if (ccc == 3)
    {
        libarcdll_GetOtherData(hCmdLine,2,"%s",sfxmodule);
    }
    else
    {
        lstrcpy(sfxmodule,"");
    }
    if (Imp.open_archive(arcname,&iai) == false)
    {
        sprintf(message,"%sは書庫ではありません。",arcname);
        libarcdll_AddMessage(hMessage,message);
        free(arcname);
        free(message);
        free(sfxname);
        free(sfxmodule);
        return Imp.get_last_error();
    }
    Imp.close_archive();
    sprintf(message,"%s OK\n",arcname);
    libarcdll_AddMessage(hMessage,message);
    sfxmodulename = (char*)malloc(513);
    if (ccc == 2)
    {
        char *tp;
        FILE *fp;
        HGLOBAL hResData;
        LPVOID lpData;
        HRSRC hResInfo;
        hResInfo = FindResource(g_hInst,MAKEINTRESOURCE(IDM_IMPSFX),"MODULE");
        if (hResInfo == NULL)
        {
            free(arcname);
            free(sfxname);
            free(message);
            free(sfxmodulename);
            free(sfxmodule);
            return ERROR_UNKNOWN;
        }
        hResData = LoadResource(g_hInst,hResInfo);
        if (hResData == NULL)
        {
            free(arcname);
            free(sfxname);
            free(message);
            free(sfxmodulename);
            free(sfxmodule);
            return ERROR_UNKNOWN;
        }
        resource_size = SizeofResource(g_hInst,hResInfo);
        lpData = LockResource(hResData);
        if (lpData == NULL)
        {
            free(arcname);
            free(sfxname);
            free(message);
            free(sfxmodulename);
            free(sfxmodule);
            return ERROR_UNKNOWN;
        }
        tp = (char*)malloc(513);
        GetTempPath(513,tp);
        GetTempFileName(tp,"UIM",0,sfxmodulename);
        remove(sfxmodulename);
        if ((fp = fopen(sfxmodulename,"wb")) == NULL)
        {
            free(arcname);
            free(sfxname);
            free(message);
            free(sfxmodulename);
            free(sfxmodule);
            free(tp);
            return ERROR_TMP_OPEN;
        }
        if (fwrite(lpData,1,resource_size,fp) != resource_size)
        {
            fclose(fp);
            remove(sfxmodulename);
            free(arcname);
            free(sfxname);
            free(message);
            free(sfxmodulename);
            free(sfxmodule);
            free(tp);
            return ERROR_CANNOT_WRITE;
        }
        free(tp);
        fclose(fp);
    }
    else
    {
        // 指定したモジュールを使用
        lstrcpy(sfxmodulename,sfxmodule);
    }
    if ((ofp = fopen(sfxname,"wb")) == NULL)
    {
        free(arcname);
        free(sfxname);
        free(message);
        if (ccc == 2)
        {
            remove(sfxmodulename);
        }
        free(sfxmodulename);
        free(sfxmodule);
        return ERROR_FILE_OPEN;
    }
    if ((ifp = fopen(sfxmodulename,"rb")) == NULL)
    {
        fclose(ofp);
        remove(sfxname);
        free(arcname);
        free(sfxname);
        free(message);
        if (ccc == 2)
        {
            remove(sfxmodulename);
        }
        free(sfxmodulename);
        free(sfxmodule);
        return ERROR_FILE_OPEN;
    }
    buffer = (BYTE*)malloc(BUFFERSIZE);
    if (buffer == NULL)
    {
        fclose(ofp);
        remove(sfxname);
        fclose(ifp);
        free(arcname);
        free(sfxname);
        free(message);
        if (ccc == 2)
        {
            remove(sfxmodulename);
        }
        free(sfxmodulename);
        free(sfxmodule);
        return ERROR_ENOUGH_MEMORY;
    }
    while ((rsize = fread(buffer,1,BUFFERSIZE,ifp)) != 0)
    {
        if (fwrite(buffer,1,rsize,ofp) != rsize)
        {
            fclose(ofp);
            remove(sfxname);
            fclose(ifp);
            free(buffer);
            free(arcname);
            free(sfxname);
            free(message);
            if (ccc == 2)
            {
                remove(sfxmodulename);
            }
            free(sfxmodulename);
            free(sfxmodule);
            return ERROR_CANNOT_WRITE;
        }
    }
    fclose(ifp);
    if ((ifp = fopen(arcname,"rb")) == NULL)
    {
        fclose(ofp);
        remove(sfxname);
        free(buffer);
        free(arcname);
        free(sfxname);
        free(message);
        if (ccc == 2)
        {
            remove(sfxmodulename);
        }
        free(sfxmodulename);
        free(sfxmodule);
        return ERROR_FILE_OPEN;
    }
    while ((rsize = fread(buffer,1,BUFFERSIZE,ifp)) != 0)
    {
        if (fwrite(buffer,1,rsize,ofp) != rsize)
        {
            fclose(ofp);
            remove(sfxname);
            fclose(ifp);
            free(buffer);
            free(arcname);
            free(sfxname);
            free(message);
            if (ccc == 2)
            {
                remove(sfxmodulename);
            }
            free(sfxmodulename);
            free(sfxmodule);
            return ERROR_CANNOT_WRITE;
        }
    }
    fclose(ifp);
    fclose(ofp);
    free(buffer);
    sprintf(message,"\n%s OK\n",sfxname);
    libarcdll_AddMessage(hMessage,message);
    free(arcname);
    free(sfxname);
    free(message);
    if (ccc == 2)
    {
        remove(sfxmodulename);
    }
    free(sfxmodulename);
    free(sfxmodule);
    return 0;
}
