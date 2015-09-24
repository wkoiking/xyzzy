/*	Copyright 1993 H.Ogasawara (COR.)	*/

#include "ai.h"
#include "microfio.h"

#include "ai_hed.h"

#include "aish.h"
#include "ai_7.h"
#include "ai_8.h"
#include "ai_s.h"
#include "ai_n.h"
#include "ai_unix.h"
#include "ai_fio.h"
#include "ai_nam.h"
#include "ai_crc.h"

static unsigned char	ishbit2z[]= {
	0,0,0,0,0,0,0,0,zJIS8,0,0,0,0,zJIS7,zSJISN,zSJIS };

T_Z	ztype[]= {
	{ decline_7, encline_7, _sf_jis7,  {'j','i','s','7',0}, 63, 13, 0x9d },
	{ decline_8, encline_8, _sf_jis8,  {'j','i','s','8',0}, 69,  8, 0x1a },
	{ decline_s, encline_s, _sf_sjis,  {'s','j','i','s',0}, 73, 15, 0x04 },
	{ decline_n, encline_n, _sf_nksjis,{'n','k','-','s','j','i','s',0}, 69, 14, 0x1a }
};


#if UUENCODE

#if BCB
#include <stdlib.h>
static int
dummy_use(unsigned int slen)
{
	return slen;
}
#endif /* BCB */

unsigned int
readuuhead(T_HD *hp, unsigned char *ptr, unsigned int slen)
{
	unsigned int mode= hp->mode;

#if BCB
	dummy_use(slen);               /* BCB ‚ÌŒx‘Îô */
#endif

	memclr(sizeof(T_HD)-NAMES, hp);
	hp->mode= mode;
	ptr = getword(hp->fname, ptr);
	if(*ptr) {
		ptr = getword(hp->fname, ptr);
		hp->perm = num7str(hp->fname);
		if(hp->perm >= 0 && *ptr) {
			ptr = getword(hp->fname, ptr);
#if BCB
			dummy_use((unsigned int)ptr);
#endif
			if(!(hp->mode & mLIST)){
				Merr((char *)hp->fname);
				Merrch(' ');
			}else{
				Mprint((char *)hp->fname);
				Mprint(" line:");
			}
			hp->ztype = zUUE;
			hp->uulength = -1;
			hp->fsize = 0;
			hp->nextline = 0;

			return TRUE;
		}
	}

	return	FALSE;
}
#endif

void
Numerr(unsigned int i)
{
	char buf[20];

	strnum((unsigned char *)buf, i);
	Merr(buf);
}

void
Numput(unsigned int i)
{
	char buf[20];

	strnum((unsigned char *)buf, i);
	Mprint(buf);
}

static unsigned int
readhead(T_HD *hp, unsigned char *ptr, unsigned int slen)
{
	struct headline	hd;
	unsigned int len
	  = (*ptr == '!') ? decline_7((unsigned char *)&hd, ptr, slen) :
						decline_8((unsigned char *)&hd, ptr, slen);
	/* ã‚Ì‚¿‚å‚Á‚ÆŠë‚È‚¢‚¼ */

	if(len > 60 && crc_check(0xffff, (unsigned char *)&hd, len) == 0x1d0f) {
		unsigned int	i;
		unsigned char	*s, *p;
/*		memclr( sizeof(T_HD), hp );*/
		timecpy(&hp->fsize, (unsigned int *)hd.fs); /* Šë‚È‚¢‚©‚à WDN */
		if((int)(hp->vol = hd.volume)) {
			timecpy(&hp->volsize, (unsigned int *)hd.vbyte); /* Šë‚©‚à WDN */
			hp->fsize= (hp->volsize*hd.volume > hp->fsize ?
				hp->fsize-(hp->volsize*(hd.volume-1)) : hp->volsize);
		}
		hp->byte= hd.line[0]+(hd.line[1]<<8);
		hp->tcrc= (hd.crc16[0]<<8)+hd.crc16[1];
		hp->body= hp->byte-3;
		hp->blockbyte= hp->body*hp->body;
		hp->date= hp->perm= 0;
		if( hd.flag & 1 )
			timecpy(&hp->date, (unsigned int *)hd.date); /* Šë‚©‚à WDN */
		hp->initflag= TRUE;
		hp->exec= ztype[hp->ztype= ishbit2z[hd.ishbit]].decexec;
		hp->block= hp->fsize/hp->blockbyte +1;
		hp->fsize+= 2;
		hp->dline= hp->fsize/hp->body+1;
		for( i= 0, s= hp->fname, p= hd.node ; *p != ' ' && i<8 ; i++ )
			*s++= *p++;
		if( *hd.ext != ' ' )
			for( *s++= '.', i= 0, p= hd.ext ; *p!=' '&&i<3 ; i++ )
				*s++= *p++;
		*s= '\0';

		if( !(hp->mode & mADJNAME) ){
			unsigned int	ic;
			for( ic= 0, s= hp->fname, p= hp->adjname ;
				*s && *s == *p && ic<8 ; s++, p++, ic++ );
			if( (!*s || *s == '.' || ic == 8) && *p )
				p_strcpy( hp->fname, hp->adjname );
		}

	/* debug */
		{
			if( !(hp->mode & mLIST) ){
				Merr( (char *)hp->fname );
				Merrch( '(' );
				Numerr( hp->fsize-2 );
				if( hd.volume ){
					Merrch( '/' );
					Numerr( hd.volume );
				}
				Merrch( ')' );
			}else{
				char	buf[28];
				Mprint( (char *)hp->fname );
				Mputchar( ' ' );
				Mputchar( '(' );
				Numput( hp->fsize-2 );
				if( hd.volume ){
					Mputchar( '/' );
					Numput( hd.volume );
				}
				Mputchar( ')' );
				Mputchar( ' ' );
				DATESTR(
					((hp->date & 0xfe000000)>>9)+
					((hp->date & 0x01e00000)>>13)+
					((hp->date & 0x001f0000)>>16)+
					0x27bc0000
						, buf );
				Mprint( buf );
				Mputchar( ' ' );
				TIMESTR(
					((hp->date & 0x0000f800)<<5)+
					((hp->date & 0x000007e0)<<3)+
					((hp->date<<1) & 0x3e)
						, buf );
				Mprint( buf );
				Mputchar( ' ' );
				Mprint( (char *)ztype[hp->ztype].name );
				Mprint( " os:" );
				Numput( hd.os );
#if 0
				if( hd.flag & 12 ){
					unsigned	i,j;
					i= (hd.crc16[0]<<8)+hd.crc16[1];
					timecpy( &j, hd.crc32 );
					if( filecrc( hp->fname, i, j ) ){
						Mprint( "<crcOK!>" );
					}
				}
#endif
				Mprint( " line:" );
			}
		}
		return	TRUE;
	}
	return	FALSE;
}


