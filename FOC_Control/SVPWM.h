#ifndef _SVPWM_H_
#define _SVPWM_H_

#include <XMC4400.h>

typedef struct {
    int16_t PWM_Period;
    int16_t t1sat;
    int16_t t2sat;
    int16_t t1;
    int16_t t2;
    int16_t	t_Sum;
    int16_t taon;
    int16_t tbon;
    int16_t tcon;
    int16_t Part;
    int16_t Sector_Old;	
    int16_t Sector;
    int16_t	Sector_NUM;
    int16_t X;
    int16_t Y;
    int16_t Z;
    int16_t A;
    int16_t B;
    int16_t C;
    int16_t Vref1;
    int16_t Vref2;
    int16_t Vref3;
    int16_t PDC_U;
    int16_t PDC_V;
    int16_t PDC_W;
} TSVPWM16_7;


extern TSVPWM16_7 SVPWM16;


void SVPWM16_7(int16_t V_alpha, int16_t V_beta);

#endif
