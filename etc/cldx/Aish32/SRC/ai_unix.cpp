/* Modified by H. Shimada. */

/* Copyright 1994 Hiroyuki Ogasawara (COR.) */

#include "ai_unix.h"

#include "config.h"

#include <cstring>
#include <ctime>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

unsigned char _sf_jis7[2];
unsigned char _sf_jis8[2];
unsigned char _sf_sjis[2];
unsigned char _sf_nksjis[2];


void *
memclr(size_t len, void *ptr)
{
#ifdef BZ_BSD
	aa
	return	bzero( ptr, len );
#else
	return	memset( ptr, '\0', len );
#endif
}

void
timecpy(unsigned int *dst, unsigned int *src)
{
	int	check = 1;

	if(*(char*)&check) {
		*dst = *src;
	}else{
		unsigned char
		  *d= ((unsigned char*)dst)+4,
		  *s= (unsigned char*)src;

		*--d= *s++;
		*--d= *s++;
		*--d= *s++;
		*--d= *s++;
#if BCB              /* BCB ÇÃåxçêëŒçÙ */
		s--;
		*s = *s;
#endif
	}
}

void
DATESTR(unsigned int date, char *str)
{
	int	year= (date & 0x0fff0000)>>16,
		month=(date & 0x0000ff00)>>8,
		mday= (date & 0x000000ff);

	sprintf( str, "%02d-%02d-%02d", year-1900, month, mday );
}

void
TIMESTR(unsigned int time, char *str)
{
	int	hour=  (time & 0x00ff0000)>>16,
		minute=(time & 0x0000ff00)>>8,
		second=(time & 0x000000ff);

	sprintf( str, "%02d:%02d:%02d", hour, minute, second );
}

off_t
filesize(int handle)
{
	struct stat st;
	fstat(handle, &st);
	return st.st_size;
}

unsigned int
filedate(int handle)
{
	struct stat	st;
	struct tm	*tp;
	fstat(handle, &st);
	tp= localtime(&st.st_mtime);

	return	((tp->tm_year+1900-1980)<<25)+
		((tp->tm_mon+1)<<21)+
		(tp->tm_mday<<16)+

		(tp->tm_hour<<11)+
		(tp->tm_min<<5)+
		(tp->tm_sec/2);

}

unsigned int
fileperm(int handle)
{
	struct stat	st;

	fstat(handle, &st);

	return	st.st_mode & 0777;
}

void
filesetdate(unsigned char *name, unsigned int date)
{
	static int	month[12]= {	0,
					31,
					31+28,
					31+28+31,
					31+28+31+30,
					31+28+31+30+31,
					31+28+31+30+31+30,
					31+28+31+30+31+30+31,
					31+28+31+30+31+30+31+31,
					31+28+31+30+31+30+31+31+30,
					31+28+31+30+31+30+31+31+30+31,
					31+28+31+30+31+30+31+31+30+31+30,
				/*	31+28+31+30+31+30+31+31+30+31+30+31*/};
	unsigned int	year=(date>>25)+1980-1970,
			mon= (date>>21)&15,	/* 1 Å` 12 */
			mday=(date>>16)&31,	/* 1 Å` 31 */
			hour=(date>>11)&31,	/* 0 Å` 23 */
			min= (date>>5)&63,	/* 0 Å` 59 */
			sec= (date&31)*2;	/* 0 Å` 58 */
	time_t		tim[2];

	mday= year*365 +((year+2)/4) -((year+70)/100) +((year+370)/400)
						+month[mon-1] +mday -1;
	if( (!(year+2)&3) && mon < 2 )
		mday--;
	tim[0]= tim[1]= mday*24*60*60 +hour*60*60 +min*60 +sec +_timezone;
	utime(name, (struct utimbuf *)tim);
}

#if CKNAME
unsigned char *
ckname( name )
unsigned char	*name;
{
	unsigned char	*p= name;
	static unsigned char	new[32];
	for(; *p ; p++ ){
		if( *p & 128 ){
			int	count= 0;
			do{
				sprintf( new, "aish_8bit_name.%03d", count++ );
			}while( access( new, 0 /*F_OK*/ ) >= 0 );
			Merr( " rename -> " );
			Merr( new );
			Merr( " " );
			return	new;
		}
	}
	return	name;
}
#endif

