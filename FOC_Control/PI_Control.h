#ifndef _PI_Control_H_
#define _PI_Control_H_
#include <XMC4400.h>

extern void PI_Id_Loop(void);
extern void PI_Iq_Loop(void);
extern void PI_SMO_PLL(void);
extern void PI_Speed_Loop(void);
extern float I_Max;
extern float I_Sum;
extern float output;
extern float P_Inc_Iq;
extern float I_Inc_Iq;
extern float Inc_Iq;
extern float Output_Iq;

extern float P_Inc_Id;
extern float I_Inc_Id;
extern float Inc_Id;
extern float Output_Id;
extern int32_t SMO_Output;

#endif

