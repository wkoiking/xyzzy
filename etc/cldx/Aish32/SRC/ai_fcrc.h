/* !.. AISHMV32.DLL ç\ízéûÇ…ÇÃÇ› CRC32 É}ÉNÉçÇÕ 1 Ç…Ç»ÇÈ */

#if CRC32
int filecrc(char *name, unsigned short c16, unsigned int c32);
#else
int filecrc(char *name, unsigned short c16);
#endif
