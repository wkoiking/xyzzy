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
// Copyright (c) 1999-2001 Technelysium Pty Ltd.
// All Right Reserved.
//
// Warning
// This source code may be incorporated into other programs only for the purpose of
// listing or extracting the contents of IMP archives.
//
// $Id: UnImp.cpp,v 1.20 2005/06/17 13:15:49 hyperbeat Exp $
//

#include <errno.h>
#include <direct.h>
#include <io.h>
#include <libarcdll.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifndef SFX
#include <shellapi.h>
#endif
#include "UnImp.h"
#ifndef SFX
#include "UnImp32.h"
#endif

#ifdef SFX
#define FIND_START_BUFFER 131072
#else
#define FIND_START_BUFFER 32768
#endif

BOOL g_debug; // デバッグモード

CUnImp::CUnImp()
{
    init();
}

CUnImp::~CUnImp()
{
    close_archive();
}

bool CUnImp::open_archive(const char *arc_name,IMP_ARCHIVE_INFO *iai)
{
    int errorcode;
    struct _stat fdata;
    if (g_debug == TRUE)
    {
        fprintf(stdout,"checkfilename(%s) ... ",arc_name);
    }
    if (lstrlen(arc_name) > MAX_PATH)
    {
        if (g_debug == TRUE)
        {
            fprintf(stdout,"error: bad file name: length = %d\n",lstrlen(arc_name));
        }
        set_last_error(EIMP_TOO_LONG_NAME);
        return false;
    }
    if (g_debug == TRUE)
    {
        fprintf(stdout,"ok\n");
        fprintf(stdout,"_stat(%s) ... ",arc_name);
    }
    if (_stat(arc_name,&fdata) == -1)
    {
        if (g_debug == TRUE)
        {
            fprintf(stdout,"error(%d)\n",errno);
        }
        set_last_error(EIMP_FIND_FILE);
        return false;
    }
    if (g_debug == TRUE)
    {
        fprintf(stdout,"ok\n");
        fprintf(stdout,"fopen(%s) ... ",arc_name);
    }
    if ((g_archfile = fopen(arc_name,"rb")) == NULL)
    {
        if (g_debug == TRUE)
        {
            fprintf(stdout,"error(%d)\n",errno);
        }
        set_last_error(EIMP_OPEN);
        return false;
    }
    if (g_debug == TRUE)
    {
        fprintf(stdout,"ok\n");
        fprintf(stdout,"make_list() ... ");
    }
    if ((errorcode = make_list(iai)) != 0)
    {
        if (g_debug == TRUE)
        {
            fprintf(stdout,"error(%d)\n",errorcode);
        }
        set_last_error(errorcode); 
        fclose(g_archfile);
        return false;
    }
    if (g_debug == TRUE)
    {
        fprintf(stdout,"ok\n");
    }
    return true;
}

bool CUnImp::open_archive(FILE *fp,IMP_ARCHIVE_INFO *iai)
{
    int errorcode;
    if (fp == NULL)
    {
        return false;
    }
    g_archfile = fp;
    if ((errorcode = make_list(iai)) != 0)
    {
        set_last_error(errorcode); 
        fclose(g_archfile);
        return false;
    }
    return true;
}

void CUnImp::close_archive(void)
{
    if (g_archfile != NULL)
    {
        fclose(g_archfile);
    }
}

#ifndef SFX

int CUnImp::is_exe(void)
{
    char temp_name[MAX_PATH * 2];
    FILE *fp;
    BYTE *buffer;
    DWORD r;
    SHFILEINFO sfi;
    sprintf(temp_name,"%s","sfx_test.dat");
    if (g_exesize == 0)
    {
        return 0;
    }
    fp = fopen(temp_name,"wb");
    if (fp == NULL)
    {
        return 0;
    }
    fseek(g_archfile,0,SEEK_SET);
    buffer = (BYTE*)malloc(g_exesize);
    if (fread(buffer,1,g_exesize,g_archfile) != g_exesize)
    {
        free(buffer);
        fclose(fp);
        remove(temp_name);
        return 0;
    }
    if (fwrite(buffer,1,g_exesize,fp) != g_exesize)
    {
        free(buffer);
        fclose(fp);
        remove(temp_name);
        return 0;
    }
    free(buffer);
    fclose(fp);
    r = SHGetFileInfo(temp_name,FILE_ATTRIBUTE_NORMAL,&sfi,sizeof(SHFILEINFO),SHGFI_EXETYPE);
    remove(temp_name);
    return r == 0 ? 0 : 1;
}

#endif

bool CUnImp::extract_file(IMP_DIR_ENTRY dirent,char *out_filename)
{
    char *p;
    int err;
    FILE *outfile;
    char dirname[MAX_PATH * 2];
    char _dnf[MAX_PATH * 2];
    if (dirent.flags & DFLAG_DIR)
    {
        sprintf(_dnf,"%s\\",out_filename);
        mkdir2(_dnf);
        return true;
    }
    lstrcpy(dirname,out_filename);
    p = strrchr(dirname,'\\');
    if (p != NULL)
    {
        *(p + 1) = '\0';
        if (mkdir2(dirname) == false)
        {
            set_last_error(EIMP_DIRECTORY);
            return false;
        }
    }
    if ((outfile = fopen(out_filename,"wb")) == NULL)
    {
        set_last_error(EIMP_MAKE_FILE);
        return false;
    }
    err = decomp(outfile,dirent.fpos,dirent.unitpos,dirent.esize,dirent.flags);
    fclose(outfile);
    if (err < 0)
    {
        remove(out_filename);
        set_last_error(err);
        return false;
    }
    if (dirent.crc != g_crc)
    {
        remove(out_filename);
        set_last_error(EIMP_BAD_CRC);
        return false;
    }
    return true;
}

int CUnImp::get_last_error(void)
{
#ifndef SFX
    return convert_imperror(g_last_error);
#else
    return g_last_error;
#endif
}

void CUnImp::init(void)
{
    g_archfile = NULL;
    g_ncodes[0] = NDCODES;
    g_ncodes[1] = ND2CODES;
    g_ncodes[2] = ND3CODES;
    g_ncoffset[0] = NLLCODES;
    g_ncoffset[1] = NLLCODES + NDCODES;
    g_ncoffset[2] = NLLCODES + NDCODES + ND2CODES;
    g_exesize = 0;
    g_overlaps[0][0] = g_overlaps[1][0] = 32768;
    g_overlaps[0][1] = g_overlaps[1][1] = 65536;
    g_overlaps[0][2] = g_overlaps[1][2] = 73728;
    g_overlaps[0][3] = g_overlaps[1][3] = 98304;
    g_overlaps[0][4] = g_overlaps[1][4] = 122880;
    g_overlaps[0][5] = g_overlaps[1][5] = 147456;
    g_overlaps[0][6] = g_overlaps[1][6] = 172032;
    g_overlaps[0][7] = g_overlaps[1][7] = 196608;
    g_overlaps[2][0] = 32768;
    g_overlaps[2][1] = 131072;
    g_overlaps[2][2] = 196608;
    g_overlaps[2][3] = 262144;
    g_overlaps[2][4] = 327680;
    g_overlaps[2][5] = 393216;
    g_overlaps[2][6] = 458752;
    g_overlaps[2][7] = MAXOVERLAP;
    g_dirsig[0] = 0x49;
    g_dirsig[1] = 0x4D;
    g_dirsig[2] = 0x50;
    g_dirsig[3] = 0x44;
    g_dirsig[4] = 0x45;
    g_dirsig[5] = 0;
    g_last_error = 0;
    init_crc_table();
}

