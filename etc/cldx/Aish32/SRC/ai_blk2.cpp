/* Modified for DLL by H. Shimada. */

/* 1993,94 H.Ogasawara (COR.) */
#include "ai_blk2.h"

#include "ai.h"
#include "microfio.h"

#include "ai_crc.h"
#include "ai_fio.h"
#include "ai_nam.h"
#include "ai_uue.h"
#include "ai_unix.h"
#include "ai_hed.h"
#include "ai_7.h"
#include "ai_ish.h"

static INLINE void
crc_set(unsigned char *str, int len)
{
	unsigned short	crc= ~crc_check( 0xffff, str, len );
	str[len  ]= crc>>8;
	str[len+1]= crc;
}

#if !CSUMASM
static void INLINE
sum_check(T_HD *hp, int pos,
  unsigned char *ptr, unsigned char *sumh, unsigned char *sumv)
{
	int	i, j, len;

	len = hp->byte-2;
	for(i= 1; i < len; i++)
		sumv[i] -= ptr[i];
	for(j = (len - pos+1) % len, i= 0; i < len; i++, j= (j+1) % len)
		sumh[j+1] -= ptr[i];
/*
	pos:   1  2  3  4  5  6  7
	len:                        8
	j:     7  6  5  4  3  2  1  0
	j+1:   8  7  6  5  4  3  2  1
	%len:  1  8  7  6  5  4  3  2
	%len:  2  1  8  7  6  5  4  3
	%len:  3  2  1  8  7  6  5  4
	%len:  4  3  2  1  8  7  6  5
	%len:  5  4  3  2  1  8  7  6
	%len:  6  5  4  3  2  1  8  7
	%len:  7  6  5  4  3  2  1  8
	%len:  8  7  6  5  4  3  2  1

*/
}
#endif

static void
enc_comment(T_HD *hp)
{
	char	buf[20];
	static unsigned	oline;
	if( hp->cline == oline )
		return;
	Mputs( &fmo, "--- " );
	Mputs( &fmo, (char *)hp->fname );
	if( hp->vol ){
		Mputs( &fmo, " " );
		strnum( (unsigned char *)buf, hp->vol );
		Mputs( &fmo, buf );
		Mputs( &fmo, "/" );
		strnum( (unsigned char *)buf, hp->os>>8 );
		Mputs( &fmo, buf );
	}
	Mputs( &fmo, " (" );
	strnum( (unsigned char *)buf, hp->cline );
	Mputs( &fmo, buf );
	Mputs( &fmo, "/" );
	strnum( (unsigned char *)buf, hp->dline );
	Mputs( &fmo, buf );
	Mputs( &fmo, ") ---\r\n" );
	oline= hp->cline;
}

static void
print_line(T_HD *hp, unsigned char *str)
{
	Mputs( &fmo, (char *)str );
	Mputs( &fmo, "\r\n" );
	hp->cline++;
	if( hp->cline == hp->nextline ){
		hp->cline++;
		enc_comment( hp );
		hp->nextline+= hp->stepline;
	}
}

#if BCB
#include <stdlib.h>
static void
dummy_use(unsigned char *ptr)
{
	/* ŠO•”‚É‚Í‰e‹¿‚ª‚È‚¢ */
	ptr = (unsigned char *)malloc(16);
	*ptr = *(ptr + 1) + *(ptr + 2);
	free(ptr);
}
#endif

static unsigned char sumh[DBUF], sumv[DBUF];

