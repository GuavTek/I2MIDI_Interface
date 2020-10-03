/*
 * UART.h
 *
 * Created: 23-Sep-20 14:43:00
 *  Author: mikda
 */ 


#ifndef UART_H_
#define UART_H_

#include "RingBuffer.h"

extern RingBuffer bufferUART_RX;

void UART_Init();
uint8_t UART_TX(uint8_t msg);


#endif /* UART_H_ */