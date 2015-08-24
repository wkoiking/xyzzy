/* Modified for DLL by H. Shimada. */

/*	Copyright 1994 H.Ogasawara (COR.)	*/

#include "aishmv.h"

#include <windows.h>

#include <cerrno>
#include <cprocess>
#include <cstdio>
#include <cstdlib>

#if UNIX
#  include	<sys/stat.h>
#endif /* UNIX */

#include <utime.h>

#include "config.h"
#include "microfio.h"

#include "ai_fio.h"
#include "ai_crc.h"
#include "ai_crc32.h"
#include "ai_nam.h"
#include "aisherr.h"

#include "aish32.h"

using namespace std;

#if !defined(TRUE)
#  define TRUE  1
#  define FALSE 0
#endif /* TRUE */

#ifndef NULL
#  define	NULL	0
#endif /* NULL */

/* aish() の戻り値 */
#define CANT_SPLIT        (-1)   /* 分割できない           */
#define TOO_MANY_VOLUMES  (-2)   /* 分割の指定が多すぎる   */
#define TEMP_CREATE_ERROR (-3)   /* ファイルが作成できない */

#define		AISH	"aish"
#define		TMPNM	"aiSmvTmp.tmp"

#define		BUFSIZE	(1024 * 32)
#define		RBSIZE	256

static
unsigned int	fsize,
				line= 500,
				volume= 1,
				volmax= 0,
				volsize= 0,
				mode= 0,
				fcrc16,
				fcrc32,
				os= 0,
				fdate,
				title= 200,
				iopt= FALSE,
#if UNIX
				aopt= TRUE;
#else
				aopt= FALSE;
#endif

static
unsigned char	fname[NAMELEN],
				sname[NAMELEN],
				tmpnm[PATHLEN],
				buf[BUFSIZE];

static m_file	fm, fmo;

unsigned char	sjis_chk[]= {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0
	};

static void
usage(void)
{
	Mprint( "\
aishmv 1.12 Multi-Volume ish encoder 1994,95 H.Ogasawara (COR.)\r\n\
usage: aishmv -{s78n} [-iado<file>T<file>O<n>t<n>m<n>] <encode file> ..\r\n" );
}


static void
mverr(char *msg1, char *msg2)
{
	Merr( msg1 );
	Merr( msg2 );
	Merr( "\r\n" );
}

unsigned char *
pp_strcpy(unsigned char *s, unsigned char *p)
{
	for(; (int)(*s++ = *p++););

	return	s - 1;
}

unsigned char *
getword(unsigned char *str, unsigned char *ptr)
{
	for(; *ptr && (*ptr == ' ' || *ptr == '\t') ; ptr++ );
	for(; *ptr && *ptr != ' ' && *ptr != '\t' ; *str++= *ptr++ );
	*str= '\0';
	return	ptr;
}

#if UNIX
static time_t
fgetdate(int handle)
{
	struct stat	st;

	fstat(handle, &st);
	return	st.st_mtime;
}

static void
fsetdate(const char *name, unsigned int date)
{
	time_t tim[2];

	tim[0] = tim[1] = date;
	utime(name, (struct utimbuf *)tim);
}

#endif

#if BCB
static int
dummy_use(int i)
{
	return i;
}
#endif /* BCB */

/* argc と argv からコマンド・ラインを作成する */
static char *
build_cmd_line(char *cmd_line, int argc, char *argv[])
{
	const char dquort[] = "\"";
	const char space[] = " ";
	int i;

	*(cmd_line + 0) = '\0';
	for(i = 1 ; i < argc ; ++i) {
		if(i != 1)
			strcat(cmd_line, space);
		if(strchr(argv[i], space[0]) == NULL) {
			strcat(cmd_line, argv[i]);
		} else {
			strcat(cmd_line, dquort);
			strcat(cmd_line, argv[i]);
			strcat(cmd_line, dquort);
		}
	}

	return cmd_line;
}


