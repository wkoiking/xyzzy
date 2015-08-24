/* Modified for DLL by H. Shimada. */

/* Copyright 1994 H.Ogasawara (COR.) */
#include "ai_fcrc.h"

#include "config.h"
#include "microfio.h"

#include "ai_fio.h"
#include "ai_ish.h"
#include "ai_crc.h"

#define		BUFSIZE	(1024*4)

/* !.. AISHMV32.DLL ç\ízéûÇ…ÇÃÇ› CRC32 É}ÉNÉçÇÕ 1 Ç∆Ç»ÇÈ */

#if CRC32
int
filecrc(char *name, unsigned short c16, unsigned int c32)
#else
int
filecrc(char *name, unsigned short c16)
#endif
{
	unsigned char	buf[BUFSIZE];
	unsigned short	crc16= 0xffff;
#if CRC32
	unsigned int	crc32= 0xffffffff;
#endif
	if( Mopen( &fmo, name ) ){
		int	size;
		do{
			size= Mread( &fmo, buf, BUFSIZE );
			crc16= (unsigned short)crc_check( crc16, buf, size );
#if CRC32
			crc32= (unsigned int)crc_check32( crc32, buf, size );
#endif
		}while( size == BUFSIZE );
		Mclose( &fmo );
	}else{
		Merr( "file not found\r\n" );
		return	0;
	}
#if CRC32
#  if ANSIC
	if( c16 != (unsigned short)~crc16 || c32 != ~crc32 )
#  else
	if( c16 != ((~crc16)&0xffff) || c32 != ~crc32 )
#  endif
#else
#  if ANSIC
	if( c16 != (unsigned short)~crc16 )
#  else
	if( c16 != ((~crc16)&0xffff) )
#  endif
#endif
		return	0;
	return	1;
}
