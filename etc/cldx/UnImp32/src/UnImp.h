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
// $Id: UnImp.h,v 1.7 2005/06/03 13:17:19 hyperbeat Exp $
//

#ifndef __UNIMP_H__
#define __UNIMP_H__

#include <stdio.h>

#define MAX_VERSION 0x10A
#define MAXMETHOD METHOD_LZ77_MM

#define IMP_SIG 0xA504D49
#define IMP_SIG_0 0x49
#define IMP_SIG_1 0x4D
#define IMP_SIG_2 0x50
#define IMP_SIG_3 0xA

#define IMPDIRBLOCK 8192
#define MAXOVERLAP 524288
#define MAXFILECOMMENT 78

typedef unsigned char uchar;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef unsigned int uint;

#pragma pack(1)

typedef struct
{
    uint32 sig;
    uint32 diroffset;
    uint32 direntries;
    uint32 ecoffset;
    uint32 cmoffset;
    uint32 length;
    uint32 csize;
    uint16 mvsig;
    uint16 disk;
    uint16 dirdisk;
    uint16 cmdisk;
    uint16 ecdisk;
    uint16 flags;
    uint16 crc;
}IMP_HEADER;

typedef struct
{
    uint16 ver;
    uint16 disk;
    uint32 fpos;
    uint16 attrib;
    uchar commlen;
    uchar flags;
    uint32 unitpos;
    uint32 esize;
    uint32 crc;
    uint16 extra;
    uint16 namelen;
    uint16 ctime;
    uint16 cdate;
    uint16 mtime;
    uint16 mdate;
    uint16 entcrc;
}IMP_DIR_ENTRY;

typedef struct
{
    uint32 esize;
    uint16 namelen;
    uint16 ver;
    uint16 entcrc;
    uchar flags;
}LOCAL_HEADER;

typedef struct
{
    IMP_DIR_ENTRY dirent;
    char filename[MAX_PATH * 2];
    char comment[MAXFILECOMMENT + 1];
    uint32 compsize;
    uint32 orgsize;
    char method[8];
}IMP_FILE_HEADER;

typedef struct
{
    uint32 file_count;
    IMP_FILE_HEADER *ifh;
}IMP_ARCHIVE_INFO;

#pragma pack()

#define IMP_HEADER_SIZE 42
#define IMP_DIR_ENTRY_SIZE 38
#define LOCAL_HEADER_SIZE 11

#define DFLAG_DIR 1
#define DFLAG_E8SHORT 2
#define DFLAG_E8LONG 4

#define HFLAG_MULTIVOL 1
#define HFLAG_LASTVOL 2
#define HFLAG_BLOWFISH 4

#define METHOD_STORE 0
#define METHOD_LZ77 1
#define METHOD_BWT 2
#define METHOD_LZ77_MM 3

#define NLLCODES 288
#define NDCODES 42
#define ND2CODES 14
#define ND3CODES 28
#define MIN_LZ_CODES (NLLCODES + NDCODES + ND2CODES)
#define LONGMATCHBITS 15
#define LONGMATCHMASK ((1 << LONGMATCHBITS) - 1)
#define LONGMATCHCODE (NLLCODES - 2)
#define MMCODE (NLLCODES - 1)
#define LLTTBITS 9
#define DTTBITS 7
#define LLTTSIZE (1 << LLTTBITS)
#define DTTSIZE (1 << DTTBITS)
#define MAX_MM_CODES 1024

#define NCODES 258
#define WINDOW 50
#define BWTABLES 6
#define BWTTBITS 9
#define BWTTSIZE (1 << BWTTBITS)

#define MAX_NCODES NLLCODES
#define MAXCODELEN 16
#define NLENCODES (MAXCODELEN + 5)
#define ZRUN (MAXCODELEN + 1)
#define DZRUN (MAXCODELEN + 3)
#define HC_MCL 14
#define HCTTBITS 6
#define HCTTSIZE (1 << HCTTBITS)
#define USEDBITS(n) {g_bitpos -= n; g_bitbuf >>= n;}