/* static int qexec(const HWND hWnd, char *ptr);
 *
 * hWnd   : オーナー・ウィンドウのハンドル
 * ptr    : コマンド・ライン
 * 戻り値
 *     0 以上: Aish() の戻り値
 *     -1    : AISH32.DLL のロードに失敗した
 *     -2    : AISH32.DLL に Aish() 関数が見つからなかった
 */
static int
qexec(const HWND hWnd, char *ptr)
{
	char	*buf[1024], **ptrs= buf;
	int ret_val;
	char *err_msg;
	HANDLE dllHandle;
	int argc;
	char cmd_line[8192];
	AishType aish32;

	for(; *ptr ;){
		for(; *ptr == ' ' ; *ptr++= '\0' )
			;
		if( *ptr )
			for( *ptrs++ = ptr ; *ptr && *ptr != ' ' ; ptr++ )
				;
	}
	*ptrs++ = NULL;
#if BCB             /* BCB の警告対策 */
	dummy_use((int)ptrs);
#endif

	dllHandle = LoadLibrary("AISH32.DLL");
	if(dllHandle) {
		aish32 = reinterpret_cast<AishType>(GetProcAddress(dllHandle, "Aish"));
		if(aish32) {
			for(argc = 0 ; buf[argc] ; ++argc)
				;
			ret_val = aish32(hWnd, build_cmd_line(cmd_line, argc - 1, buf + 1),
				Gstdout_buf + Gstdout_buf_pos,
				Gstdout_buf_size - (unsigned int)Gstdout_buf_pos);
			if(ret_val >= 0) {
				Gstdout_buf_pos = strlen(Gstdout_buf);
			}
		} else {
			ret_val = -2; /* Aish() 関数が見つからない */
		}
	} else {
		ret_val = -1;  /* AISH32.DLL を LoadLibrary() できない */
	}

	if(ret_val == -1) {
		err_msg = "AISH32.DLL をロードできなかった。";
		mverr(*buf, ":LoadLibrary() API failure");
		mverr(err_msg, "");
	} else if(ret_val == -2) {
		err_msg = "Aish 関数が AISH32.DLL 内に見つからなかった。";
		mverr(*buf, ":GetProcAddress() API failure");
		mverr(err_msg, "");
	}

	if(dllHandle)
		FreeLibrary(dllHandle);

	return ret_val;
}

static int
filecrc(void)
{
	unsigned short	crc16= 0xffff;
	unsigned int	crc32= 0xffffffff;
	int		total= 0, size;

	do{
		total+= size= Mread( &fm, buf, BUFSIZE );
		crc16= (unsigned short)crc_check( crc16, buf, size );
		crc32= (unsigned int)crc_check32( crc32, buf, size );
	}while( size == BUFSIZE );
	fcrc16= (~crc16) & 0xffff;
	fcrc32= ~crc32;
	return	total;
}

static void
calcline(void)
{
	int	body= 69-3, hline;
	if( mode == '7' )
		body= 63-3;
	else if( mode == 's' )
		body= 73-3;
	hline= line-6 -(line/title);
	volsize= (hline -(iopt ? -2 : (hline/body+1)*2))*body;
	volmax= (fsize+volsize-1)/volsize;
}

/* static int aish(const HWND hWnd, int *qexec_ret_value)
 *
 * hWnd : オーナー・ウィンドウのハンドル
 * *aish_ret_value: qexec() の戻り値           (return)
 *
 * 戻り値
 *     0                : まだ処理を続けるべきとき
 *     1                : 処理が正常に終わったとき
 *     CANT_SPLIT       : 分割できない
 *     TOO_MANY_VOLUMES : 分割の指定が多すぎる
 *     TEMP_CREATE_ERROR: 作業用ファイルが作成できない
 */
