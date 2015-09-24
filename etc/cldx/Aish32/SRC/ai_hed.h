#if !defined(WDN_AI_HED_H)
#define WDN_AI_HED_H

struct headline {
	unsigned char	sequence,	/* 0 only */
			volume,		/* 0:single volume, 1..250:volume */
			fs[4],		/* file size */
			line[2],	/* bytes of line */
			block[2],	/* lines in block */
			orgbit,		/* 8:jis8,16:jis7|sjis|sjisn */
			ishbit,		/* 8:jis8,13:jis7,15:sjis,14,sjisn */
			node[8], ext[3], /* file name (MS-DOS format) */
			janle,		/* zero */
			os,		/* os type */
			exttype,	/* 0:old_style 1:check sum */
/* 0:time stamp, 1:difference time, 2:CRC16, 3:CRC32, 4:CRCvol16 */
			flag,		/* bit flag */
			date[4],	/* time & date (MS-DOS format) */
			tsec[3],	/* difference time */
			crc16[2],	/* total crc16 */
			crc32[4],	/* total crc32 */
/* end of volume check */
			vbyte[4],	/* volume size (byte) */
			vcrc16[2],	/* volume crc16 */
			reserve[15],
			dummy[17];	/* jis8 headder */
};

extern T_Z ztype[];

void writehead(T_HD *hp, struct headline *hd);
unsigned int checkhead(T_HD *hp, unsigned char *ptr, unsigned int slen);
void Numput(unsigned int i);
void Numerr(unsigned int i);

#endif
