/*	Copyright 1993,94,95 H.Ogasawara (COR.)	*/
#if !defined(WDN_CONFIG_H)
#define WDN_CONFIG_H

#define			CRC32		0
#define			DELCTRL		1
#define			ERRCONT		0
#define			PREREAD		1

/* - - - - - - - - - - - - - - - - - - - - - - - - */

#if UNIX && !BCB
#  define		SYSDOSLIB	0
#  define		HUMANFIO	0
#  define		SUPASM		0
#  define		CSUMASM		0
#  define		CRCASM		0
#  define		SJENCASM	0
#  define		SJDECASM	0
#  define		ERRCASM		0
#  define		ANSIC		0
#  define		CKNAME		1
#  define		PATHLEN		256
#  define		NAMELEN		256
#  define		INLINE
#  define		BROPEN		"r"
#  define		BWOPEN		"w"
#  define		BAOPEN		"a"
#  define		BRWOPEN		"r+"
#endif

#if BCB
#  define		SYSDOSLIB	0
#  define		HUMANFIO	0
#  define		SUPASM		0
#  define		CSUMASM		0
#  define		CRCASM		0
#  define		SJENCASM	0
#  define		SJDECASM	0
#  define		ERRCASM		0
#  define		ANSIC		0
#  define		CKNAME		0
#  define		PATHLEN		256
#  define		NAMELEN		256
#  define		INLINE
#  define		BROPEN		"rb"
#  define		BWOPEN		"wb"
#  define		BAOPEN		"ab"
#  define		BRWOPEN		"r+b"
#endif

#if HUMAN68K | HUMAN68K2
# if !HUMAN68K2
#   define		SYSDOSLIB	1
#   define		HUMANFIO	1
#   define		SUPASM		1
#   define		CSUMASM		1
#   define		CRCASM		1
#   define		SJENCASM	1
#   define		SJDECASM	1
#   define		ERRCASM		1
# else
#   define		SYSDOSLIB	0
#   define		HUMANFIO	0
#   define		SUPASM		0
#   define		CSUMASM		0
#   define		CRCASM		0
#   define		SJENCASM	0
#   define		SJDECASM	0
#   define		ERRCASM		0
# endif
# define		ANSIC		1
# define		CKNAME		0
# define		PATHLEN		80
# define		NAMELEN		24
# define		INLINE		inline
# define		BROPEN		"rb"
# define		BWOPEN		"wb"
# define		BAOPEN		"ab"
# define		BRWOPEN		"r+b"
# ifndef HUMAN68K
#  define		HUMAN68K	1
# endif
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - */

/* AISHMV32.DLL ÇÃç\ízéûÇÃÇ› MVOLENC == 1 Ç∆Ç»ÇÈ */
#if MVOLENC
# undef	CRC32
# undef	CRCASM
# define	CRC32	1
# define	CRCASM	0
#endif

#if MIMEDEC
# undef	PREREAD
# define	PREREAD	1
#endif


#endif /* config.h */
