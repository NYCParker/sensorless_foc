#ifndef _AGREEMENT_H_
#define _AGREEMENT_H_


#include <XMC4400.h>

extern void Communication_App(void);
extern void Virtual_Scope(void);
void Uart_Rec(void);
extern void Serial_Communication(void);

extern uint8_t hand_shake_Flag;    //Œ’ ÷
extern uint8_t uart_state_flag;
extern uint16_t Current_Cmd_Iq;
#endif
