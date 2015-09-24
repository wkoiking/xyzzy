/* Modified for DLL by H. Shimada. */

/* Copyrigth 1993 H.Ogasawara (COR.) */

#include "ai_fio.h"

#include <cstring>
#include "config.h"

#include "ai_ish.h"

using namespace std;

#if DELCTRL
static unsigned char	ctrlmap[]={
	1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1
};
#endif

/* �@stdout�Astderr �ɏo�͂���Ă����f�[�^�� DLL �ł͎��̃o�b�t�@�Ɋi�[���A
 * �Ǘ�����B
 */
char *Gstdout_buf;
int Gstdout_buf_pos;
unsigned long Gstdout_buf_size;


#if HUMANFIO
/* ============================================= */

#if SYSDOSLIB
#  include	<sys_doslib.h>
#else
#  include	<doslib.h>
#endif
#include	"microfio.h"

/* !.. MVOLENC �}�N���́AAISHMV32.DLL �\�z���� 1 �ɂȂ� */
#if MVOLENC
Merr( msg )
char	*msg;
{
	FPUTS( msg, 2 );
}
#endif

Mopen( fm, name )
m_file	*fm;
unsigned char	*name;
{
	if( *name == '-' && !name[1] ){
		fm->fn= 0;
	}else if( (fm->fn= OPEN( name, 0 )) < 0 )
		return	0;
	fm->limit= 0;
	return	1;
}

Mcreate( fm, name )
m_file	*fm;
unsigned char	*name;
{
	if( *name == '-' && !name[1] ){
		fm->fn= 1;
#if !MVOLENC
	}else if( (fm->fn= CREATE( (void*)getfname(name), 0x20 )) < 0 )
#else
	}else if( (fm->fn= CREATE( name, 0x20 )) < 0 )
#endif
		return	0;
	fm->ptr= fm->buf;
	fm->limit= fm->buf+MFBUFSIZE;
	return	1;
}

#if !MVOLENC
Mappend( fm, name )
m_file	*fm;
unsigned char	*name;
{
	if( CHMOD( name, -1 ) < 0 )
		return	Mcreate( fm, name );
	if( (fm->fn= OPEN( name, 1 )) < 0 )
		return	0;
/*	SEEK( fm->fn, 0, 2 );*/
	fm->ptr= fm->buf;
	fm->limit= fm->buf+MFBUFSIZE;
	return	1;
}

Mseek( fm, pos, mode )
m_file	*fm;
{
	int	p= SEEK( fm->fn, pos, mode );
	if( p < 0 ){
		p= SEEK( fm->fn, 0, 2 );
		for(; pos > p+MFBUFSIZE ; p+= MFBUFSIZE )
			WRITE( fm->fn, fm, MFBUFSIZE );
		WRITE( fm->fn, fm, pos-p );
	}
}
#endif	/* MVOLENC */

Mclose( fm )
m_file	*fm;
{
	if( fm->fn >= 3 )
		return	CLOSE( fm->fn );
}

Mread( fm, buf, len )
m_file	*fm;
unsigned char	*buf;
unsigned int	len;
{
	unsigned char	*str= buf,
			*ptr;
	for( ptr= fm->ptr ; len-- ;){
		if( ptr >= fm->limit ){
			int	size= READ( fm->fn, ptr= fm->buf, MFBUFSIZE );
			if( size <= 0 ){
				fm->limit= 0;
				break;
			}
			fm->limit= fm->buf+ size;
		}
		*str++= *ptr++;
	}
	fm->ptr= ptr;
	return	str-buf;
}

#if !MVOLENC
Mgets( fm, buf, len )
m_file	*fm;
unsigned char	*buf;
unsigned int	len;
{
	unsigned char	*str= buf,
			*ptr;
	for( ptr= fm->ptr ; len-- ; ptr++ ){
		if( ptr >= fm->limit ){
			int	size= READ( fm->fn, ptr= fm->buf, MFBUFSIZE );
			if( size <= 0 )
				return	0;
			fm->limit= fm->buf+ size;
		}
		if( *ptr == '\n' ){
			ptr++;
			break;
		}
#if DELCTRL
		if( !ctrlmap[*ptr] )
#else
		if( *ptr != '\r' )
#endif
			*str++= *ptr;
	}
	*str= '\0';
	fm->ptr= ptr;
	return	str-buf+1;
}
#endif /* MVOLENC */

Mwrite( fm, ptr, len )
m_file	*fm;
unsigned char	*ptr;
{
	unsigned char	*str;
	int		ret= 1;
	for( str= fm->ptr ; len-- ;){
		if( str >= fm->limit )
			ret= WRITE( fm->fn, str= fm->buf,MFBUFSIZE)>=MFBUFSIZE;
		*str++= *ptr++;
	}
	fm->ptr= str;
	return	ret;
}