unsigned int
encblock(T_HD *hp)
{
	static unsigned int	pos;
	static unsigned short	crc;
	unsigned int		ret = ENCODE_SUCCESS;
	unsigned char		str[DBUF],
				buf[DBUF];
#if UUENCODE
	if( hp->ztype == zUUE )
		return	uuencode( hp );
#endif
	if( hp->initflag ){
		unsigned char	*ptr;
		if( !Mopen( &fm, (char *)hp->fname ) ){
			Merr( "open error\r\n" );
			return ENCODE_CANT_OPEN_INFILE;
		}
		ptr = ishname( str, hp->fname );
		p_strcpy( hp->fname, str );
		if( hp->mode & mOUTPUT ){
			p_strcpy( str, hp->outname );
		}else{
			if( hp->mode & mAPPEND ) {
				for(; *ptr ; ptr++ )
					;
			}
			*ptr++ = '.';
			*ptr++ = 'I';
			*ptr++ = 'S';
			*ptr++ = 'H';
			*ptr++ = '\0';
			dummy_use(ptr); /* BCB ‚ÌŒx‘Îô */
		}
		Merr( (char *)str );
		Merrch( ':' );
		if( !Mcreate( &fmo, (char *)str ) ){
			Mclose( &fm );
			Merr( "create error\r\n" );
			return ENCODE_CANT_CREATE_OUTFILE;
		}
#if UNIX
		hp->fsize= filesize( fm.fn );
		hp->date= filedate( fm.fn );
#else
		hp->fsize= SEEK( fm.fn, 0, 2 );
		SEEK( fm.fn, 0, 0 );
		hp->date= FILEDATE( fm.fn, 0 );
#endif
		if( *hp->skipname )
			p_strcpy( hp->fname, hp->skipname );
		Mputs( &fmo, "<<< " );
		Mputs( &fmo, (char *)hp->fname );
		Mputs( &fmo, " " );
		if( hp->vol ){
			strnum( str, hp->vol );
			Mputs( &fmo, (char *)str );
			Mputs( &fmo, "/" );
			strnum( str, hp->os>>8 );
			Mputs( &fmo, (char *)str );
			Mputs( &fmo, " " );
		}
		strnum( str, hp->fsize );
		Mputs( &fmo, (char *)str );
		Mputs( &fmo, " byte (" );
		{
			T_Z	*zp= ztype+hp->ztype;
			hp->byte= zp->byte;
			hp->exec= zp->encexec;
			hp->sf_tbl= zp->sf_tbl;
			Mputs( &fmo, (char *)zp->name );
		}
		hp->body= hp->byte-3;
/*
<<< adepo023.lzh 304023 byte 00/00/00 00:00 (sjis) [9879 lines] aish 0.93 >>>
<<< adepo023.lzh 304023 byte (sjis) [9879 lines] aish 0.93 >>>
<<< adepo023.lzh 4/10 304023 byte (sjis) [9879 lines] aish 0.93 >>>
*/
		Mputs( &fmo, ") [" );
		hp->cline= 1;
		hp->dline= hp->fsize/hp->body+1+5+
				(hp->mode & mNOTSUM ? -2 :
				 (hp->fsize/(hp->body*hp->body)+1)*2);
		hp->nextline= hp->stepline-1;
		if( hp->dline != hp->stepline )
			hp->dline+= hp->dline/hp->stepline;
		strnum( str, hp->dline );
		Mputs( &fmo, (char *)str );
		Mputs( &fmo, " lines] aish 1.12 >>>\r\n" ); /* <=== Ver !!*/
		writehead(hp, (struct headline *)buf); /* ŠëŒ¯‚©‚à WDN */
		crc_set( buf, 63-2      /*jis7*/ );
		encline_7( str, buf, 63 /*jis7*/ );
		print_line( hp, str );
		if( !(hp->mode & mNOTSUM) ){
			print_line( hp, str );
			print_line( hp, str );
		}
		crc= 0xffff;
		pos= 1;
		hp->initflag= FALSE;
	}else{
		static unsigned int	lastblock= FALSE;
		buf[0]= (unsigned char)pos;
		if( lastblock ){
			buf[1]= crc;
			memclr( hp->byte-2, buf+2 );
			lastblock= FALSE;
			ret= EOISH;
		}else{
			unsigned int	byte= Mread( &fm, buf+1, hp->body );
			crc= crc_check( crc, buf+1, byte );
			if( byte < hp->body ){
				int	len;
				buf[byte+1]= (crc=~crc)>>8;
				if( (len= hp->body-byte) >= 2 ){
					buf[byte+2]= crc;
					memclr( len, buf+byte+3 );
					ret= EOISH;
				}else
					lastblock= TRUE;
			}
		}
		if( pos == 1 ){
			memclr( DBUF, sumh );
			memclr( DBUF, sumv );
		}
#if !CSUMASM
		sum_check( hp, pos, buf, sumh, sumv );
#else
		sum_check( pos, hp->body, buf, hp->sf_tbl );
#endif
		crc_set( buf, hp->byte-2 );
		(*hp->exec)( str, buf, hp->byte );
		print_line( hp, str );
		if( pos == hp->body || ret == EOISH ){
			if( !(hp->mode & mNOTSUM) ){
				sumv[0]= (unsigned char)(hp->byte - 2);
				crc_set( sumv, hp->byte-2 );
				(*hp->exec)( str, sumv, hp->byte );
				print_line( hp, str );
#if !CSUMASM
				sum_check( hp, hp->byte-2, sumv, sumh, sumv );
#else
				sum_check2( hp->byte-2, hp->body, hp->sf_tbl );
#endif
				sumh[0]= (unsigned char)hp->byte;
				if( ++pos != hp->byte-2 ){
					for(; pos < hp->byte-2 ; pos++ ){
						unsigned int	i=
							((hp->byte-2)-(pos-1))%
							(hp->byte-2)+1;
						sumh[i] = (unsigned char)
						            ((sumh[i] & 0xff) - (pos & 0xff));
					}
				}
				crc_set( sumh, hp->byte-2 );
				(*hp->exec)( str, sumh, hp->byte );
				print_line( hp, str );
			}
			pos= 0;
			Merrch( 'o' );
			if( ret == EOISH ){
				hp->cline++;
				enc_comment( hp );
				Mflush( &fmo );
				Mclose( &fmo );
				Merr( "\r\n" );
				Mclose( &fm );
			}
		}
		pos++;
	}
	return	ret;
}