static int
aish(const HWND hWnd, int *qexec_ret_value)
{
	unsigned char cmd[1024], ofname[NAMELEN], ifname[NAMELEN];
	int last = 0;

#if BCB
	dummy_use(last);    /* BCB の警告対策 */
#endif

	calcline();

	if( volmax == 1 ){
		mverr( (char *)fname, ":can't split" );
		return	CANT_SPLIT;
	}
	if( Mcreate( &fmo, (char *)tmpnm ) ){
		unsigned int	rsize, size, total = volsize;
		unsigned char	*ptr, *str = cmd;

		do {
			rsize = (total > BUFSIZE) ? BUFSIZE : total;
			size = Mread(&fm, buf, rsize);
			last = size < rsize;  // 正常終了で 1 となる

			/* 次の if 文の比較はオリジナルでは < だった */
			/* size_t < 0 はいつも偽                     */
			/* だから、たぶん == だと思う                */
			/*                                  by WDN   */
			if(Mwrite(&fmo, (char *)buf, size) == 0)
				mverr((char *)tmpnm, ":tmp write err");
		} while(!last && (total -= rsize));

		Mclose( &fmo );
#if UNIX
		fsetdate( (char *)tmpnm, fdate );
#endif
		ishname( ifname, fname );
		ptr= (unsigned char*)ishname( ofname, *sname ? sname : fname );
		if( *ofname != '-' || ofname[1] ){
			if( aopt )
				for(; *ptr ; ptr++ );
			*ptr++ = '.';
			*ptr++ = (unsigned char)
			         ('I' + (volume / 100)); /* 分割 ISH の拡張子 */
			*ptr++ = (unsigned char)(((volume/10)%10)+'0');
			*ptr++ = (unsigned char)((volume%10)+'0');
			*ptr   = '\0';
		}

		str= pp_strcpy( str, (unsigned char *)AISH );
		*str++= ' ';
		*str++= '-';
		*str++= 'O';
		str+= strnum( str, (volume<<16)+(volmax<<8)+(os&255) );
		*str++= ' ';
		*str++= '-';
		*str++= '_';
		str+= strnum( str, fsize );
		*str++= ' ';
		*str++= '-';
		*str++= '/';
		str+= strnum( str, fcrc16 );
		*str++= ' ';
		*str++= '-';
		*str++= '-';
		str+= strnum( str, fcrc32 );
		*str++= ' ';
		*str++= '-';
		*str++= 't';
		str+= strnum( str, title );
		*str++= ' ';
		*str++= '-';
		*str++= 'x';
		str= pp_strcpy( str, ifname );
		*str++= ' ';
		*str++= '-';
		if( iopt )
			*str++= 'i';
		*str++ = (unsigned char)mode;
		*str++ = 'o';
		str= pp_strcpy( str, ofname );
		*str++= ' ';
		str= pp_strcpy( str, tmpnm );
#if BCB
		dummy_use((int)str);      /* BCB の警告対策 */
#endif

#if defined(AISHMV_DEBUG)
		sprintf( cmd, "%s -O%d -_%d -/%d --%d -t%d -x%s -%s%co %s %s",
				AISH, (volume<<16)+(volmax<<8)+(os&255),
				fsize, fcrc16, fcrc32, title, ifname,
				iopt ? "i" : "",
				mode, ofname, tmpnm );
		Mprint( cmd );
		Mprint( "\r\n" );
#endif
		*qexec_ret_value = qexec(hWnd, (char *)cmd);
		unlink( (char *)tmpnm );
		if( ++volume > 250 ){
			mverr( (char *)fname, ":too many volumes" );
			return	TOO_MANY_VOLUMES;
		}
	}else{
		mverr( (char *)tmpnm, ":create error" );
		return	TEMP_CREATE_ERROR;
	}
	return	last;
}

