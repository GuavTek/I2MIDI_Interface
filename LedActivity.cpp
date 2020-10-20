/*
 * LedActivity.cpp
 *
 * Created: 17-Oct-20 23:13:02
 *  Author: Davod
 */ 

#include "Includes.h"

void Led_Init(){
	//Set clk
	TCB0.CTRLA = TCB_CLKSEL_CLKDIV2_gc;

	//Enable interrupt
	TCB0.INTCTRL = TCB_CAPT_bm;
	
	TCB0.DBGCTRL = Debug;
	
	//Periodic interrupt mode
	TCB0.CTRLB = TCB_CNTMODE_INT_gc;
	
	//Set period
	TCB0.CCMP = 8000;
	
	//Enable timer
	TCB0.CTRLA |= TCB_ENABLE_bm;

	//Enable LED outputs
	PORTA.DIRSET = PIN7_bm; //I2C
	PORTB.DIRSET = PIN4_bm; //DIN
	
	PORTB.OUTSET = PIN4_bm;
	PORTA.OUTSET = PIN7_bm;
}

void UART_Activity(){
	PORTB.OUTSET = PIN4_bm;
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

void I2C_Activity(){
	PORTA.OUTSET = PIN7_bm;
	TCB0.CTRLA |= TCB_ENABLE_bm;
}

ISR(TCB0_INT_vect){
	//Clear flag
	TCB0.INTFLAGS = TCB_CAPT_bm;
	
	//Disable timer
	TCB0.CTRLA &= ~(TCB_ENABLE_bm);
	
	//Turn off leds
	PORTA.OUTCLR = PIN7_bm;
	PORTB.OUTCLR = PIN4_bm;
}