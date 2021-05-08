#ifndef UART_H_
#define UART_H_

#include <XMC4400.h>

extern void UART_Init(void);
extern void UART_SendData(uint16_t uwData);
extern void UART_SendBuffer(const uint8_t *Buffer, uint16_t length);

#endif