#if !MVOLENC
Mputs( fm, ptr )
m_file	*fm;
unsigned char	*ptr;
{
	unsigned char	*str;
	int		ret= 1;
	for( str= fm->ptr ; *ptr ;){
		if( str >= fm->limit )
			ret= WRITE( fm->fn, str= fm->buf,MFBUFSIZE)>=MFBUFSIZE;
		*str++= *ptr++;
	}
	fm->ptr= str;
	return	ret;
}

Misfile( fm )
m_file	*fm;
{
	return	fm->fn >= 3;	/* 5 */
}
#endif /* MVOLENC */

Mflush( fm )
m_file	*fm;
{
	if( fm->ptr > fm->buf ){
		WRITE( fm->fn, fm->buf, fm->ptr - fm->buf );
		fm->ptr= fm->buf;
	}
}

#if !MVOLENC
Maccess( name )
char	*name;
{
	return	CHMOD( name, -1 ) >= 0;
}
#endif


#else	/* HUMANFIO */
/* ============================================= */
/* Normal - file I/O rourtine */
#include "microfio.h"
#include <io.h>

#if NOERR
int	noerrflag = 0;
#endif

void
Mputchar(char ch)
{
	if(Gstdout_buf_pos - 3 < (long)Gstdout_buf_size) {
		if(ch != '\r') {
			Gstdout_buf[Gstdout_buf_pos] = ch;
			++Gstdout_buf_pos;
			Gstdout_buf[Gstdout_buf_pos] = '\0';
		}
	}
}

void
Merrch(char ch)
{
	if(Gstdout_buf_pos - 3 < (long)Gstdout_buf_size) {
		if(ch != '\r') {
			Gstdout_buf[Gstdout_buf_pos] = ch;
			++Gstdout_buf_pos;
			Gstdout_buf[Gstdout_buf_pos] = '\0';
		}
	}
}

void
Mprint(const char *ptr)
{
	for(; *ptr ; Mputchar( *ptr++ ) );
}

void
Merr(char *ptr)
{
	while(*ptr) {
		Merrch(*ptr++);
	}
}

int
Mopen(m_file *fm, char *name)
{
	if( *name == '-' && !name[1] ){
		fm->fp = stdin;
		fm->fn = fileno(stdin);
		return	1;
	}else if((fm->fp = fopen(name, BROPEN)) != NULL){
		fm->fn = fileno(fm->fp);
		return	1;
	}
	return	0;
}

int
Mcreate(m_file *fm, char *name)
{
	if( *name == '-' && !name[1] ){
		fm->fp= stdout;
		fm->fn= fileno( stdout );
		return	1;
#if !MVOLENC
	}else if((fm->fp = fopen(getfname(name), BWOPEN)) != NULL) {
#else
	}else if((fm->fp = fopen(name, BWOPEN)) != NULL) {
#endif
		fm->fn= fileno( fm->fp );
		return	1;
	}
	return	0;
}

int
Mappend(m_file *fm, char *name)
{
	if(access( name, 0 /*F_OK*/) < 0 )
		return Mcreate(fm, name);

	if((fm->fp = fopen( name, BRWOPEN)) != NULL ){
		fm->fn = fileno( fm->fp );

		return 1;
	}

	return 0;
}

int
Mclose(m_file *fm)
{
	if(fm->fp != stdin && fm->fp != stdout)
		return fclose(fm->fp);
	else
		return EOF;
}

size_t
Mread(m_file *fm, void *buf, size_t len)
{
	return fread(buf, 1, len, fm->fp);
}

static int
dummy_use(unsigned int a)
{
	return a;
}

#define	RDBUFSIZE	1024
size_t
Mgets(m_file *fm, unsigned char *buf, unsigned int len)
{
	unsigned char readbuf[RDBUFSIZE];

	dummy_use(len); /* BCB �̌x���΍� */
	if(fgets((char *)readbuf, RDBUFSIZE, fm->fp)) {
		unsigned char *str = buf,

		*ptr = readbuf;
		for(; *ptr && *ptr != '\r' && *ptr != '\n'; ptr++) {
#if DELCTRL
			if(!ctrlmap[*ptr])
#endif
				*str++ = *ptr;
		}
		*str = '\0';
		return	strlen((char *)buf) + 1;
	}
	return	0;
}

size_t
Mwrite(m_file *fm, char *ptr, size_t len)
{
	return	fwrite( ptr, 1, len, fm->fp ) >= len;
}

int
Mputs(m_file *fm, char *ptr)
{
	if(fm->fp == stdout) {
		while(*ptr) {
			Mputchar(*ptr);
			++ptr;
		}
	} else if(fm->fp == stderr) {
		while(*ptr) {
			Merrch(*ptr);
			++ptr;
		}
	} else {
		fputs( ptr, fm->fp );
	}

	return 1;
}

void
Mflush(m_file *fm)
{
	fflush( fm->fp );
}

void
Mseek(m_file *fm, long pos, int mode)
{
	fseek(fm->fp, pos, mode);
}

int
Misfile(m_file *fm)
{
	return	fm->fn >= 3;	/* tenuki */
}

#if !MVOLENC
int
Maccess(const char *name)
{
	return !access(name, 0);
}
#endif

#endif	/* !HUMANFIO */
