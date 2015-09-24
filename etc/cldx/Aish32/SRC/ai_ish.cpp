/* Modified for DLL by H. Shimada. */

/*	Copyright 1993,94 H.Ogasawara (COR.)	*/

#include "ai.h"
#include "microfio.h"

#include "ai_fio.h"
#include "ai_nam.h"
#include "ai_fcrc.h"
#include "ai_unix.h"
#include "ai_hed.h"
#include "ai_blk.h"

#include "aisherr.h"

m_file fm;
m_file fmo;
T_HD hp;

char *
getfname(char *name)
{
	static char	newbuf[NAMELEN];

	if( (hp.mode & mOWRITE) && Maccess( name ) ){
		int	count= 0;
		char	*s, *p, *e;
		do{
			if( hp.mode & mFHEAD ){
				s= newbuf;
				*s++= (char)(count / 10 + '0');
				*s++= (char)(count++ % 10 + '0');
				p_strcpy( (unsigned char *)s, (unsigned char *)name );
			}else{
				for(e= 0,s= newbuf, p= name ; *p ; *s++= *p++){
					if( *p == '.' )
						e= s;
				}
				if( !(hp.mode & mAPPEND) && e )
					s= e;
				*s++= '.';
				*s++= (char)(count / 10 + '0');
				*s++= (char)(count++ % 10 + '0');
				*s= '\0';
			}
		}while( Maccess( newbuf ) );
		Merr( " -> " );
		Merr( newbuf );
		Merr( " " );
		return	newbuf;
	}

	return	name;
}


#if 0
void
ai_encode_init( mode, ptr, name, title, os, exfile, tsize, crc16, crc32 )
unsigned char	*ptr, *name, *exfile;
unsigned int	title, os, tsize;
{
	static unsigned char	codes[]= { CODES, CODE7, CODE8, CODEN, UUE };
	memclr( sizeof(T_HD), &hp );
	p_strcpy( hp.fname, ptr );
	p_strcpy( hp.outname, name );
	p_strcpy( hp.skipname, exfile );
	hp.code= codes[mode & 31];
	hp.mode= mode>>8;
	hp.initflag= TRUE;
	hp.stepline= title;
	hp.os= os;
	hp.vol= os>>16;
	hp.volsize= tsize;
	hp.tcrc= crc16;
	hp.tcrc32= crc32;
}

ai_encode()
{
	return	encblock(&hp);
}
#endif

void
ai_decode_close(void)
{
	Mflush( &fmo );
	if( Misfile( &fmo ) ){
		Mclose( &fmo );
		if( !(hp.mode & mOUTPUT) && hp.date )
			filesetdate( hp.fname, hp.date );
	}
	if( !(hp.mode & mOUTPUT) && hp.vol && hp.tcrc ){
		Merr( " -- multi volume " );
		if( filecrc( (char *)hp.fname, hp.tcrc ) ){
			Merr( (char *)hp.fname );
			Merr( " finished!" );
		}
		Merr( "\r\n" );
	}
}

#if 0
ai_decode_init( mode, name, exfile )
{
	hp.mode= mode;
	p_strcpy( hp.outname, name );
	p_strcpy( hp.skipname, exfile );
}
#endif

unsigned int
ai_decode(unsigned char *ptr, unsigned int len)
{
	static unsigned	int	nodec = TRUE;

	if(nodec){
_uuretry:
		if( checkhead( &hp, ptr, len ) ){
			unsigned char	*name = (hp.mode & mOUTPUT) ?
#if CKNAME
				hp.outname : (unsigned char*)ckname(hp.fname);
#else
						hp.outname : hp.fname ;
#endif
			if( (hp.mode & mSKIP) &&
					p_strcmp( (char *)hp.fname, (char *)hp.skipname ) ){
				Merr( " skip\r\n" );
				return	nodec;
			}
			if( hp.vol ){
				if((int)(nodec = !Mappend( &fmo, (char *)name )))
					Merr( "append error\r\n" );
				else
					Mseek( &fmo, hp.volsize*(hp.vol-1), 0 );
			}else{
				if((int)(nodec= !Mcreate( &fmo, (char *)name )))
					Merr( "create error\r\n" );
			}
		}
	}else{
#if ERRCONT
		switch( decblock( &hp, ptr, len ) ){
		case FALSE:
		case EOISH:
			ai_decode_close();
			nodec= TRUE;
			break;
		case ESKIP:
			if( checkhead( &hp, ptr, len ) ){
				ai_decode_close();
				nodec= TRUE;
				return	ai_decode( ptr, len );
			}
			break;
		case UURETRY:
			ai_decode_close();
			nodec= TRUE;
			goto	_uuretry;
		}
#else
	    {
		int	ret;
		if( (ret= decblock( &hp, ptr, len )) != TRUE ){
			ai_decode_close();
			nodec= TRUE;
			if( ret == UURETRY )
				goto	_uuretry;
		}
	    }
#endif
	}
	return	nodec;
}

void
ai_decode_lastchance(void)
{
	hp.initflag= 0xff;
	if( decblock( &hp, 0, 0 ) != EOISH )
		Merr( " eof error\r\n" );
	ai_decode_close();
}
