#include <cstdio>
#include "microfio.h"

using namespace std;

extern int noerrflag;

extern char *Gstdout_buf;
extern int Gstdout_buf_pos;
extern unsigned long Gstdout_buf_size;

void Mputchar(char ch);
int Mcreate(m_file *fm, char *name);
int Mappend(m_file *fm, char *name);
int Mclose(m_file *fm);
void Merrch(char ch);
void Merr(char *ptr);
int Mopen(m_file *fm, char *name);
size_t Mgets(m_file *fm, unsigned char *buf, unsigned int len);
size_t Mwrite(m_file *fm, char *ptr, size_t len);
int Mputs(m_file *fm, char *ptr);
void Mprint(const char *ptr);
size_t Mread(m_file *fm, void *buf, size_t len);
void Mflush(m_file *fm);
void Mseek(m_file *fm, long pos, int mode);
int Misfile(m_file *fm);
int Maccess(const char *name);
