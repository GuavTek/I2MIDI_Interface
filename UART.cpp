/*
 * UART.c
 *
 * Created: 23-Sep-20 14:42:40
 *  Author: mikda
 */ 

#include "Includes.h"

RingBuffer bufferUART_RX;
RingBuffer bufferUART_TX;

#define MIDIRate (F_CPU/(16*31250))-1

void UART_Init(){
	//Set Baudrate = 31250Hz
	UBRR0 = MIDIRate;
	
	//Enable RX, TX, and interrupts
	UCSR0B = (1 << RXEN0)|(1 << TXEN0)|(1 << RXCIE0);
	
	//Set frame format: 8bit, 1stop
	UCSR0C = 0b11 << UCSZ00;
}

uint8_t UART_TX(uint8_t msg){
	if (bufferUART_TX.Count() > 254)
	{
		return 1;
	}
	bufferUART_TX.Write(msg);
	
	//Enable interrupt
	UCSR0B |= 1 << UDRIE0;
	return 0;
}

ISR(USART_RX_vect){
	uint8_t rx = UDR0;
	bufferUART_RX.Write(rx);
}

ISR(USART_UDRE_vect){
	if (bufferUART_TX.Count() > 0)
	{
		UDR0 = bufferUART_TX.Read();
	} else {
		//Disable interrupt if send register is empty
		UCSR0B &= ~(1 << UDRIE0);
	}
}
