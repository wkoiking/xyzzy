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
// $Id: List.cpp,v 1.18 2005/06/03 13:17:19 hyperbeat Exp $
//

#include <io.h>
#include <libarcdll.h>
#include <malloc.h>
#include "resource.h"
#include "UnImp.h"
#include "Common.h"
#include "UnImp32.h"

typedef unsigned char uchar;

void cut_dir(char *src,char *dest)
{
    int i;
    char *p;
    if ((p = strrchr(src,'\\')) == NULL)
    {
        lstrcpy(dest,src);
    }
    else
    {
        for (i = 0,p++;p < src + lstrlen(src);i++,p++)
        {
            *(dest + i) = *p;
        }
        *(dest + i) = '\0';
    }
}

DWORD GetAttributes(WORD src)
{
    DWORD dwAttribute = 0;
    if (src & _A_ARCH)
    {
        dwAttribute |= FILE_ATTRIBUTE_ARCHIVE;
    }
    if (src & _A_HIDDEN)
    {
        dwAttribute |= FILE_ATTRIBUTE_HIDDEN;
    }
    if (src & _A_RDONLY)
    {
        dwAttribute |= FILE_ATTRIBUTE_READONLY;
    }
    if (src & _A_SUBDIR)
    {
        dwAttribute |= FILE_ATTRIBUTE_DIRECTORY;
    }
    if (src & _A_SYSTEM)
    {
        dwAttribute |= FILE_ATTRIBUTE_SYSTEM;
    }
    return dwAttribute;
}

// ƒŠƒXƒg
// [L,V] [ArchiveName] [Path] ...
int List(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage,bool path)
{
    int mfc = 0;
    char method[8];
    char ctime_str[128];
    char mtime_str[128];
    char fname[MAX_PATH * 2];
    uint32 i;
    uint32 filesize = 0;
    int j;
    int recursive = 0;
    char *arcname;
    char *filepath;
    char *message;
    bool check;
    CUnImp Imp;
    ENUM_MEMBER_INFO64 param64;
    BOOL cs;
    BOOL retval;
    FILETIME lft;
    FILETIME cft;
    FILETIME aft;
    FILETIME mft;
    IMP_ARCHIVE_INFO iai;
    arcname = (char*)malloc(513);
    libarcdll_GetOtherData(hCmdLine,0,"%s",arcname);
    if (Imp.open_archive(arcname,&iai) == false)
    {
        free(arcname);
        return Imp.get_last_error();
    }
    message = (char*)malloc(8096);
    sprintf(message,"Listing of %s\n",arcname);
    libarcdll_AddMessage(hMessage,message);
    libarcdll_AddMessage(hMessage,"-----------------------------------------------------------------------\n");
    libarcdll_AddMessage(hMessage,"FileName\n    FileSize    Creation Time       Modified Time        CRC     Method\n");
    libarcdll_AddMessage(hMessage,"-----------------------------------------------------------------------\n");
    libarcdll_GetSwitch(hCmdLine,NULL,"r",NULL,NULL,NULL,"%d",&recursive);
    for (i = 0;i < iai.file_count;i++)
    {
        check = false;
        if (libarcdll_GetOtherDataCount(hCmdLine) >= 2)
        {
            filepath = (char*)malloc(513);
            for (j = 1;j < libarcdll_GetOtherDataCount(hCmdLine);j++)
            {
                libarcdll_GetOtherData(hCmdLine,j,"%s",filepath);
                if (recursive == 0)
                {
                    if (libarcdll_ComparisonWildcard2(filepath,iai.ifh[i].filename,COMPARISON_MODE_SAMEDIRECTORY) == TRUE)
                    {
                        check = true;
                    }
                }
                else
                {
                    if (libarcdll_ComparisonWildcard2(filepath,iai.ifh[i].filename,COMPARISON_MODE_RECURSIVE) == TRUE)
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
            else if (recursive == 0 && libarcdll_GetCharCount(iai.ifh[i].filename,'\\') == 0)
            {
                check = true;
            }
        }
        if (check == true)
        {
            cs = FALSE;
            retval = FALSE;
            DosDateTimeToFileTime(iai.ifh[i].dirent.cdate,iai.ifh[i].dirent.ctime,&lft);
            FileTimeToLocalFileTime(&lft,&cft);
            DosDateTimeToFileTime(iai.ifh[i].dirent.mdate,iai.ifh[i].dirent.mtime,&lft);
            FileTimeToLocalFileTime(&lft,&aft);
            DosDateTimeToFileTime(iai.ifh[i].dirent.mdate,iai.ifh[i].dirent.mtime,&lft);
            FileTimeToLocalFileTime(&lft,&mft);
            {
                param64.dwStructSize = sizeof(ENUM_MEMBER_INFO64);
                param64.uCommand = UNIMP_LIST_COMMAND;
                param64.llOriginalSize = iai.ifh[i].orgsize;
                param64.llCompressedSize = iai.ifh[i].compsize;
                param64.dwAttributes = GetAttributes(iai.ifh[i].dirent.attrib);
                param64.dwCRC = 0;
                param64.uOSType = 0;
                param64.wRatio = 0;
                param64.ftCreateTime = cft;
                param64.ftAccessTime = aft;
                param64.ftWriteTime = mft;
                lstrcpy(param64.szFileName,iai.ifh[i].filename);
                lstrcpy(param64.szAddFileName,"");
                cs = libarcdll_SendEnumMembersProc2(&param64,&retval);
            }
            if (cs == TRUE && retval == FALSE)
            {
                check = false;
            }
        }
        if (check == true)
        {
            mfc++;
            if (path == false)
            {
                cut_dir(iai.ifh[i].filename,fname);
            }
            else
            {
                lstrcpy(fname,iai.ifh[i].filename);
            }
            sprintf(message,"%s\n",fname);
            libarcdll_AddMessage(hMessage,message);
            time_to_string(iai.ifh[i].dirent.cdate,iai.ifh[i].dirent.ctime,ctime_str);
            time_to_string(iai.ifh[i].dirent.mdate,iai.ifh[i].dirent.mtime,mtime_str);
            if (iai.ifh[i].dirent.flags & DFLAG_DIR)
            {
                lstrcpy(method,"IMPD");
            }
            else
            {
                lstrcpy(method,"IMP1");
            }
            filesize += iai.ifh[i].dirent.esize;
            sprintf(message," %11ld %s %s   %08X   %s\n",iai.ifh[i].dirent.esize,ctime_str,mtime_str, iai.ifh[i].dirent.crc,method);
            libarcdll_AddMessage(hMessage,message);
        }
    }
    libarcdll_AddMessage(hMessage,"-----------------------------------------------------------------------\n");
    sprintf(message," %11d\n",filesize);
    libarcdll_AddMessage(hMessage,message);
    libarcdll_AddMessage(hMessage,"-----------------------------------------------------------------------\n");
    Imp.close_archive();
    free(iai.ifh);
    free(arcname);
    free(message);
    if (mfc == 0)
    {
        return ERROR_NOT_FIND_FILE;
    }
    return 0;
}
