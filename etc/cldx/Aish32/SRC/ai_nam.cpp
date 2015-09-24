/*	Copyright 1993,94 H.Ogasawara (COR.)	*/

#include "ai_nam.h"

#include "config.h"

#include "ai_s.h"  /* for sjis_chk[] */

#if PREREAD
unsigned char *
#else
void
#endif
p_strcpy(unsigned char *str, const unsigned char *ptr)
{
	while((int)(*str++ = *ptr++));
#if PREREAD
	return	str;
#endif
}

int
p_strcmp(const char *str, const char *ptr)
{
	for(; *str && *str == *ptr ; str++, ptr++ );
	return (int)(*str - *ptr);
}

#if UUENCODE
char
p_strmatch(const char *str, const char *ptr)
{
	for(; *ptr && *str == *ptr ; str++, ptr++ );

	return	*ptr;
}

int
num7str(unsigned char *ptr)
{
	int	i;
	int perm = 0;

	for( i= 0 ; i < 3 && *ptr >= '0' && *ptr <= '7' ; i++, ptr++ ){
		perm <<= 3;
		perm += *ptr & 7;
	}
	if( !i || *ptr )
		return	-1;

	return perm;
}
#endif

unsigned int
strnum(unsigned char *str, unsigned int i)
{
	unsigned char	buf[24], *ptr= buf+24;
	*--ptr= '\0';
	do{
		*--ptr = (unsigned char)(i % 10 + '0');
	}while( i/= 10 );
	for(; (int)(*str++ = *ptr++); i++);
	return	i;
}

unsigned int
numstr(unsigned char *ptr)
{
	unsigned int i = 0;

	for(; *ptr; ptr++) {
		i *= 10;
		i += *ptr & 15;
	}

	return	i;
}

unsigned char *
ishname(unsigned char *str, unsigned char *ptr)
{
	unsigned int	kanji= 0;
	unsigned char	*lastpos;
	for( lastpos= ptr ; *ptr ; ptr++ ){
		if( kanji ){
			kanji= 0;
		}else{
			kanji= sjis_chk[*ptr];
			if( *ptr == '/' || *ptr == '\\' || *ptr == ':' )
				lastpos= ptr+1;
		}
	}
	for( ptr= 0 ; *lastpos ; *str++= *lastpos++ ){
		if( *lastpos == '.' )
			ptr= str;
	}
	*str= '\0';
	return	ptr ? ptr : str;
}

