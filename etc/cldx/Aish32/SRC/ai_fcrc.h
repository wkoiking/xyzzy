/* !.. AISHMV32.DLL 構築時にのみ CRC32 マクロは 1 になる */

#if CRC32
int filecrc(char *name, unsigned short c16, unsigned int c32);
#else
int filecrc(char *name, unsigned short c16);
#endif
