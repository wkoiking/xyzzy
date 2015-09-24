/* Modified for DLL by H. Shimada. */

/*	Copyright 1993,94 H.Ogasawara (COR.)	*/
#include "aish.h"

#include <cstdio>
#include <cstdlib>

#include "ai.h"
#include "microfio.h"

using namespace std;

#include "ai_ish.h" /* for fm and hp */
#include "ai_fio.h" /* for funcs and noerrflag */
#include "ai_nam.h"
#include "ai_unix.h"
#include "ai_hed.h"
#include "ai_blk2.h"
#include "mimedec.h"
#include "aisherr.h"

#define	ishNOP		0
#define	ishDECODE	1
#define	ishENCODE	2
#define	ishLIST		3
#define	RBSIZE		256

#define MSGBUF_SIZE (1024 * 2)

static void
usage(void)
{
	Mprint( "\
aish v1.12 ish/uuencode converter 1993,94,95 H.Ogasawara (COR.)\r\n\
usage: aish [-pfadhAlUo<file>x<file>] <decode text> ..\r\n\
       aish -{s78nu} [-iadco<file>x<file>O<n>t<n>] <encode file> ..\
\r\n"
		);
}

unsigned char *
getword(unsigned char *str, unsigned char *ptr)
{
	for(; *ptr == ' ' || *ptr == '\t' ; ptr++ )
		;
	for(; *ptr && *ptr != ' ' && *ptr != '\t' ; *str++ = *ptr++ )
		;
	*str= '\0';

	return	ptr;
}

#if PREREAD
static int	PreFlag= FALSE;
static unsigned char	PreBuf[RBSIZE];

static
MMgets(m_file *fm, unsigned char *ptr, unsigned int len)
{
	if( PreFlag ){
		PreFlag= FALSE;
		return	(unsigned char*)p_strcpy( ptr, PreBuf )-ptr;
	}
	return	Mgets( fm, ptr, len );
}
unsigned char *
PreRead(void)
{
	Mgets( &fm, PreBuf, RBSIZE-8 );
	PreFlag= TRUE;
	return	PreBuf;
}
#endif


