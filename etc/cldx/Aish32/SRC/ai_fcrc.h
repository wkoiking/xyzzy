/* !.. AISHMV32.DLL �\�z���ɂ̂� CRC32 �}�N���� 1 �ɂȂ� */

#if CRC32
int filecrc(char *name, unsigned short c16, unsigned int c32);
#else
int filecrc(char *name, unsigned short c16);
#endif
