#include <cstdio>
#include <sys/types.h>

using namespace std;

extern unsigned char _sf_jis7[2];
extern unsigned char _sf_jis8[2];
extern unsigned char _sf_sjis[2];
extern unsigned char _sf_nksjis[2];

unsigned int filedate(int handle);
off_t filesize(int handle);
unsigned int fileperm(int handle);
void filesetdate(unsigned char *name, unsigned int date);
void *memclr(size_t len, void *ptr);
void timecpy(unsigned int *dst, unsigned int *src);
void DATESTR(unsigned int date, char *str);
void TIMESTR(unsigned int time, char *str);
