#if !defined(WDN_AI_BLK2_H)
#define WDN_AI_BLK2_H

#include "ai.h"

// encblock() ‚Ì–ß‚è’l
#define ENCODE_SUCCESS             (0)
//      EOISH                      (2) <== defined in ai.h
#define ENCODE_CANT_OPEN_INFILE    (3)
#define ENCODE_CANT_CREATE_OUTFILE (4)

unsigned int encblock(T_HD *hp);


#endif
