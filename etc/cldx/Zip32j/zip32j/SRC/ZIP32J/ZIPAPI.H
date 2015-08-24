/*
	zipapi.h

  		by Yoshioka Tsuneo(QWF00133@niftyserve.or.jp)
		welcome any e-mail!!
		You can use this file as Public Domain Software.
		Copy,Edit,Re-distibute and for any purpose,you can use this file.

*/
#ifndef _ZIPAPI_H
#define _ZIPAPI_H

typedef unsigned char uch;
typedef unsigned long ulg;
// #define PATH_MAX 260

#include <wtypes.h>

/* definitions from API.H at zip source */
typedef struct {
	int  argc;              /* Count of files to zip */
	LPSTR lpszZipFN;        /* name of archive to create/update */
	char **FNV;             /* array of file names to zip up */
} ZCL, *LPZCL;
typedef int (WINAPI DLLPRNT) (LPSTR, unsigned long);
typedef int (WINAPI DLLPASSWORD) (LPSTR, int, const LPSTR, const LPSTR);
typedef LPSTR (WINAPI DLLCOMMENT)(LPSTR);
typedef int (WINAPI DLLSERVICE) (LPCSTR, unsigned long);
typedef struct {
	DLLPRNT *print;
	DLLCOMMENT *comment;
	DLLPASSWORD *password;
	DLLSERVICE *ServiceApplication;
} ZIPUSERFUNCTIONS, far * LPZIPUSERFUNCTIONS;
/* intended to be a private struct: */
typedef struct _zip_ver {
    uch major;              /* e.g., integer 5 */
    uch minor;              /* e.g., 2 */
    uch patchlevel;         /* e.g., 0 */
    uch not_used;
} _zip_version_type;
typedef struct _ZpVer {
    ulg structlen;          /* length of the struct being passed */
    ulg flag;               /* bit 0: is_beta   bit 1: uses_zlib */
    char betalevel[10];     /* e.g., "g BETA" or "" */
    char date[20];          /* e.g., "4 Sep 95" (beta) or "4 September 1995" */
    char zlib_version[10];  /* e.g., "0.95" or NULL */
    _zip_version_type zip;
    _zip_version_type os2dll;
    _zip_version_type windll;
} ZpVer;

/* struct for zip2.2 */
typedef struct {          /* zip options */
	BOOL fSuffix;             /* include suffixes (not implemented) */
	BOOL fEncrypt;            /* encrypt files */
	BOOL fSystem;             /* include system and hidden files */
	BOOL fVolume;             /* Include volume label */
	BOOL fExtra;              /* Include extra attributes */
	BOOL fNoDirEntries;       /* Do not add directory entries */
	BOOL fExcludeDate;        /* Exclude files earlier than specified date */
	BOOL fIncludeDate;        /* Include only files earlier than specified date */
	BOOL fVerbose;            /* Mention oddities in zip file structure */
	BOOL fQuiet;              /* Quiet operation */
	BOOL fCRLF_LF;            /* Translate CR/LF to LF */
	BOOL fLF_CRLF;            /* Translate LF to CR/LF */
	BOOL fJunkDir;            /* Junk directory names */
	BOOL fRecurse;            /* Recurse into subdirectories */
	BOOL fGrow;               /* Allow appending to a zip file */
	BOOL fForce;              /* Make entries using DOS names (k for Katz) */
	BOOL fMove;               /* Delete files added or updated in zip file */
	BOOL fDeleteEntries;      /* Delete files from zip file */
	BOOL fUpdate;             /* Update zip file--overwrite only if newer */
	BOOL fFreshen;            /* Freshen zip file--overwrite only */
	BOOL fJunkSFX;            /* Junk SFX prefix */
	BOOL fLatestTime;         /* Set zip file time to time of latest file in it */
	BOOL fComment;            /* Put comment in zip file */
	BOOL fOffsets;            /* Update archive offsets for SFX files */
	BOOL fPrivilege;          /* Use privileges (WIN32 only) */
	BOOL fEncryption;         /* TRUE if encryption supported, else FALSE.
								 this is a read only flag */
	int  fRepair;             /* Repair archive. 1 => -F, 2 => -FF */
	char fLevel;              /* Compression level (0 - 9) */
	char Date[9];             /* Date to include after */
#define ZOPT22_PATH_MAX 128
	char szRootDir[ZOPT22_PATH_MAX]; /* Directory to use as base for zipping */
} ZPOPT22, * LPZPOPT22;