unsigned int
checkhead(T_HD *hp, unsigned char *ptr, unsigned int slen)
{
	if( (*ptr == '!' || *ptr == '#' ) && slen >= 76 && slen <= 81 ){
		return	readhead( hp, ptr, slen );
#if UUENCODE
	}else if( !(hp->mode & mUUENC) &&
				*ptr == 'b' && !p_strmatch( (char *)ptr, "begin " ) ){
		return	readuuhead( hp, ptr, slen );
#endif
	}else if( *ptr == '<' && ptr[1] == '<' && ptr[2] == '<' &&
		ptr[3] == ' ' && slen >= 18 ){
		unsigned char	*str= hp->adjname;
		int	i= 30;
		for( ptr+= 4 ; *ptr && *ptr != ' ' && i-- ; *str++= *ptr++ );
		*str= '\0';
	}
	return	FALSE;
}

void
writehead(T_HD *hp, struct headline *hd)
{
	unsigned char	*s, *p, *ptr, buf[30];
	unsigned int	i, extflag= FALSE;
	memclr( sizeof(struct headline), hd );
	if((int)(hp->vol)) {
		unsigned int mv = (hp->os >> 8) & 0xff;

		timecpy((unsigned int *)hd->fs, &hp->volsize); /* Šë‚È‚¢‚©‚à WDN */
		if(hp->vol > 1 && hp->vol == mv ){
			mv = (hp->volsize-hp->fsize) / (hp->vol-1);
			timecpy((unsigned int *)hd->vbyte, &mv); /* Šë‚È‚¢‚©‚à WDN */
		}else
			timecpy((unsigned int *)hd->vbyte, &hp->fsize); /* Šë‚È‚©‚à WDN */
		timecpy((unsigned int *)hd->crc32, &hp->tcrc32); /* Šë‚È‚¢‚©‚à WDN */
		hd->crc16[0] = (unsigned char)(hp->tcrc >> 8);
		hd->crc16[1] = hp->tcrc;
		hd->flag= 13;	/* use:time stamp,crc16,crc32 */
	}else{
		timecpy((unsigned int *)hd->fs, &hp->fsize); /* Šë‚È‚¢‚©‚à WDN */
		hd->flag= 1;	/* use:time stamp */
	}
	hd->line[0]= (unsigned char)hp->byte;
	hd->line[1]= (unsigned char)(hp->byte >> 8);
	hd->block[0]= (unsigned char)(hp->byte - 1);
	hd->block[1]= (unsigned char)((hp->byte-1) >> 8);
	hd->ishbit= ztype[hp->ztype].code;
	hd->orgbit= (hp->ztype == zJIS8) ? (unsigned char)8 : (unsigned char)16;
	ptr= (unsigned char*)ishname( buf, hp->fname );
	if( *ptr == '.' )
		extflag= TRUE;
	for( *ptr++='\0', s=hd->node, p=buf,i=8 ; *p && i ; i--, *s++= *p++ );
	for(; i-- ; *s++= ' ' );
	i= 3;
	if( extflag )
		for(; *ptr && i ; i--, *s++= *ptr++ );
	for(; i-- ; *s++= ' ' );
	hd->os= hp->os;
	hd->volume= (unsigned char)hp->vol;
	hd->exttype= 1;	/* 1:new_style */
	timecpy((unsigned int *)hd->date, &hp->date);
}
