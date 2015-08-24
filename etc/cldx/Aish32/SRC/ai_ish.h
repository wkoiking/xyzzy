#include "ai.h"
#include "microfio.h"

extern m_file fm;
extern m_file fmo;
extern T_HD hp;

char *getfname(char *name);
unsigned int ai_decode(unsigned char *ptr, unsigned int len);
void ai_decode_lastchance(void);
