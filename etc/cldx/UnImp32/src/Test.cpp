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
// $Id: Test.cpp,v 1.16 2005/06/02 14:10:25 hyperbeat Exp $
//

#include <libarcdll.h>
#include <malloc.h>
#include "resource.h"
#include "UnImp.h"
#include "Common.h"
#include "UnImp32.h"

DWORD GetAttributes(WORD src);

// ƒeƒXƒg
int Test(HWND hwnd,HANDLE hCmdLine,HANDLE hMessage)
{
    uint32 i;
    BOOL cs;
    BOOL retval;
    int j;
    int mfc = 0;
    int recursive = 0;
    char *arcname;
    char *filepath;
    char *message;
    bool check;
    CUnImp Imp;
    FILETIME lft;
    FILETIME cft;
    FILETIME aft;
    FILETIME mft;
    ENUM_MEMBER_INFO64 param64;
    IMP_ARCHIVE_INFO iai;
    arcname = (char*)malloc(513);
    libarcdll_GetOtherData(hCmdLine,0,"%s",arcname);
    if (Imp.open_archive(arcname,&iai) == false)
    {
        return Imp.get_last_error();
    }
    message = (char*)malloc(8096);
    sprintf(message,"Testing of %s\n\n",arcname);
    libarcdll_AddMessage(hMessage,message);
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
                param64.uCommand = UNIMP_TEST_COMMAND;
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
                cs = libarcdll_SendEnumMembersProc(&param64,&retval);
            }
            if (cs == TRUE && retval == FALSE)
            {
                check = false;
            }
        }
        if (check == true)
        {
            mfc++;
            sprintf(message,"Checking %s CRC OK\n",iai.ifh[i].filename);
            libarcdll_AddMessage(hMessage,message);
        }
    }
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
