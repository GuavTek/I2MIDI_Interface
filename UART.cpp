/*
 * UART.c
 *
 * Created: 23-Sep-20 14:42:40
 *  Author: Davod
 */ 

#include "Includes.h"

RingBuffer<32> bufferUART_RX;
RingBuffer<16> bufferUART_TX;

//Yooooo, USART0.registers are fucked up
//Does not work

#define MIDIRate (4*F_CPU/31250)

void UART_Init(){
	PORTMUX.CTRLB = PORTMUX_USART0_DEFAULT_gc;
	
	//Invert output
	PORTB.PIN2CTRL = PORT_INVEN_bm;
	
	//Configure output pin
	PORTB.DIRSET = PIN2_bm;
	PORTB.OUTSET = PIN2_bm;
	
	//Set Baudrate = 31250Hz
	//USART0.BAUD = MIDIRate;
	USART0_BAUD = MIDIRate;
	
	//USART0.DBGCTRL = Debug;
	USART0_DBGCTRL = Debug;
	
	//Set frame format: 8bit, 1stop
	//USART0.CTRLC = USART_CHSIZE_8BIT_gc|USART_CMODE_ASYNCHRONOUS_gc;
	USART0_CTRLC = USART_CHSIZE_8BIT_gc|USART_CMODE_ASYNCHRONOUS_gc;
	
	//Invert input?
	//PORTB.PIN3CTRL = PORT_INVEN_bm;
	PORTB.DIRCLR = PIN3_bm;
	
	//Enable RX interrupt
	//USART0.CTRLA = USART_RXCIE_bm;
	USART0_CTRLA = USART_RXCIE_bm;
	
	//Enable RX and TX
	//USART0.CTRLB = USART_RXEN_bm|USART_TXEN_bm;
	USART0_CTRLB = USART_RXEN_bm|USART_TXEN_bm;
}

uint8_t UART_TX(uint8_t msg){
	if (bufferUART_TX.Count() > 254)
	{
		return 1;
	}
	bufferUART_TX.Write(msg);
	
	//Enable interrupt
	//USART0.CTRLA |= USART_DREIE_bm;
	USART0_CTRLA |= USART_DREIE_bm;
	return 0;
}

ISR(USART0_RXC_vect){
	//uint8_t rx = USART0.RXDATAL;
	uint8_t rx = USART0_RXDATAL;
	bufferUART_RX.Write(rx);
}

ISR(USART0_DRE_vect){
	if (bufferUART_TX.Count() > 0)
	{
		//USART0.TXDATAL = bufferUART_TX.Read();
		USART0_TXDATAL = bufferUART_TX.Read();
		UART_Activity();
	} else {
		//Disable interrupt if send register is empty
		//USART0.CTRLA &= ~(USART_DREIE_bm);
		USART0_CTRLA &= ~(USART_DREIE_bm);
	}
}
