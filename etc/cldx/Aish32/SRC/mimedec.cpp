/*	1995 H.Ogasawara (COR.)		*/

#if MIMEDEC

#include "mimedec.h"

#include "ai.h"
#include "microfio.h"

#include "ai_fio.h"
#include "ai_nam.h"

#define		CHARSET		"ISO-2022-JP"
#define		ENCODING	"Base64"
#define		ENCODINGCH	'B'

#define		IOBUF		256
#define		STRBUF		256

#define		PutLine(a)	Mprint( a )
#define		PutChar(a)	Mputchar( a )

#define		cMULTIPART	0x10

static char	content_name[30],
		content_type[30],
		encoding_name[30],
		charset[30]= CHARSET;
static	int	TmpNumber= 0;

static unsigned char	ibuf[IOBUF],
			obuf[IOBUF];

/*
static unsigned char	pade[]= {
	'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
};
*/
static unsigned char	padd[]= {
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,62, 0, 0, 0,63,
	52,53,54,55,56,57,58,59,60,61, 0, 0, 0,99, 0, 0,
	 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
	15,16,17,18,19,20,21,22,23,24,25, 0, 0, 0, 0, 0,
	 0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


static unsigned char *
decbenc(unsigned char *str, unsigned char *ptr)
{
	for(; *ptr && *ptr != '\n' && *ptr != '\r' ; ptr+= 4 ){
		unsigned int	i= (padd[*ptr]<<18)+(padd[ptr[1]]<<12)+
					(padd[ptr[2]]<<6)+(padd[ptr[3]]);
		*str++ = (unsigned char)((i >> 16) & 255);
		if( ptr[2] != '=' )
			*str++ = (unsigned char)((i >> 8) & 255);
		if( ptr[3] != '=' )
			*str++ = (unsigned char)(i & 255);
	}
	return	str;
}

static int
isalnum(unsigned char ch)
{
	return	(ch >= '0' && ch <= '9')||
		(ch >= 'A' && ch <= 'Z')||
		(ch >= 'a' && ch <= 'z');
}

static int
strcmpi(unsigned char *str, unsigned char *ptr)
{
	for(; *str && (*str | 0x20) == (*ptr | 0x20); str++, ptr++)
		;
	return (*str | 0x20) - (*ptr | 0x20);
}

static unsigned char *
get_tkn(unsigned char *str, unsigned char *ptr, unsigned char ch)
{
	for(; *ptr && *ptr != ch ;)
		*str++= *ptr++;
	*str= '\0';
	if( *ptr == ch )
		ptr++;

	return	ptr;
}

static unsigned char *
get_word(unsigned char *str, unsigned char *ptr)
{
	for(; *ptr && !isalnum( *ptr ) ; ptr++ );
	for(; *ptr && isalnum(*ptr) ;)
		*str++= *ptr++;
	*str= '\0';
	return	ptr;
}

static void
content_init(void)
{
	*content_type= '\0';
	*content_name= '\0';
	*encoding_name= '\0';
}

static void
content_decode(unsigned char *ptr)
{
	unsigned char	buf[STRBUF];

	content_init();
	ptr = get_word((unsigned char *)content_type, ptr);
	if(*ptr == '/')
		ptr = get_word((unsigned char *)content_name, ptr+1);
	ptr = get_word(buf, ptr);
	if(*buf) {
		if(!strcmpi( buf, (unsigned char *)"charset")) {
			if(*ptr == '=')
				ptr = get_tkn((unsigned char *)charset, ptr+1, '"');
#if BCB
				*ptr = *ptr;  /* BCB ‚ÌŒx‘Îô */
#endif
		}
	}
}

void
line_decode(unsigned char *ptr)
{
	unsigned char	chset[STRBUF],
			encode[STRBUF];

	for(; *ptr ; ptr++ ){
		if( *ptr == '=' && ptr[1] == '?' ){
			unsigned char	*p= get_tkn( chset,  ptr+2, '?' );
			if( *p && !strcmpi( chset, (unsigned char *)charset ) ){
				ptr= get_tkn( encode, p, '?' );
				ptr= get_tkn( ibuf, ptr, '?' );
				p= decbenc( obuf, ibuf );
				*p= '\0';
				PutLine( (char *)obuf );
			}
		}else
			PutChar( *ptr );
	}
}

static void
skip_header(m_file *fm)
{
	for(; Mgets( fm, ibuf, IOBUF ) ;){
		PutLine( (char *)ibuf );
		PutLine( "\r\n" );
		if( !*ibuf )
			return;
	}
}

static void
bin_decode(m_file *fm)
{
	unsigned char	name[STRBUF], *s;
	m_file	fo;
	skip_header( fm );
	s = p_strcpy( name, (unsigned char *)content_type );
	s[-1]= '_';
	s = p_strcpy( s, (unsigned char *)content_name );
	s[-1]= '.';
	strnum( s, TmpNumber++ );
	if( Mcreate( &fo, (char *)name ) ){
		for(; Mgets( fm, ibuf, IOBUF ) ;){
			if( *ibuf != '-' || ibuf[1] != '-' ){
				unsigned char	*p= decbenc( obuf, ibuf );
				Mwrite( &fo, (char *)obuf, p-obuf );
			}else{
				PutLine( (char *)ibuf );
				PutLine( "\r\n" );
				break;
			}
		}
		Mflush( &fo );
		Mclose( &fo );
	}
}

void
MimeDec(m_file *fm)
{
	content_init();
	for(; Mgets( fm, ibuf, IOBUF ) ;){
		if( !p_strmatch( (char *)ibuf, "Content-Type: " ) ){
			PutLine( (char *)ibuf );
			PutLine( "\r\n" );
			content_decode( ibuf+14 );
		}else if( !p_strmatch( (char *)ibuf, "Content-Transfer-Encoding: " ) ){
			PutLine( (char *)ibuf );
			PutLine( "\r\n" );
			get_word( (unsigned char *)encoding_name, ibuf+27 );
			/*encoding_decode( ibuf+27 );*/
			if( !strcmpi( (unsigned char *)encoding_name, (unsigned char *)ENCODING ) )
				bin_decode( fm );
		}else{
			line_decode( ibuf );
			PutLine( "\r\n" );
		}
	}
}

#endif