void CUnImp::init_crc_table(void)
{
    int i;
    for (i = 0;i < 256;i++)
    {
        uint32 crc = i;
        char c;
        for (c = 8;c;c--)
        {
            crc = crc & 1 ? 0xEDB88320L^(crc >> 1) : crc >> 1;
        }
        g_crctbl[i] = crc;
    }
}

uint32 CUnImp::block_crc(void *block,int32 n,uint32 crc)
{
    uchar *src,*end;
    for (src = (uchar*)block,end = src + n;src < end;src++)
    {
        crc = g_crctbl[(crc ^ *src) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}

int CUnImp::isJMS(const char *str,int nPos)
{
    int i;
    int state = 0;
    for (i = 0;*(str + i) != '\0';i++)
    {
        if ((state == 0) && (jms1(*(str + i))))
        {
            state = 1;
        }
        else if ((state == 1) && (jms2(*(str + i))))
        {
            state = 2;
        }
        else if ((state == 2) && (jms1(*(str + i))))
        {
            state = 1;
        }
        else
        {
            state = 0;
        }
        if (i == nPos)
        {
            return state;
        }
    }
    return 0;
}

void CUnImp::convert_path(char *path)
{
    int i;
    for (i = 0;*path;path++,i++)
    {
        if (isJMS(path - i,i) == 0 && *path == '/')
        {
            *path = '\\';
        }
    }
}

uint16 CUnImp::get_uint16(void *src)
{
    return *(uchar*)src + (((uchar*)src)[1] << 8);
}

uint32 CUnImp::get_uint32(void *src)
{
    int i;
    uint32 u = 0;
    for (i = 3;i >= 0;i--)
    {
        u = (u << 8) | ((uchar*)src)[i];
    }
    return u;
}

void CUnImp::init_getbits(void)
{
    g_dcsrc = g_dcend = g_dcbuffer;
    g_bitpos = 0;
    g_bitbuf = 0;
    g_eof = 0;
}

uint32 CUnImp::getbits(int nbits)
{
    if (nbits > g_bitpos)
    {
        while (g_bitpos < 25)
        {
            if (g_dcsrc >= g_dcend)
            {
                g_dcsrc = g_dcbuffer;
                if (!g_eof)
                {
                    g_dcend = g_dcbuffer + fread(g_dcbuffer,1,sizeof(g_dcbuffer),g_archfile);
                    g_eof = feof(g_archfile);
                }
            }
            g_bitbuf += (*g_dcsrc++) << g_bitpos;
            g_bitpos += 8;
        }
    }
    return g_bitbuf;
}

int32 CUnImp::unstore(uchar *output,int32 size)
{
    fseek(g_archfile,g_dcsrc - g_dcend - (g_bitpos >> 3),SEEK_CUR);
    return fread(output,1,size,g_archfile);
}

void CUnImp::make_codes(uint *huffcodes,uchar *hclengths,int ncodes)
{
    uint i,j,k,l,count[MAXCODELEN + 1],nextcode[MAXCODELEN + 1];
    memset(count,0,sizeof(count));
    for (i = 0;i < (uint)ncodes;i++)
    {
        count[hclengths[i]]++;
    }
    count[0] = 0;
    for (i = 1,k = 0;i <= MAXCODELEN;i++)
    {
        k = (k + count[i - 1]) << 1;
        nextcode[i] = k;
    }
    for (i = 0;i < (uint)ncodes;i++)
    {
        j = hclengths[i];
        if (j)
        {
            l = 0;
            k = nextcode[j]++;
            for (;j > 0;j--)
            {
                l <<= 1;
                l += k & 1;
                k >>= 1;
            }
            huffcodes[i] = l;
        }
    }
}

int CUnImp::make_decode_tables(int *ttable,int hdchain[][2],uint *huffcodes,uchar *hclengths,int ttbits,int ncodes)
{
    int hdnode,hdndest,i,ttsize;
    uint base,n,hcode;
    ttsize = 1 << ttbits;
    if (hdchain)
    {
        for (i = 0;i < ncodes;i++)
        {
            hdchain[i][0] = hdchain[i][1] = -MAX_NCODES;
        }
    }
    for (i = 0;i < ttsize;i++)
    {
        ttable[i] = -MAX_NCODES;
    }
    hdndest = 1;
    for (i = 0;i < ncodes;i++)
    {
        if (hclengths[i])
        {
            base = 1 << hclengths[i];
            n = huffcodes[i];
            if (base <= (uint)ttsize)
            {
                while (n < (uint)ttsize)
                {
                    ttable[n] = i;
                    n += base;
                }
            }
            else if (hdchain)
            {
                base = hclengths[i] - ttbits - 1;
                n &= ttsize - 1;
                hcode = huffcodes[i] >> ttbits;
                if (ttable[n] == -MAX_NCODES)
                {
                    ttable[n] = -hdndest;
                    hdnode = hdndest++;
                }
                else
                {
                    hdnode = -ttable[n];
                    while (hdchain[hdnode][hcode & 1] >= 0)
                    {
                        hdnode = hdchain[hdnode][hcode & 1];
                        hcode >>= 1;
                        base--;
                    }
                }
                while (base--)
                {
                    if (hdndest == ncodes)
                    {
                        return EIMP_DAMAGED;
                    }
                    hdchain[hdnode][hcode & 1] = hdndest;
                    hdnode = hdndest++;
                    hcode >>= 1;
                }
                hdchain[hdnode][hcode & 1] = -i;
            }
        }
    }
    return 0;
}

void CUnImp::load_fixed_codes(uchar *hclens,int32 exsize)
{
    memset(hclens,8,244);
    memset(hclens + 244,9,17);
    hclens[256] = 0;
    memset(hclens + 261,10,5);
    memset(hclens + 266,11,NLLCODES - 266);
    if (exsize <= 8192)
    {
        memset(hclens + NLLCODES,4,4);
        memset(hclens + NLLCODES + 4,5,24);
        memset(hclens + NLLCODES + 28,0,NDCODES - 28);
    }
    else
    {
        memset(hclens + NLLCODES,5,22);
        memset(hclens + NLLCODES + 22,6,NDCODES - 22);
    }
    memset(hclens + NLLCODES + NDCODES,4,ND2CODES);
    hclens[NLLCODES + NDCODES] = 3;
    hclens[NLLCODES + NDCODES + 1] = 3;
}

int CUnImp::load_hcodes(uchar *hclengths,int ncodes,int ntrees,int32 exsize)
{
    int i,j,dzrpt,zrpt,len,hcode,code;
    uchar hlclens[NLENCODES];
    uchar *hcldest,*hclend,*hclrpt;
    uint huffcodes[NLENCODES];
    int ttable[HCTTSIZE];
    int hdchain[NLENCODES][2];
    if (!ntrees)
    {
        load_fixed_codes(hclengths,exsize);
        return 0;
    }
    memset(hlclens,0,NLENCODES);
    for (i = 0,j = 1;i < NLENCODES;)
    {
        code = getbits(5) & 31;
        USEDBITS(5);
        if (code < 2)
        {
            i += j << code;
            j <<= 1;
        }
        else
        {
            if (code > MAXCODELEN + 1)
            {
                return EIMP_DAMAGED;
            }
            hlclens[i++] = code - 1;
            j = 1;
        }
    }
    make_codes(huffcodes,hlclens,NLENCODES);
    make_decode_tables(ttable,hdchain,huffcodes,hlclens,HCTTBITS,NLENCODES);
    hcldest = hclengths;
    hclend = hclengths + ncodes * ntrees;
    hclrpt = hclengths + ncodes;
    len = 0;
    dzrpt = 1;
    zrpt = 1;
    while (hcldest < hclend)
    {
        code = ttable[getbits(HCTTBITS) & (HCTTSIZE - 1)];
        if (code < 0)
        {
            if (code == -MAX_NCODES)
            {
                return EIMP_DAMAGED;
            }
            hcode = getbits(MAXCODELEN) >> HCTTBITS;
            code = -code;
            do
            {
                code = hdchain[code][hcode & 1];
                hcode >>= 1;
            }while (code > 0);
            if (code == -MAX_NCODES)
            {
                return EIMP_DAMAGED;
            }
            code = -code;
        }
        USEDBITS(hlclens[code]);
        if (code >= DZRUN)
        {
            code = dzrpt << (code - DZRUN);
            while (code-- && hcldest < hclend)
            {
                if (hcldest >= hclrpt && hcldest[-ncodes])
                {
                    len = hcldest[-ncodes];
                }
                *hcldest++ = len;
            }
            dzrpt <<= 1;
            zrpt = 1;
        }
        else if (code >= ZRUN)
        {
            code = zrpt << (code - ZRUN);
            while (code-- && hcldest < hclend)
            {
                *hcldest++ = 0;
            }
            zrpt <<= 1;
            dzrpt = 1;
        }
        else
        {
            if (hcldest >= hclrpt && hcldest[-ncodes])
            {
                len = code + hcldest[-ncodes];
            }
            else
            {
                len += code;
            }
            if (len > MAXCODELEN)
            {
                len -= MAXCODELEN + 1;
            }
            *hcldest++ = len;
            dzrpt = 1;
            zrpt = 1;
        }
    }
    for (hcldest = hclengths;hcldest < hclend;hcldest++)
    {
        if (*hcldest > MAXCODELEN)
        {
            return EIMP_DAMAGED;
        }
    }
    return 0;
}

void CUnImp::reverse_mm(uchar *block,int32 bsize)
{
    int m,d;
    uchar *src,*end;
    for (m = 0;m < g_mmptr;m++)
    {
        d = -(int)(g_mmlist[m] >> 24);
        if (!d)
        {
            continue;
        }
        src = block + (g_mmlist[m] & 0xFFFFFF);
        end = block + bsize;
        if (m < g_mmptr - 1)
        {
            end = block + (g_mmlist[m + 1] & 0xFFFFFF);
        }
        while (src < end)
        {
            *src += src[d];
            src++;
        }
    }
}

void CUnImp::redo_mm(uint32 overlap)
{
    int mmptr,d;
    uchar *start,*src,*end;
    start = g_blockbuffer + g_blockstart - overlap;
    if (start < g_blockbuffer)
    {
        start = g_blockbuffer;
    }
    for (mmptr = g_mmptr - 1;mmptr >= 0;mmptr--)
    {
        d = -(int)(g_mmlist[mmptr] >> 24);
        if (!d)
        {
            continue;
        }
        end = g_blockbuffer + (g_mmlist[mmptr] & 0xFFFFFF);
        if (end < start)
        {
            end = start;
        }
        src = g_blockbuffer + g_blockstart;
        if (mmptr < g_mmptr - 1)
        {
            src = g_blockbuffer + (g_mmlist[mmptr + 1] & 0xFFFFFF);
        }
        for (src--;src >= end;src--)
        {
            *src -= src[d];
        }
        if (end <= start)
        {
            break;
        }
    }
}

int32 CUnImp::expand_lz(uchar *output,int32 start,int32 exsize,int mm)
{
    uint huffcodes[MAX_NCODES];
    int llcode_table[LLTTSIZE],distcode_table[3][DTTSIZE];
    int llcode_chain[NLLCODES][2],distcode_chain[3][NDCODES][2];
    uint32 dist_base[NDCODES - 6],dist_mask[NDCODES - 6];
    uchar dist_extra[NDCODES - 6],len_base[NLLCODES - 268],len_mask[NLLCODES - 268],len_extra[NLLCODES - 268];
    uchar *dest,*output_end;
    int i,j,treenum,code,num_dist_trees,dist_split,numcodes,numtrees;
    uint len;
    uint32 hcode,dist,last_dist,last_dist2;
    uchar *code_lengths,*codelen_src;
    if (mm)
    {
        g_mmptr = 0;
    }
    num_dist_trees = getbits(2) & 3;
    USEDBITS(2);
    if (!num_dist_trees)
    {
        return EIMP_DAMAGED;
    }
    dist_split = num_dist_trees > 2;
    numcodes = NLLCODES + NDCODES + ND2CODES;
    if (dist_split)
    {
        numcodes += ND3CODES;
    }
    dest = output + start;
    output_end = dest + exsize;
    numtrees = getbits(6);
    USEDBITS(6);
    numtrees &= 63;
    if (numtrees)
    {
        code_lengths = (uchar*)malloc(numcodes * numtrees);
    }
    else
    {
        code_lengths = (uchar*)malloc(MIN_LZ_CODES);
    }
    if (!code_lengths)
    {
        return EIMP_OUTOFMEMORY;
    }
    i = load_hcodes(code_lengths,numcodes,numtrees,start + exsize);
    if (i < 0)
    {
        free(code_lengths);
        return i;
    }
    for (i = 2,len = 11,code = 0;code < NLLCODES - 268;i <<= 1)
    {
        for (j = code + 4;code < j;len += i)
        {
            len_base[code++] = len;
        }
    }
    for (i = 1,code = 0;code < NLLCODES - 268;i++)
    {
        for (j = code + 4;code < j;code++)
        {
            len_extra[code] = i;
        }
    }
    for (code = 0,len = 1;code < NLLCODES - 268;len = (len << 1) + 1)
    {
        for (j = code + 4;code < j;code++)
        {
            len_mask[code] = len;
        }
    }
    for (hcode = 2,dist = 5,code = 0;code < NDCODES - 6;hcode <<= 1)
    {
        dist_base[code] = dist;
        dist += hcode;
        dist_base[code + 1] = dist;
        dist += hcode;
        code += 2;
    }
    for (i = 1,code = 0;code < NDCODES - 6;i++,code += 2)
    {
        dist_extra[code] = dist_extra[code + 1] = i;
    }
    for (code = 0,dist = 1;code < NDCODES - 6;dist = (dist << 1) + 1,code += 2)
    {
        dist_mask[code] = dist_mask[code + 1] = dist;
    }
    codelen_src = code_lengths - numcodes;
    last_dist = 0;
    last_dist2 = 0;
    code = 256;
    numtrees += !numtrees;
    while (dest < output_end)
    {
        if (code == 256)
        {
            if (!numtrees)
            {
                break;
            }
            numtrees--;
            codelen_src += numcodes;
            make_codes(huffcodes,codelen_src,NLLCODES);
            make_decode_tables(llcode_table,llcode_chain,huffcodes,codelen_src,LLTTBITS,NLLCODES);
            codelen_src += NLLCODES;
            for (i = 0;i < num_dist_trees;i++)
            {
                make_codes(huffcodes,codelen_src,g_ncodes[i]);
                make_decode_tables(distcode_table[i],distcode_chain[i],huffcodes,codelen_src,DTTBITS,g_ncodes[i]);
                codelen_src += g_ncodes[i];
            }
            codelen_src -= numcodes;
            code = 0;
        }
        code = llcode_table[getbits(LLTTBITS) & (LLTTSIZE - 1)];
        if (code < 0)
        {
            if (code == -MAX_NCODES)
            {
                break;
            }
            hcode = getbits(MAXCODELEN) >> LLTTBITS;
            code = -code;
            do
            {
                code = llcode_chain[code][hcode & 1];
                hcode >>= 1;
            }while (code > 0);
            if (code == -MAX_NCODES)
            {
                break;
            }
            code = -code;
        }
        USEDBITS(codelen_src[code]);
        if (code < 256)
        {
            *dest++ = code;
        }
        else if (code > 256)
        {
            if (code < 266)
            {
                len = code - 255;
            }
            else if (code < LONGMATCHCODE)
            {
                len = getbits(5);
                USEDBITS(len_extra[code - 266]);
                len = (len & len_mask[code - 266]) + len_base[code - 266];
            }
            else if (code == LONGMATCHCODE)
            {
                len = (getbits(LONGMATCHBITS) & LONGMATCHMASK) + 259;
                USEDBITS(LONGMATCHBITS);
            }
            else
            {
                len = getbits(3) & 7;
                USEDBITS(3);
                g_mmlist[g_mmptr++] = (dest - output) | (len << 24);
                continue;
            }
            treenum = (len == 2) + ((dist_split & (len == 3)) << 1);
            code = distcode_table[treenum][getbits(DTTBITS) & (DTTSIZE - 1)];
            if (code < 0)
            {
                if (code == -MAX_NCODES)
                {
                    break;
                }
                hcode = getbits(MAXCODELEN) >> DTTBITS;
                code = -code;
                do
                {
                    code = distcode_chain[treenum][code][hcode & 1];
                    hcode >>= 1;
                }while (code > 0);
                if (code == -MAX_NCODES)
                {
                    break;
                }
                code = -code;
            }
            USEDBITS(codelen_src[code + g_ncoffset[treenum]]);
            if (!code)
            {
                dist = last_dist;
            }
            else if (code == 1)
            {
                dist = last_dist2;
                last_dist2 = last_dist;
                last_dist = dist;
            }
            else
            {
                if (code < 6)
                {
                    dist = code - 1;
                }
                else
                {
                    dist = getbits(18);
                    USEDBITS(dist_extra[code - 6]);
                    dist = (dist & dist_mask[code - 6]) + dist_base[code - 6];
                    last_dist2 = last_dist;
                    last_dist = dist;
                }
            }
            if (dist <= (uint)(dest - output))
            {
                while (len-- && dest < output_end)
                {
                    *dest = dest[-(int)dist];
                    dest++;
                }
            }
        }
    }
    free(code_lengths);
    if (mm)
    {
        reverse_mm(output,start + exsize);
    }
    return dest - output - start;
}

int CUnImp::e8ut_crc_wr16(FILE *outfile,uchar *block,int32 size,uint32 e8offset)
{
    uchar buffer[16384];
    int32 w;
    uint u;
    uchar *src,*end,*dest;
    uchar c;
    src = block;
    end = block + size;
    dest = buffer;
    while (src < end)
    {
        if (end - src > sizeof(buffer) - 3)
        {
            end = src + sizeof(buffer) - 3;
        }
        while (src < end)
        {
            c = *dest++ = *src++;
            g_crc = g_crctbl[(uchar)g_crc ^ c] ^ (g_crc >> 8);
            if (c == 0xE8)
            {
                u = get_uint16(src) - (uint16)(e8offset + (src - block));
                *dest = (uchar)u;
                dest[1] = (uchar)(u >> 8);
                src += 2;
                g_crc = g_crctbl[(uchar)g_crc ^ *dest++] ^ (g_crc >> 8);
                g_crc = g_crctbl[(uchar)g_crc ^ *dest++] ^ (g_crc >> 8);
            }
        }
        w = fwrite(buffer,1,dest - buffer,outfile);
        if (w < dest - buffer)
        {
            return EIMP_WRITE;
        }
        dest = buffer;
        end = block + size;
    }
    return src - end;
}

int CUnImp::e8ut_crc_wr32(FILE *outfile,uchar *block,int32 size,uint32 e8offset,uint32 srcsize)
{
    uchar buffer[16384];
    int32 i,j,w;
    uchar *src,*end,*end2,*dest;
    uchar c;
    src = block;
    end = block + size;
    dest = buffer;
    if (end - src > sizeof(buffer) - 5)
    {
        end = src + sizeof(buffer) - 5;
    }
    while (src < end)
    {
        if (end - src > sizeof(buffer) - 5)
        {
            end = src + sizeof(buffer) - 5;
        }
        while (src < end)
        {
            c = *dest++ = *src++;
            g_crc = g_crctbl[(uchar)g_crc ^ c] ^ (g_crc >> 8);
            if (c == 0xE8)
            {
                i = get_uint32(src);
                j = e8offset + (src - block);
                if (i < 0 && i >= -j)
                {
                    i = srcsize - i - j - 1;
                }
                else if ((uint32)i < (uint32)srcsize)
                {
                    i -= j;
                }
                src += 4;
                for (end2 = dest + 4;dest < end2;dest++,i >>= 8)
                {
                    *dest = (uchar)i;
                    g_crc = g_crctbl[(uchar)g_crc ^ *dest] ^ (g_crc >> 8);
                }
            }
        }
        w = fwrite(buffer,1,dest - buffer,outfile);
        if (w < dest - buffer)
        {
            return EIMP_WRITE;
        }
        dest = buffer;
        end = block + size;
    }
    return src - end;
}

int CUnImp::expand_bwt(uchar *output,int32 exsize)
{
    int32 count[257];
    uchar hclengths[BWTABLES][NCODES];
    uint huffcodes[NCODES];
    int ttable[BWTABLES][BWTTSIZE];
    int hdchain[BWTABLES][NCODES][2];
    uchar mtf[256],treemtf[BWTABLES];
    uint32 *links;
    uchar *dest,*output_end;
    int i,treenum,code,codes,numtrees;
    int32 rptbase,rpt,link,start;
    uint hcode;
    uchar rc;
    rc = getbits(8) & 0xFF;
    USEDBITS(8);
    numtrees = getbits(3) & 7;
    USEDBITS(3);
    if (numtrees < 1)
    {
        return EIMP_DAMAGED;
    }
    for (treenum = 0;treenum < numtrees;treenum++)
    {
        i = load_hcodes(hclengths[treenum],NCODES,1,0);
        if (i < 0)
        {
            return i;
        }
        make_codes(huffcodes,hclengths[treenum],NCODES);
        make_decode_tables(ttable[treenum],hdchain[treenum],huffcodes,hclengths[treenum],BWTTBITS,NCODES);
    }
    g_bitbuf >>= g_bitpos & 7;
    g_bitpos &= ~7;
    for (i = 0;i < 256;i++)
    {
        mtf[i] = i;
    }
    for (i = 0;i < BWTABLES;i++)
    {
        treemtf[i] = i;
    }
    memset(count,0,sizeof(count));
    rptbase = 1;
    codes = WINDOW;
    dest = output;
    output_end = output + exsize + 1;
    start = 0;
    while (dest < output_end)
    {
        if (codes == WINDOW)
        {
            hcode = getbits(numtrees);
            code = 0;
            while (hcode & 1)
            {
                hcode >>= 1;
                code++;
            }
            if (code >= numtrees)
            {
                return EIMP_DAMAGED;
            }
            USEDBITS(code + 1);
            treenum = treemtf[code];
            if (code)
            {
                for (;code;code--)
                {
                    treemtf[code] = treemtf[code - 1];
                }
                treemtf[0] = treenum;
            }
            codes = 0;
            continue;
        }
        codes++;
        hcode = getbits(BWTTBITS) & (BWTTSIZE - 1);
        code = ttable[treenum][hcode];
        if (code < 0)
        {
            if (code == -MAX_NCODES)
            {
                return EIMP_DAMAGED;
            }
            hcode = getbits(MAXCODELEN) >> BWTTBITS;
            code = -code;
            do
            {
                code = hdchain[treenum][code][hcode & 1];
                hcode >>= 1;
            }while (code > 0);
            if (code == -MAX_NCODES)
            {
                return EIMP_DAMAGED;
            }
            code = -code;
        }
        USEDBITS(hclengths[treenum][code]);
        if (code < 2)
        {
            rpt = rptbase << code;
            if (dest + rpt > output_end)
            {
                rpt = output_end - dest;
            }
            i = mtf[0];
            count[i + 1] += rpt;
            while (rpt--)
            {
                *dest++ = i;
            }
            rptbase <<= 1;
        }
        else if (code < 257)
        {
            i = mtf[--code];
            *dest++ = i;
            for (;code;code--)
            {
                mtf[code] = mtf[code - 1];
            }
            mtf[0] = i;
            count[i + 1]++;
            rptbase = 1;
        }
        else
        {
            start = dest - output;
            dest++;
            rptbase = 1;
        }
    }
    count[0] = 1;
    for (i = 1;i < 256;i++)
    {
        count[i] += count[i - 1];
    }
    links = (uint*)malloc((exsize + 1) * sizeof(int32));
    if (!links)
    {
        return EIMP_OUTOFMEMORY;
    }
    for (link = 0;link <= exsize;link++)
    {
        if (link != start)
        {
            links[link] = (output[link] << 24) | count[output[link]]++;
        }
    }
    links[start] = 0;
    for (link = 0,dest = output_end - 2;dest >= output;dest--)
    {
        link = links[link & 0xFFFFFF];
        *dest = (link >> 24) + rc;
    }
    free(links);
    return 0;
}

int CUnImp::e8ut_crc_write(FILE *outfile,uchar *block,int32 size,uint32 e8offset,uint32 srcsize,int last)
{
    int32 i,w;
    uchar *src,*src2,*end;
    if (g_e8extra)
    {
        i = sizeof(g_e8buf) - g_e8extra;
        memcpy(g_e8buf + g_e8extra,block,i > size ? size : i);
        if (i > size)
        {
            block = g_e8buf;
            size += g_e8extra;
            e8offset -= g_e8extra;
        }
        else
        {
            if (g_e8t == 2)
            {
                i = e8ut_crc_wr16(outfile,g_e8buf,g_e8extra,e8offset - g_e8extra);
            }
            else
            {
                i = e8ut_crc_wr32(outfile,g_e8buf,g_e8extra,e8offset - g_e8extra,srcsize);
            }
            if (i < 0)
            {
                return i;
            }
            block += i;
            e8offset += i;
            size -= i;
        }
    }
    if (g_e8t == 2)
    {
        i = e8ut_crc_wr16(outfile,block,size - g_e8t,e8offset);
    }
    else
    {
        i = e8ut_crc_wr32(outfile,block,size - g_e8t,e8offset,srcsize);
    }
    if (i < 0)
    {
        return i;
    }
    end = block + size;
    src = end - g_e8t + i;
    src2 = src;
    while (src2 < end && (*src2 != 0xE8 || last))
    {
        g_crc = g_crctbl[(uchar)g_crc ^ *src2++] ^ (g_crc >> 8);
    }
    if (src2 > src)
    {
        w = fwrite(src,1,src2 - src,outfile);
        if (w < src2 - src)
        {
            return EIMP_WRITE;
        }
    }
    g_e8extra = end - src2;
    if (g_e8extra)
    {
        memmove(g_e8buf,src2,g_e8extra);
    }
    return 0;
}

int CUnImp::decomp(FILE *outfile,uint32 fpos,uint32 unitpos,uint32 togo,uchar flags)
{
    uchar lhbuf[LOCAL_HEADER_SIZE];
    uint16 namelen;
    int i,lhptr,nameptr;
    uint method,level,ovlevel;
    int32 nread,written;
    uint32 overlap,e8offset,srcsize,block_csize;
    uchar *blocksrc;
    if (!togo)
    {
        g_crc = 0;
        return 0;
    }
    g_crc = 0xFFFFFFFF;
    g_e8extra = 0;
    srcsize = togo;
    e8offset = 0;
    g_exlast = 0;
    lhptr = 0;
    nameptr = 0;
    g_e8t = 0;
    if (flags & DFLAG_E8SHORT)
    {
        g_e8t = 2;
    }
    else if (flags & DFLAG_E8LONG)
    {
        g_e8t = 4;
    }
    fpos += 6 + g_exesize;
    if (fpos != g_prevfpos || unitpos < g_unitoffset)
    {
        g_blockstart = 0;
        g_blocksize = 0;
        g_exlast = 0;
        g_exprev = 0;
        g_unitoffset = 0;
        g_exerror = 0;
        g_mmptr = 0;
        g_prevfpos = fpos;
        fseek(g_archfile,fpos,SEEK_SET);
        if (g_blockbuffer)
        {
            free(g_blockbuffer);
            g_blockbuffer = NULL;
        }
    }
    while (togo)
    {
        if (g_exerror)
        {
            return EIMP_DAMAGED;
        }
        if (unitpos < g_unitoffset + g_blocksize)
        {
            blocksrc = g_blockbuffer + g_blockstart + unitpos - g_unitoffset;
            nread = g_blocksize - (unitpos - g_unitoffset);
            if (lhptr < LOCAL_HEADER_SIZE)
            {
                i = LOCAL_HEADER_SIZE - lhptr;
                if (i > nread)
                {
                    i = nread;
                }
                memcpy(lhbuf + lhptr,blocksrc,i);
                lhptr += i;
                unitpos += i;
                nread -= i;
                if (lhptr < LOCAL_HEADER_SIZE)
                {
                    continue;
                }
            }
            namelen = get_uint16(lhbuf + 4);
            if (nameptr < namelen)
            {
                i = namelen - nameptr;
                if (i > nread)
                {
                    i = nread;
                }
                nameptr += i;
                unitpos += i;
                continue;
            }
            if (nread > (int)togo)
            {
                nread = togo;
            }
            if (!g_e8t)
            {
                g_crc = block_crc(blocksrc,nread,g_crc);
                written = fwrite(blocksrc,1,nread,outfile);
                if (written < nread)
                {
                    return EIMP_WRITE;
                }
            }
            else
            {
                i = e8ut_crc_write(outfile,blocksrc,nread,e8offset,srcsize,nread == (int)togo);
                if (i < 0)
                {
                    return i;
                }
                written = nread;
                e8offset += nread;
            }
            togo -= nread;
            unitpos += nread;
        }
        if (togo)
        {
            g_exerror = 1;
            if (g_exlast)
            {
                return 0;
            }
            g_blockstart += g_blocksize;
            g_unitoffset += g_blocksize;
            fpos = ftell(g_archfile);
            init_getbits();
            method = getbits(4) & 15;
            USEDBITS(4);
            if (method > MAXMETHOD)
            {
                return EIMP_DAMAGED;
            }
            g_exlast = getbits(1) & 1;
            USEDBITS(1);
            g_blocksize = getbits(20) & ((1L << 20) - 1);
            USEDBITS(20);
            block_csize = getbits(20) & ((1L << 20) - 1);
            USEDBITS(20);
            if (!g_blockbuffer)
            {
                g_exmemory = g_blocksize;
                g_blockbuffer = (uchar*)malloc(g_exmemory + 512);
                if (!g_blockbuffer)
                {
                    return EIMP_OUTOFMEMORY;
                }
            }
            if (method == METHOD_LZ77 || method == METHOD_LZ77_MM)
            {
                if (g_exprev)
                {
                    level = getbits(2) & 3;
                    USEDBITS(2);
                    if (level > 2)
                    {
                        return EIMP_DAMAGED;
                    }
                    ovlevel = getbits(3) & 7;
                    USEDBITS(3);
                    overlap = g_overlaps[level][ovlevel];
                    redo_mm(overlap);
                    if (g_blockstart > overlap)
                    {
                        memmove(g_blockbuffer,g_blockbuffer + g_blockstart - overlap,overlap);
                        g_blockstart = overlap;
                    }
                    if (g_blocksize + g_blockstart > g_exmemory)
                    {
                        uchar *t;
                        g_exmemory = g_blocksize + g_blockstart;
                        t = (uchar*)realloc(g_blockbuffer,g_exmemory + 512);
                        if (!t)
                        {
                            free(g_blockbuffer);
                            g_blockbuffer = 0;
                            return EIMP_OUTOFMEMORY;
                        }
                        g_blockbuffer = t;
                    }
                }
                else
                {
                    USEDBITS(5);
                }
                written = expand_lz(g_blockbuffer,g_blockstart,g_blocksize,1);
                if (written < 0)
                {
                    return written;
                }
                if ((uint32)written < g_blocksize)
                {
                    g_blocksize = (uint32)written;
                    g_exlast = 1;
                }
            }
            else
            {
                if (g_blocksize > g_exmemory)
                {
                    uchar *t;
                    g_exmemory = g_blocksize;
                    t = (uchar*)realloc(g_blockbuffer,g_exmemory + 512);
                    if (!t)
                    {
                        free(g_blockbuffer);
                        return EIMP_OUTOFMEMORY;
                    }
                    g_blockbuffer = t;
                }
                if (!g_exlast)
                {
                    overlap = MAXOVERLAP - g_blocksize;
                    if ((int32)overlap < 0)
                    {
                        overlap = 0;
                    }
                    else if (overlap > g_exmemory - g_blocksize)
                    {
                        overlap = g_exmemory - g_blocksize;
                    }
                    redo_mm(overlap);
                    if (g_blockstart > overlap)
                    {
                        memmove(g_blockbuffer,g_blockbuffer + g_blockstart - overlap,overlap);
                        g_blockstart = overlap;
                    }
                }
                else
                {
                    g_blockstart = 0;
                }
                g_mmptr = 0;
                if (method == METHOD_BWT)
                {
                    i = expand_bwt(g_blockbuffer + g_blockstart,g_blocksize);
                    if (i < 0)
                    {
                        return i;
                    }
                }
                else
                {
                    i = unstore(g_blockbuffer + g_blockstart,g_blocksize);
                    if (i < 0)
                    {
                        return i;
                    }
                    if ((uint32)i < g_blocksize)
                    {
                        g_blocksize = (uint32)i;
                        g_exlast = 1;
                    }
                }
            }
            fseek(g_archfile,fpos + block_csize,SEEK_SET);
            g_exerror = 0;
            g_exprev = 1;
        }
    }
    g_crc = ~g_crc;
    return 0;
}

int CUnImp::find_start(void)
{
    char *buffer,*src,*end;
    int32 nread;
    fseek(g_archfile,0,SEEK_SET);
    buffer = (char*)malloc(FIND_START_BUFFER);
    if (!buffer)
    {
        return EIMP_OUTOFMEMORY;
    }
    nread = fread(buffer,1,FIND_START_BUFFER,g_archfile);
    if (nread < IMP_HEADER_SIZE)
    {
        return EIMP_NOT_IMP;
    }
    src = buffer;
    end = buffer + nread - IMP_HEADER_SIZE + 1;
    g_exesize = 0;
    do
    {
        src = (char*)memchr(src,IMP_SIG_0,end - src);
        if (!src)
        {
            return EIMP_NOT_IMP;
        }
        if (src[1] == IMP_SIG_1 && src[2] == IMP_SIG_2 && src[3] == IMP_SIG_3)
        {
            g_exesize = src - buffer;
            free(buffer);
            return 0;
        }
        src++;
    }while (src < end);
    return EIMP_NOT_IMP;
}

int CUnImp::extract_dir(void)
{
    int i,method;
    uint32 fpos,csize,exsize;
    char sig[8]; 
    fpos = ftell(g_archfile);
    fseek(g_archfile,g_dirfpos,SEEK_SET);
    g_dirbufsrc = g_dirbuf;
    g_dirbufend = g_dirbuf;
    if (!fread(sig,6,1,g_archfile))
    {
        return EIMP_EOF;
    }
    if (memcmp(sig,&g_dirsig,6) != 0)
    {
        return EIMP_BAD_DIR;
    }
    init_getbits();
    method = getbits(5) & 15;
    USEDBITS(5);
    if (method > METHOD_LZ77)
    {
        return EIMP_BAD_DIR;
    }
    exsize = getbits(20) & ((1L << 20) - 1);
    USEDBITS(20);
    if (exsize > IMPDIRBLOCK)
    {
        return EIMP_BAD_DIR;
    }
    csize = getbits(20) & ((1L << 20) - 1);
    USEDBITS(20);
    if (method == METHOD_LZ77)
    {
        USEDBITS(5);
        i = expand_lz((uchar*)g_dirbuf,0,exsize,0);
    }
    else
    {
        i = unstore((uchar*)g_dirbuf,exsize);
    }
    g_dirfpos += 6 + csize;
    fseek(g_archfile,fpos,SEEK_SET);
    if (i < 0)
    {
        return i;
    }
    g_dirbufend += i;
    return 0;
}

int CUnImp::get_dir_entry(IMP_DIR_ENTRY *pdirent,char *name,char *comment)
{
    int i;
    uint32 crc;
    if (g_dirbufsrc >= g_dirbufend)
    {
        int err = extract_dir();
        if (err < 0)
        {
            return err;
        }
    }
    if (g_dirbufend - g_dirbufsrc < IMP_DIR_ENTRY_SIZE)
    {
        return EIMP_BAD_DIR;
    }
    pdirent->ver = get_uint16(g_dirbufsrc);
    pdirent->disk = get_uint16(g_dirbufsrc + 2);
    pdirent->fpos = get_uint32(g_dirbufsrc + 4);
    pdirent->attrib = get_uint16(g_dirbufsrc + 8);
    pdirent->commlen = g_dirbufsrc[10];
    pdirent->flags = g_dirbufsrc[11];
    pdirent->unitpos = get_uint32(g_dirbufsrc + 12);
    pdirent->esize = get_uint32(g_dirbufsrc + 16);
    pdirent->crc = get_uint32(g_dirbufsrc + 20);
    pdirent->extra = get_uint16(g_dirbufsrc + 24);
    pdirent->namelen = get_uint16(g_dirbufsrc + 26);
    pdirent->ctime = get_uint16(g_dirbufsrc + 28);
    pdirent->cdate = get_uint16(g_dirbufsrc + 30);
    pdirent->mtime = get_uint16(g_dirbufsrc + 32);
    pdirent->mdate = get_uint16(g_dirbufsrc + 34);
    pdirent->entcrc = get_uint16(g_dirbufsrc + 36);
    if ((pdirent->ver & 0xFFF) > MAX_VERSION)
    {
        return EIMP_OLDVERSION;
    }
    i = IMP_DIR_ENTRY_SIZE + pdirent->namelen + pdirent->commlen + pdirent->extra;
    if (g_dirbufend - g_dirbufsrc < i)
    {
        return EIMP_BAD_DIR;
    }
    g_dirbufsrc[36] = 0;
    g_dirbufsrc[37] = 0;
    crc = block_crc(g_dirbufsrc,i,0xFFFFFFFF);
    if ((~crc & 0xFFFF) != pdirent->entcrc)
    {
        return EIMP_BAD_DIR;
    }
    memcpy(name,g_dirbufsrc + IMP_DIR_ENTRY_SIZE,pdirent->namelen);
    name[pdirent->namelen] = 0;
    convert_path(name);
    memcpy(comment,g_dirbufsrc + IMP_DIR_ENTRY_SIZE + pdirent->namelen,pdirent->commlen);
    comment[pdirent->commlen] = 0;
    g_dirbufsrc += i;
    return 0;
}

int CUnImp::get_impcomment(uint32 cmoffset,char *impcomment)
{
    char chbuf[4];
    int len;
    fseek(g_archfile,g_exesize + cmoffset,SEEK_SET);
    if (!fread(chbuf,4,1,g_archfile))
    {
        return EIMP_EOF;
    }
    len = get_uint16(chbuf);
    if (len > sizeof(impcomment) - 1)
    {
        len = sizeof(impcomment) - 1;
    }
    if (!fread(impcomment,1,len,g_archfile))
    {
        return EIMP_EOF;
    }
    if ((uint16)~block_crc(impcomment,len,0xFFFFFFFF) != get_uint16(chbuf + 2))
    {
        return EIMP_DAMAGED;
    }
    *(impcomment + len) = '\0';
    return 0;
}

int CUnImp::make_list(IMP_ARCHIVE_INFO *iai)
{
    char name[MAX_PATH * 2],comment[MAXFILECOMMENT + 1],impcomment[4096];
    uchar ihbuf[IMP_HEADER_SIZE];
    IMP_HEADER ih;
    IMP_DIR_ENTRY dirent;
    IMP_FILE_HEADER ifh;
    int err;
    uint32 ent;
    uint32 archsize;
    fseek(g_archfile,0,SEEK_END);
    archsize = ftell(g_archfile);
    if (archsize < sizeof(IMP_HEADER))
    {
        return EIMP_NOT_IMP;
    }
    fseek(g_archfile,0,SEEK_SET);
    if (!fread(ihbuf,4,1,g_archfile))
    {
        return EIMP_EOF;
    }
    fseek(g_archfile,0,SEEK_SET);
    ih.sig = get_uint32(ihbuf);
    if (ih.sig != IMP_SIG)
    {
        err = find_start();
        if (err < 0)
        {
            return err;
        }
        if (g_exesize + IMP_HEADER_SIZE > archsize)
        {
            return EIMP_EOF;
        }
        fseek(g_archfile,g_exesize,SEEK_SET);
    }
    if (!fread(ihbuf,IMP_HEADER_SIZE,1,g_archfile))
    {
        return EIMP_EOF;
    }
    ih.sig = get_uint32(ihbuf);
    ih.diroffset = get_uint32(ihbuf + 4);
    ih.direntries = get_uint32(ihbuf + 8);
    ih.ecoffset = get_uint32(ihbuf + 12);
    ih.cmoffset = get_uint32(ihbuf + 16);
    ih.length = get_uint32(ihbuf + 20);
    ih.csize = get_uint32(ihbuf + 24);
    ih.mvsig = get_uint16(ihbuf + 28);
    ih.disk = get_uint16(ihbuf + 30);
    ih.dirdisk = get_uint16(ihbuf + 32);
    ih.cmdisk = get_uint16(ihbuf + 34);
    ih.ecdisk = get_uint16(ihbuf + 36);
    ih.flags = get_uint16(ihbuf + 38);
    ih.crc = get_uint16(ihbuf + 40);
    ihbuf[40] = 0;
    ihbuf[41] = 0;
    if ((~block_crc(ihbuf,IMP_HEADER_SIZE,0xFFFFFFFF) & 0xFFFF) != ih.crc)
    {
        return EIMP_DAMAGED;
    }
    if (ih.flags & HFLAG_MULTIVOL)
    {
        return EIMP_NOMULTIVOL;
    }
    if (ih.flags & HFLAG_BLOWFISH)
    {
        return EIMP_NOCIPHER;
    }
    if (ih.cmoffset)
    {
        if (g_exesize + ih.cmoffset > archsize)
        {
            return EIMP_EOF;
        }
        err = get_impcomment(ih.cmoffset,impcomment);
        if (err < 0)
        {
            return err;
        }
    }
    g_blockbuffer = 0;
    g_prevfpos = 0xFFFFFFFF;
    g_dirbufsrc = g_dirbufend = g_dirbuf;
    g_dirfpos = ih.diroffset + g_exesize;
    if (g_dirfpos > archsize)
    {
        return EIMP_EOF;
    }
    fseek(g_archfile,g_dirfpos,SEEK_SET);
    iai->file_count = ih.direntries;
    iai->ifh = (IMP_FILE_HEADER*)calloc(ih.direntries,sizeof(IMP_FILE_HEADER));
    for (ent = 0;ent < ih.direntries;ent++)
    {
        err = get_dir_entry(&dirent,name,comment);
        if (err < 0)
        {
            return err;
        }
        ifh.dirent = dirent;
        strcpy(ifh.filename,name);
        strcpy(ifh.comment,comment);
        ifh.orgsize = dirent.esize;
        ifh.compsize = 0;
        lstrcpy(ifh.method,"DUMMY");
        iai->ifh[ent] = ifh;
    }
    return 0;
}

void CUnImp::set_last_error(int error)
{
    g_last_error = error;
}

bool CUnImp::mkdir2(const char *dirname)
{
    int i;
    char temp[MAX_PATH * 2];
    char _dirname[MAX_PATH * 2];
    lstrcpy(_dirname,dirname);
    if (_dirname[lstrlen(_dirname) - 1] != '\\')
    {
        lstrcat(_dirname,"\\");
    }
    for (i = 0;i < lstrlen(_dirname);i++)
    {
        temp[i] = _dirname[i];
        if (isJMS(_dirname,i) == 0 && (_dirname[i] == '\\' || _dirname[i] == '/'))
        {
            temp[i] = '\\';
            temp[i + 1] = '\0';
            if (mkdir(temp) == -1 && errno == ENOENT)
            {
                return false;
            }
        }
    }
    return true;
}

#ifndef SFX

int CUnImp::convert_imperror(int error)
{
    int code = 0;
    switch (error)
    {
    case EIMP_OLDVERSION:
        code = ERROR_NOT_SUPPORT;
        break;
    case EIMP_NOT_IMP:
        code = ERROR_NOT_ARC_FILE;
        break;
    case EIMP_DAMAGED:
        code = ERROR_BROKEN_DATA;
        break;
    case EIMP_BAD_DIR:
        code = ERROR_HEADER_BROKEN;
        break;
    case EIMP_EOF:
        code = ERROR_UNEXPECTED_EOF;
        break;
    case EIMP_WRITE:
        code = ERROR_CANNOT_WRITE;
        break;
    case EIMP_NOCIPHER:
        code = ERROR_NOT_SUPPORT;
        break;
    case EIMP_NOMULTIVOL:
        code = ERROR_NOT_SUPPORT;
        break;
    case EIMP_BAD_CRC:
        code = ERROR_FILE_CRC;
        break;
    case EIMP_OUTOFMEMORY:
        code = ERROR_ENOUGH_MEMORY;
        break;
    case EIMP_DIRECTORY:
        code = ERROR_MAKEDIRECTORY;
        break;
    case EIMP_FIND_FILE:
        code = ERROR_NOT_FIND_ARC_FILE;
        break;
    case EIMP_OPEN:
        code = ERROR_ARC_FILE_OPEN;
        break;
    case EIMP_MAKE_FILE:
        code = ERROR_FILE_OPEN;
        break;
    case EIMP_TOO_LONG_NAME:
        code = ERROR_LONG_FILE_NAME;
        break;
    default:
        code = ERROR_UNKNOWN;
        break;
    }
    return code;
}

#endif

#pragma pack(1)

typedef struct
{
    char magic[3];
    CUnImp *imp;
    IMP_ARCHIVE_INFO *iai;
    uint32 now;
}ImpArchive;

#pragma pack()

LPVOID CALLBACK OpenArchive(const char *arcname,LPVOID userdata)
{
    CUnImp *imp = new CUnImp();
    ImpArchive *ia;
    IMP_ARCHIVE_INFO *iai = (IMP_ARCHIVE_INFO*)malloc(sizeof(IMP_ARCHIVE_INFO));
    if (imp->open_archive(arcname,iai) == false)
    {
        free(iai);
        return NULL;
    }
    ia = (ImpArchive*)malloc(sizeof(ImpArchive));
    ia->magic[0] = 'I';
    ia->magic[1] = 'M';
    ia->magic[2] = 'P';
    ia->imp = imp;
    ia->iai = iai;
    ia->now = 0;
    return (LPVOID)ia;
}

BOOL CALLBACK CloseArchive(LPVOID ah,LPVOID userdata)
{
    ImpArchive *ia;
    if (ah == NULL)
    {
        return FALSE;
    }
    ia = (ImpArchive*)ah;
    if (ia->magic[0] != 'I' || ia->magic[1] != 'M' || ia->magic[2] != 'P')
    {
        return FALSE;
    }
    ia->imp->close_archive();
    free(ia->iai->ifh);
    return TRUE;
}

int CALLBACK ReadHeader(LPVOID ah,FileHeader *hdr,LPVOID userdata)
{
    ImpArchive *ia;
    if (ah == NULL || hdr == NULL)
    {
        return -1;
    }
    ia = (ImpArchive*)ah;
    if (ia->magic[0] != 'I' || ia->magic[1] != 'M' || ia->magic[2] != 'P')
    {
        return -1;
    }
    if (ia->now >= ia->iai->file_count)
    {
        return 0;
    }
    memset(hdr,0,sizeof(FileHeader));
    lstrcpy(hdr->filename,ia->iai->ifh[ia->now].filename);
    hdr->originalsize = ia->iai->ifh[ia->now].orgsize;
    hdr->packedsize = ia->iai->ifh[ia->now].compsize;
    hdr->ratio = 0;
    hdr->create_time = MAKELONG(ia->iai->ifh[ia->now].dirent.ctime,ia->iai->ifh[ia->now].dirent.cdate);
    DosDateTimeToFileTime(ia->iai->ifh[ia->now].dirent.cdate,ia->iai->ifh[ia->now].dirent.ctime,&hdr->create_time_ft);
    hdr->modified_time = hdr->lastaccess_time = MAKELONG(ia->iai->ifh[ia->now].dirent.mtime,ia->iai->ifh[ia->now].dirent.mdate);
    DosDateTimeToFileTime(ia->iai->ifh[ia->now].dirent.mdate,ia->iai->ifh[ia->now].dirent.mtime,&hdr->modified_time_ft);
    hdr->lastaccess_time_ft = hdr->modified_time_ft;
    hdr->crc = ia->iai->ifh[ia->now].dirent.crc;
    if (ia->iai->ifh[ia->now].dirent.flags & DFLAG_DIR)
    {
        lstrcpy(hdr->method,"IMPD");
    }
    else
    {
        lstrcpy(hdr->method,"IMP1");
    }
    hdr->os = 0;
    hdr->attribute = ia->iai->ifh[ia->now].dirent.attrib;
    hdr->date = ia->iai->ifh[ia->now].dirent.mdate;
    hdr->time = ia->iai->ifh[ia->now].dirent.mtime;
    lstrcpy(hdr->attribute_str,"");
    ia->now++;
    return 1;
}

int CALLBACK Command(LPVOID ah,BYTE command,const FileHeader *hdr,const OpenArchiveInfoDet mi,LPVOID userdata)
{
    ImpArchive *ia;
    if (ah == NULL)
    {
        return ERROR_HARC_ISNOT_OPENED;
    }
    ia = (ImpArchive*)ah;
    if (ia->magic[0] != 'I' || ia->magic[1] != 'M' || ia->magic[2] != 'P')
    {
        return ERROR_HARC_ISNOT_OPENED;
    }
    switch (command)
    {
    case COMMAND_EXTRACT:
        if (ia->now == 0 || ia->now > ia->iai->file_count)
        {
            return ERROR_NOT_SEARCH_MODE;
        }
        if (!(mi.flag & OPENARCHIVEINFO_DET_FLAG_OUTFILENAME))
        {
            return ERROR_UNKNOWN;
        }
        if (ia->imp->extract_file(ia->iai->ifh[ia->now - 1].dirent,mi.outfilename) == FALSE)
        {
            return 1;
        }
        break;
    case COMMAND_TEST:
        if (ia->now == 0 || ia->now > ia->iai->file_count)
        {
            return ERROR_NOT_SEARCH_MODE;
        }
        break;
    default:
        return ERROR_COMMAND_NAME;
    }
    return 0;
}