static void
gettemp(unsigned char *ptr)
{
	const unsigned char *tmpdir;

	tmpdir = (unsigned char *)getenv("TEMP");
	if(tmpdir == NULL)
		tmpdir = (unsigned char *)getenv("TMP");
	if(tmpdir == NULL)
		tmpdir = (unsigned char *)getenv("TMPDIR");
	if(tmpdir == NULL)
		tmpdir = (unsigned char *)"";

	p_strcpy(ptr, tmpdir);
	while(*ptr)
		++ptr;
	p_strcpy(ptr, (unsigned char *)"\\");  /* デリミタはもっと凝らなくちゃ */
	while(*ptr)
		++ptr;

	p_strcpy(ptr, (unsigned char *)TMPNM);
}


/* コマンド・ライン版では main() だった関数 */
int
aishmv_main(const HWND hWnd, int argc, char *argv[])
{
	unsigned char ptr[RBSIZE];
	int qexec_ret_value;
	int aish_ret;
	int ret;


	ret = EXIT_SUCCESS;
	*sname= *fname= '\0';
	gettemp( tmpnm );

	/* コマンド・ラインの解析 */
	for(; --argc && (p_strcpy( ptr, (unsigned char *)*++argv ),1) ;) {
		if( *ptr == '-' && ptr[1] ){
			unsigned char	*p= ptr+1;
			for(; *p ;) switch( *p++ ){
			case 's':
			case '7':
			case '8':
			case 'n':
				mode= p[-1];
				break;
			case 'm':
				if( *p ){
					line= numstr( p );
				}else{
					if( argc >= 2 ){
						argc--;
						line= numstr( (unsigned char *)*++argv );
					}
				}
				if( line < 20 )
					line= 20;
				goto	_fbreak;
			case 'i':
				iopt= TRUE;
				break;
			case 'a':
				aopt= TRUE;
				break;
			case 'd':
				aopt= FALSE;
				break;
			case 't':
				if( *p ){
					title= numstr( p );
				}else{
					if( argc >= 2 ){
						argc--;
						title= numstr( (unsigned char *)*++argv );
					}
				}
				goto	_fbreak;
			case 'O':
				if( *p ){
					os= numstr( p );
				}else{
					if( argc >= 2 ){
						argc--;
						os= numstr( (unsigned char *)*++argv );
					}
				}
				goto	_fbreak;
			case 'o':
				if( *p ){
					p_strcpy( sname, p );
				}else{
					if( argc >= 2 ){
						argc--;
						p_strcpy( sname, (unsigned char *)*++argv );
					}
				}
				goto	_fbreak;
			case 'T':
				if( *p ){
					p_strcpy( tmpnm, p );
				}else{
					if( argc >= 2 ){
						argc--;
						p_strcpy( tmpnm, (unsigned char *)*++argv );
					}
				}
				goto	_fbreak;
			default:
				usage();

				return EXIT_SUCCESS;
			}
			_fbreak:;
		}else if( mode ){
			p_strcpy( fname, ptr );
			if( Mopen( &fm, (char *)fname ) ){
#if UNIX
				fdate= fgetdate( fm.fn );
#endif
				Merr( "crc check.." );
				fsize= filecrc();
				Mseek( &fm, 0, 0 );
				Merr( "\r\n" );
				while((aish_ret = aish(hWnd, &qexec_ret_value)) == 0)
					;
				Mclose( &fm );
				if(aish_ret == 1)
					ret = EXIT_SUCCESS;
				else if(aish_ret == CANT_SPLIT)
					ret = AISH_CANT_SPLIT;
				else if(aish_ret == TOO_MANY_VOLUMES)
					ret = AISH_TOO_MANY_VOLUMES;
				else if(aish_ret == TEMP_CREATE_ERROR)
					ret = AISH_TMP_OPEN;
			} else {
				mverr( (char *)fname, ":open error" );
				ret = AISH_CANT_READ_INFILE;
			}
		}
	}
	if( !mode )
		usage();

	return ret;
}

