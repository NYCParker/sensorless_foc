#ifndef _DAC_H_
#define _DAC_H_
#include <XMC4400.h>

void DAC_Init(void);
void DAC_Output1(int16_t dat);
void DAC_Output0(int16_t dat);
void DAC_vInit(void);	

#endif
