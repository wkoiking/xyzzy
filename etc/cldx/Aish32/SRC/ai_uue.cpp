/* Modified for DLL by H. Shimada. */

/* Copyright 1993,94 H.Ogasawara (COR.) */

/* v1.00 10/17   Ogasawara Hiroyuki (oga@dgw.yz.yamagata-u.ac.jp) */

#if UUENCODE

#include "ai_uue.h"

#include "ai.h"
#include "microfio.h"

#include "ai_ish.h"
#include "ai_fio.h"
#include "ai_nam.h"
#include "ai_hed.h"
#include "ai_unix.h"
#include "aish.h"
#include "ai_blk2.h" // uuencode() は encblock() と同じリターン・コードを使う

#define	UUCHKSUM

static char	decmap[]= {
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
	16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
	32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
	48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
	 0,-1,-1,-1,-1,-1, 6,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

static unsigned char	encmap[]= {
	0x60,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
	0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
	0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
	0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
	0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
};


#if !defined(TRUE)
#  define TRUE  1
#  define FALSE 0
#endif /* TRUE */

#define	ERR	(-1)
#define	MAXLINE	256
#define	LBYTE	45

static int
DecodeLine(unsigned char *str, unsigned char *ptr, unsigned int uulength)
{
	int	byte, len, sum= 0, sumx= 0;
	if( (byte= decmap[*ptr++]) < 0 )
		return	ERR;
	if( uulength == -1 ){
		if( byte > 63 )
			return	ERR;
	}else{
		if((unsigned int)byte > uulength)
			return	ERR;
	}
	for( len= (byte+2)/3*4 ; len > 0 ; len-= 4 ){
		int	code0, code1, code2, code3;
		sumx+= *ptr +ptr[1] +ptr[2] +ptr[3];
		if( (code0= decmap[*ptr++]) < 0 ||
		    (code1= decmap[*ptr++]) < 0 ||
		    (code2= decmap[*ptr++]) < 0 ||
		    (code3= decmap[*ptr++]) < 0 )
			return	ERR;
		sum += *str++ = (unsigned char)((code0 << 2) + (code1 >> 4));
		sum += *str++ = (unsigned char)((code1 << 4) + (code2 >> 2));
		sum += *str++ = (unsigned char)((code2 << 6) + (code3     ));
	}
	if( *ptr >= 'a' && *ptr <= 'z' )
		ptr++;
	else if( (len= decmap[*ptr]) >= 0 ){
		ptr++;
		if( (sum&0x3f) != len && (sumx&0x3f) != len )
			return	ERR;
	}
	if( *ptr == '\n' || *ptr == '\r' || !*ptr )
		return	byte;
	return	ERR;
}

static void
EncodeLine(unsigned char *str, unsigned char *ptr, int n, unsigned int chk)
{
	int	i, sum= 0;

	*str++= encmap[n];
	for( i= 0 ; i < n ; i+= 3 ){
		sum+= ptr[i] +ptr[i+1] +ptr[i+2];
		*str++=encmap[  ptr[i  ]>>2 ];
		*str++=encmap[((ptr[i  ]<<4)&0x30)|((ptr[i+1]>>4)&0x0f)];
		*str++=encmap[((ptr[i+1]<<2)&0x3c)|((ptr[i+2]>>6)&0x03)];
		*str++=encmap[( ptr[i+2]    &0x3f)];
	}
#ifdef UUCHKSUM
	if( chk )
		*str++= encmap[sum&0x3f];
#endif
	*str= '\0';
}

int
uudecode(T_HD *hp, unsigned char *ptr, unsigned int slen)
{
	int		byte;
	unsigned char	str[90];
	if(!ptr || !slen)
		return	FALSE;
	if((byte= DecodeLine(str, ptr, hp->uulength)) != ERR) {
		if(!byte) {
#if PREREAD
			unsigned char	*p= PreRead();
			if( *p == 'e' && p[1] == 'n' && p[2] == 'd' ){
				Merr( " (" );
				Numerr( hp->fsize );
				Merrch( ')' );
				p = PreRead();
				if( !p_strmatch( (char *)p, "size " ) ){
					if( hp->fsize != numstr( p+5 ) ){
						Merr( " size error" );
					}
				}
				Merr( "\r\n" );
				return	EOISH;
			}
			return	TRUE;
#else
			Merr( " (" );
			Numerr( hp->fsize );
			Merr( ")\r\n" );
			return	EOISH;
#endif
		}
		if( !hp->nextline-- ){
			Merrch( 'o' );
			hp->nextline= 100;
		}
		Mwrite( &fmo, (char *)str, hp->uulength = byte );
		hp->fsize+= byte;
	}else if( !(hp->mode & mUUENC) && *ptr == 'b' &&
				!p_strmatch( (char *)ptr, "begin " ) ){
/*		ai_decode_close();*/
		Merr( " -- error\r\n" );
/*		readuuhead( hp, ptr, slen );*/
		return	UURETRY;
	}
	return	TRUE;
}

#if BCB
#include <stdlib.h>
static void
dummy_use(unsigned char *ptr)
{
	/* 外部への影響はない */
	ptr = (unsigned char *)malloc(16);
	*ptr = *(ptr + 1) + *(ptr + 2);
	free(ptr);
}
#endif

int
uuencode(T_HD *hp)
{
	unsigned char	*ptr,
			str[256],
			buf[256];

	if( hp->initflag ){
		if( !Mopen( &fm, (char *)hp->fname ) ){
			Merr( "open error\r\n" );
			return ENCODE_CANT_OPEN_INFILE;
		}
		if( hp->stepline > 63 )
			hp->stepline= LBYTE;
		ptr= (unsigned char*)ishname( str, hp->fname );
		p_strcpy( hp->fname, str );
		if( hp->mode & mOUTPUT ){
			p_strcpy( str, hp->outname );
		}else{
			if( hp->mode & mAPPEND )
				for(; *ptr ; ptr++ );
			*ptr++ = '.';
			*ptr++ = 'U';
			*ptr++ = 'U';
			*ptr++ = 'E';
			*ptr++ = '\0';
			dummy_use(ptr); /* BCB の警告対策 */
		}
		Merr( (char *)str );
		if( !Mcreate( &fmo, (char *)str ) ){
			Mclose( &fm );
			Merr( " create error\r\n" );
			return ENCODE_CANT_CREATE_OUTFILE;
		}
#if UNIX
		if( Misfile( &fm ) ){
			sprintf( (char *)buf, "begin %03o ", fileperm( fm.fn ) );
			Mputs( &fmo, (char *)buf );
		}else{
			Mputs( &fmo, "begin 644 " );
			p_strcpy( hp->fname, (unsigned char *)"AishEnc" );
		}
#else
		if( !Misfile( &fm ) )
			p_strcpy( hp->fname, "AishEnc" );
		Mputs( &fmo, "begin 644 " );
#endif
		Mputs(&fmo, *hp->skipname ? (char *)hp->skipname : (char *)hp->fname);
		Mputs( &fmo, "\r\n" );
		hp->initflag= FALSE;
	}else{
		unsigned int	byte= Mread( &fm, buf, hp->stepline );
		EncodeLine( str, buf, byte, hp->mode & mUUCHK );
		hp->fsize+= byte;
		Mputs( &fmo, (char *)str );
		Mputs( &fmo, "\r\n" );
		if( byte == 0 ){
			Mputs( &fmo, "end\r\nsize " );
			strnum( buf, hp->fsize );
			Mputs( &fmo, (char *)buf );
			Mputs( &fmo, "\r\n" );
			Mflush( &fmo );
			Mclose( &fmo );
			Merr( "\r\n" );
			Mclose( &fm );
			return	EOISH;
		}
	}
	return ENCODE_SUCCESS;
}

#endif

