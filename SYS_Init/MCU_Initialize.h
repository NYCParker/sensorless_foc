#ifndef _MCU_Initialize_h
#define _MCU_Initialize_h

#include <XMC4400.h>
#include "VAR_Initialize.h"

//≤‚ ‘
#define Test_Output  P1_0

//¡˘¬∑PWM
#define PWM_UH	P0_5
#define PWM_UL 	P0_2
#define PWM_VH 	P0_4
#define PWM_VL 	P0_1
#define PWM_WH 	P0_3
#define PWM_WL 	P0_0

//ADC ‰»Î
#define R_ADC   P14_1  //G0CH1
#define Iu_ADC  P14_2  //G0CH2
#define Iw_ADC  P15_2  //G2CH2
#define Iv_ADC  P14_8  //G3CH2

//
#define GPIO_RX  P2_2
#define GPIO_TX  P2_5

extern void GPIO_Init(void);
extern void CCU80_Init(void);
extern void CCU80_Start(void);
extern void NVIC_Handle(void);
extern void ADC_Init(void);
extern void POSIF0_Init(void);
extern void Switch_off(void);
extern void Switch_on(void);
#endif
