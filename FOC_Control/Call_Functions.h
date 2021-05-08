#ifndef _Call_Functions_h
#define _Call_Functions_h

#include <XMC4400.h>

extern uint8_t speed_count;
extern uint32_t speed_time[32];
extern uint64_t speed_time_sum;
extern uint8_t  speed_get_flag;   //用于计算转速
extern uint16_t Interval;

extern void delay_Xms(uint16_t ticx);
extern void Motor_State_Commmand(void);
extern void Speed_Calculate(void);
extern void Speed_Calculation_Initialize(void);
extern void ASpeed_Filter(void);
extern void ASpeed_Filter_Initialize(void);
extern void SMO_IF_Filter(void);
extern void SMO_IF_Filter_Initialize(void);
extern void SMO_Speed_Est_Init(void);   
extern void SMO_Speed_Est(void);
#endif

