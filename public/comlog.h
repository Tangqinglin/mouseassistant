
#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "platformdef.h"


void ProcessSIG(int iSig);
void PrintHex(char *p, int len);


#ifdef __cplusplus
}
#endif

#endif  //_LOG_H_