#if 0
/* struct for zip2.3 BETA */
typedef struct {          /* zip options */
LPSTR Date;               /* Date to include after */
LPSTR szRootDir;          /* Directory to use as base for zipping */
BOOL fSuffix;             /* include suffixes (not implemented) */
BOOL fEncrypt;            /* encrypt files */
BOOL fSystem;             /* include system and hidden files */
BOOL fVolume;             /* Include volume label */
BOOL fExtra;              /* Include extra attributes */
BOOL fNoDirEntries;       /* Do not add directory entries */
BOOL fExcludeDate;        /* Exclude files earlier than specified date */
BOOL fIncludeDate;        /* Include only files earlier than specified date */
BOOL fVerbose;            /* Mention oddities in zip file structure */
BOOL fQuiet;              /* Quiet operation */
BOOL fCRLF_LF;            /* Translate CR/LF to LF */
BOOL fLF_CRLF;            /* Translate LF to CR/LF */
BOOL fJunkDir;            /* Junk directory names */
BOOL fGrow;               /* Allow appending to a zip file */
BOOL fForce;              /* Make entries using DOS names (k for Katz) */
BOOL fMove;               /* Delete files added or updated in zip file */
BOOL fDeleteEntries;      /* Delete files from zip file */
BOOL fUpdate;             /* Update zip file--overwrite only if newer */
BOOL fFreshen;            /* Freshen zip file--overwrite only */
BOOL fJunkSFX;            /* Junk SFX prefix */
BOOL fLatestTime;         /* Set zip file time to time of latest file in it */
BOOL fComment;            /* Put comment in zip file */
BOOL fOffsets;            /* Update archive offsets for SFX files */
BOOL fPrivilege;          /* Use privileges (WIN32 only) */
BOOL fEncryption;         /* TRUE if encryption supported, else FALSE.
                             this is a read only flag */
int  fRecurse;            /* Recurse into subdirectories. 1 => -R, 2 => -r */
int  fRepair;             /* Repair archive. 1 => -F, 2 => -FF */
char fLevel;              /* Compression level (0 - 9) */
} ZPOPT23, * LPZPOPT23;
#else
	/* zip32 2.3 release */
typedef struct {        /* zip options */
LPSTR Date;             /* Date to include after */
LPSTR szRootDir;        /* Directory to use as base for zipping */
LPSTR szTempDir;        /* Temporary directory used during zipping */
BOOL fTemp;             /* Use temporary directory '-b' during zipping */
BOOL fSuffix;           /* include suffixes (not implemented) */
BOOL fEncrypt;          /* encrypt files */
BOOL fSystem;           /* include system and hidden files */
BOOL fVolume;           /* Include volume label */
BOOL fExtra;            /* Exclude extra attributes */
BOOL fNoDirEntries;     /* Do not add directory entries */
BOOL fExcludeDate;      /* Exclude files newer than specified date */
BOOL fIncludeDate;      /* Include only files newer than specified date */
BOOL fVerbose;          /* Mention oddities in zip file structure */
BOOL fQuiet;            /* Quiet operation */
BOOL fCRLF_LF;          /* Translate CR/LF to LF */
BOOL fLF_CRLF;          /* Translate LF to CR/LF */
BOOL fJunkDir;          /* Junk directory names */
BOOL fGrow;             /* Allow appending to a zip file */
BOOL fForce;            /* Make entries using DOS names (k for Katz) */
BOOL fMove;             /* Delete files added or updated in zip file */
BOOL fDeleteEntries;    /* Delete files from zip file */
BOOL fUpdate;           /* Update zip file--overwrite only if newer */
BOOL fFreshen;          /* Freshen zip file--overwrite only */
BOOL fJunkSFX;          /* Junk SFX prefix */
BOOL fLatestTime;       /* Set zip file time to time of latest file in it */
BOOL fComment;          /* Put comment in zip file */
BOOL fOffsets;          /* Update archive offsets for SFX files */
BOOL fPrivilege;        /* Use privileges (WIN32 only) */
BOOL fEncryption;       /* TRUE if encryption supported, else FALSE.
                           this is a read only flag */
int  fRecurse;          /* Recurse into subdirectories. 1 => -r, 2 => -R */
int  fRepair;           /* Repair archive. 1 => -F, 2 => -FF */
char fLevel;            /* Compression level (0 - 9) */
} ZPOPT23, * LPZPOPT23;
#endif

void (WINAPI *ZpVersion)(ZpVer *);
int (WINAPI *ZpInit)(ZIPUSERFUNCTIONS *lpZipUserFunc);
BOOL (WINAPI *ZpSetOptions22)(ZPOPT22 Opts);
BOOL (WINAPI *ZpSetOptions23)(LPZPOPT23 Opts);
ZPOPT22 (WINAPI *ZpGetOptions22)(void);
ZPOPT23 (WINAPI *ZpGetOptions23)(void);
int(WINAPI *ZpArchive)(ZCL C);

int ZIP32DLLLoadLibrary(void);
int ZIP32DLLFreeLibrary(void);

int ZIP32DLLVersion(void);	/* 256 * major + minor */

#endif /* ifdef _ZIPAPI_H */