int aish_main(int argc, char *argv[])
{
	unsigned char ptr[RBSIZE];
	static char msgbuf[MSGBUF_SIZE];
	int encode= ishNOP;
	int ret;

	ret = EXIT_SUCCESS;

	memclr( sizeof(T_HD), &hp );
	hp.stepline= 200;
#if UNIX
	hp.mode= mAPPEND;
#else
	hp.mode= 0;
#endif

	for(; --argc && (p_strcpy( ptr, (unsigned char *)*++argv ), 1) ;) {
		if( encode == ishNOP )
			encode= ishDECODE;
		if( *ptr == '-' && ptr[1] ){
			unsigned char	*p= ptr+1;
			for(; *p ; p++ ) switch( *p ){
			case 'l':
				hp.mode|= mLIST;
				encode= ishLIST;
				break;
			case 'r':
				encode= ishDECODE;
				break;
			case '7':
				hp.ztype= zJIS7;
				encode= ishENCODE;
				break;
			case '8':
				hp.ztype= zJIS8;
				encode= ishENCODE;
				break;
			case 's':
				hp.ztype= zSJIS;
				encode= ishENCODE;
				break;
			case 'n':
				hp.ztype= zSJISN;
				encode= ishENCODE;
				break;
			case 'u':
				hp.ztype= zUUE;
				encode= ishENCODE;
				break;
			case 't':
				if( p[1] ){
					hp.stepline= numstr( p+1 );
				}else{
					if( argc >= 2 ){
						argc--;
						hp.stepline= numstr( (unsigned char *)*++argv );
					}
				}
				if( hp.stepline <= 2 )
					hp.stepline= -1;
				goto	_forbreak;
			case '_':
				hp.volsize= numstr( p+1 );
				goto	_forbreak;
			case '/':
				hp.tcrc= numstr( p+1 );
				goto	_forbreak;
			case '-':
				hp.tcrc32= numstr( p+1 );
				goto	_forbreak;
#if NOERR
			case '+':
				{
				noerrflag = TRUE;  /* noerrflag is global in "ai_fio.c" */
				}
				break;
#endif
			case 'O':
				if( p[1] ){
					int	i= numstr( p+1 );
					hp.os= i;
					hp.vol= i>>16;
				}else{
					int	i;
					if( argc >= 2 ){
						argc--;
						i= numstr( (unsigned char *)*++argv );
					}
					hp.os= i;
					hp.vol= i>>16;
				}
				goto	_forbreak;
			case 'p':
				hp.mode|= mPRTOUT;
				break;
			case 'i':
				hp.mode|= mNOTSUM;
				break;
			case 'A':
				hp.mode|= mADJNAME;
				break;
			case 'a':
				hp.mode|= mAPPEND;
				break;
			case 'd':
				hp.mode&= ~mAPPEND;
				break;
			case 'U':
			case 'c':
				hp.mode|= mUUENC;	/* mUUCHK */
				break;
			case 'f':
				hp.mode|= mOWRITE;
				break;
			case 'h':
				hp.mode|= mFHEAD;
				break;
			case 'm':
				hp.mode|= mMIME;
				break;
			case 'x':
				if( p[1] )
					p_strcpy( hp.skipname, p+1 );
				else
					if( argc >= 2 ){
						argc--;
						p_strcpy( hp.skipname, (unsigned char *)*++argv );
					}
				if( *hp.skipname )
					hp.mode|= mSKIP;
				goto	_forbreak;
			case 'o':
				hp.mode|= mOUTPUT;
				if( p[1] )
					p_strcpy( hp.outname, p+1 );
				else
					if( argc >= 2 ){
						argc--;
						p_strcpy( hp.outname, (unsigned char *)*++argv);
					}
				if( *hp.outname )
					goto	_forbreak;
			default:
				usage();

				return EXIT_SUCCESS;  /* ただ使い方を表示する */
			}

			_forbreak:;

		}else if( encode == ishDECODE ){

			/* ISH のデコード */

			if( Mopen( &fm, (char *)ptr ) ){
				unsigned int	len, nodec= TRUE;
#if MIMEDEC
				if( hp.mode & mMIME ){
					MimeDec( &fm );
				}else
#endif
				{
				for(; (int)(len = MMgets( &fm, ptr, RBSIZE-8 )); ) {
					if( (hp.mode & mPRTOUT) && nodec ){
						Mprint( (char *)ptr ),
						Mprint( "\r\n" );
					}
					if( !--len )
						continue;
					nodec= ai_decode( ptr, len );
				}
				if( !nodec )
					ai_decode_lastchance();
				}
				Mclose( &fm );
			} else {
				/* ISH ファイルが開けなかった */
				sprintf(msgbuf, "Can't open the ISH file \"%s\".\r\n", ptr);
				Merr(msgbuf);
				ret = AISH_ARC_FILE_OPEN;
			}

		}else if( encode == ishLIST ){

			/* ISH ファイルに含まれているファイルのリストを表示 */

			if( Mopen( &fm, (char *)ptr ) ){
				unsigned int	len, line;
				for(line= 1; (int)(len = Mgets(&fm, ptr, RBSIZE-8)); line++) {
					if( checkhead( &hp, ptr, len-1 ) ){
						Numput( line );
						Mprint( "\r\n" );
					}
				}
				Mclose( &fm );
			}else {
				// ISH ファイルが開けなかった
				sprintf(msgbuf, "Can't open the ISH file \"%s\".\r\n", ptr);
				Merr(msgbuf);
				ret = AISH_FILE_OPEN;
			}
		}else if( encode == ishENCODE ){

			/* ISH ファイルにエンコード */

			unsigned int encode_stat;

			p_strcpy( hp.fname, ptr );
			hp.initflag = TRUE;
			while((encode_stat = encblock( &hp )) == ENCODE_SUCCESS)
				;
			if(encode_stat == EOISH)
				ret = EXIT_SUCCESS;
			else if(encode_stat == ENCODE_CANT_OPEN_INFILE)
				ret = AISH_CANT_READ_INFILE;
			else if(encode_stat == ENCODE_CANT_CREATE_OUTFILE)
				ret = AISH_ARC_FILE_OPEN;
		}
	}
	if( encode == ishNOP ) {
		usage();
		ret = EXIT_SUCCESS;
	}

	return ret;
}