#define jms1(c) (((((uchar)(c)) >= 0x81) && (((uchar)(c)) <= 0x9F)) || ((((uchar)(c)) >= 0xE0) && (((uchar)(c)) <= 0xFC)))
#define jms2(c) ((((uchar)(c)) != 0x7F) && (((uchar)(c)) >= 0x40) && (((uchar)(c)) <= 0xFC))

#define EIMP_OLDVERSION -1
#define EIMP_NOT_IMP -2
#define EIMP_DAMAGED -3
#define EIMP_BAD_DIR -4
#define EIMP_EOF -5
#define EIMP_WRITE -6
#define EIMP_NOCIPHER -7
#define EIMP_NOMULTIVOL -8
#define EIMP_BAD_CRC -9
#define EIMP_OUTOFMEMORY -10
#define EIMP_DIRECTORY -11
#define EIMP_FIND_FILE -12
#define EIMP_OPEN -13
#define EIMP_MAKE_FILE -14
#define EIMP_TOO_LONG_NAME -15

class CUnImp
{
public:
    CUnImp();
    ~CUnImp();
public:
    bool open_archive(const char *arc_name,IMP_ARCHIVE_INFO *iai);
    bool open_archive(FILE *fp,IMP_ARCHIVE_INFO *iai);
    void close_archive(void);
#ifndef SFX
    int is_exe(void);
#endif
    bool extract_file(IMP_DIR_ENTRY dirent,char *out_filename);
    int get_last_error(void);
public:
    static int isJMS(const char *str,int nPos);
    static bool mkdir2(const char *dirname);
private:
    void init(void);
    void init_crc_table(void);
    uint32 block_crc(void *block,int32 n,uint32 crc);
    void convert_path(char *path);
    uint16 get_uint16(void *src);
    uint32 get_uint32(void *src);
    void init_getbits(void);
    uint32 getbits(int nbits);
    int32 unstore(uchar *output,int32 size);
    void make_codes(uint *huffcodes,uchar *hclengths,int ncodes);
    int make_decode_tables(int *ttable,int hdchain[][2],uint *huffcodes,uchar *hclengths,int ttbits,int ncodes);
    void load_fixed_codes(uchar *hclens,int32 exsize);
    int load_hcodes(uchar *hclengths,int ncodes,int ntrees,int32 exsize);
    void reverse_mm(uchar *block,int32 bsize);
    void redo_mm(uint32 overlap);
    int32 expand_lz(uchar *output,int32 start,int32 exsize,int mm);
    int e8ut_crc_wr16(FILE *outfile,uchar *block,int32 size,uint32 e8offset);
    int e8ut_crc_wr32(FILE *outfile,uchar *block,int32 size,uint32 e8offset,uint32 srcsize);
    int expand_bwt(uchar *output,int32 exsize);
    int e8ut_crc_write(FILE *outfile,uchar *block,int32 size,uint32 e8offset,uint32 srcsize,int last);
    int decomp(FILE *outfile,uint32 fpos,uint32 unitpos,uint32 togo,uchar flags);
    int find_start(void);
    int extract_dir(void);
    int get_dir_entry(IMP_DIR_ENTRY *pdirent,char *name,char *comment);
    int get_impcomment(uint32 cmoffset,char *impcomment);
    int make_list(IMP_ARCHIVE_INFO *iai);
    void set_last_error(int error);
#ifndef SFX
    int convert_imperror(int error);
#endif
private:
    int g_e8t,g_bitpos,g_mmptr,g_eof,g_ncodes[3],g_ncoffset[3],g_last_error;
    uint g_exesize,g_e8extra;
    char g_dirbuf[IMPDIRBLOCK],*g_dirbufsrc,*g_dirbufend;
    FILE *g_archfile;
    uchar g_exprev,g_exlast,g_exerror,g_dirsig[6],g_e8buf[8];
    uchar *g_blockbuffer,*g_dcsrc,*g_dcend,g_dcbuffer[256];
    uint32 g_crc,g_crctbl[256],g_dirfpos,g_prevfpos,g_unitoffset,g_exmemory;
    uint32 g_blockstart,g_blocksize,g_bitbuf,g_mmlist[MAX_MM_CODES],g_overlaps[3][8];
};

#endif
