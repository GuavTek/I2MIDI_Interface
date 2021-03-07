/*
 * I2MIDI_Interface.cpp
 *
 * Created: 23-Sep-20 16:18:43
 * Author : Davod
 */ 

#include "Includes.h"
void CLK_Init();
void Merge();
void Timer_Init();
struct MergeStatus status;

int main(void)
{
	CLK_Init();
	Timer_Init();
	UART_Init();
	I2C_Init();
	Led_Init();
	
	//Set button pullup
	//PORTB.PIN2CTRL = PORT_PULLUPEN_bm;
	
	//Set LEDS as output
	PORTB.DIRSET = PIN4_bm;
	PORTA.DIRSET = PIN7_bm;
	
	status.currentSource = DIN5;
	status.dinDone = 1;
	status.I2CDone = 1;
	
	sei();
	
	//_delay_ms(300);
	
	//TXI2C('L');
	//WordReady(1);
	
    while (1) 
    {
		Merge();
		
		if (WordCountI2C() > 0)
		{
			StartTransmission();
		}
		
    }
}

void CLK_Init(){
	//Disable prescaler
	CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = 0;
}

void Timer_Init(){
	//Set normal mode.
	TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
	
	//Set timer period = 1ms
	TCA0.SINGLE.PER = 1250;
	
	//Enable interrupt
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	
	TCA0.SINGLE.DBGCTRL = Debug;
	
	//Enable timer
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc|TCA_SINGLE_ENABLE_bm;
}


void Merge(){
	if (status.dinDone && status.I2CDone) {
		
		if (status.msgDone)
		{
			status.msgDone = 0;
			
			if (status.currentSource == DIN5)
			{
				status.currentSource = I2C;
				
				if (status.length > 0)
				{
					WordReady(status.length);
				}
			} else {
				status.currentSource = DIN5;
			}
			
			status.length = 0;
		}
		
		//Get character
		if (status.currentSource == DIN5)
		{
			if (bufferUART_RX.Count() > 0)
			{
				status.length++;
				status.currentChar = bufferUART_RX.Read();	
				//reset timer
				TCA0.SINGLE.CNT = 0;
			} else {
				//No data
				return;
			}
			
			if (bufferUART_RX.Peek() > 127)
			{
				//Next char is command byte
				status.msgDone = 1;
			}
			
			//Try to load character
			if (TXI2C(status.currentChar))
			{
				//I2C failed
				status.I2CDone = 0;
			}
			
			if (!(PORTC.IN & PIN2_bm))
			{
				if (UART_TX(status.currentChar))
				{
					//Uart failed
					status.dinDone = 0;
				}
			}
			
			
		} else {
			if (I2Cbuffer_RX.Count() > 0)
			{
				status.length++;
				status.currentChar = I2Cbuffer_RX.Read();
				//reset timer
				TCA0.SINGLE.CNT = 0;
			} else {
				//No data
				return;
			}
			
			if (I2Cbuffer_RX.Peek() > 127)
			{
				//Next char is command byte
				status.msgDone = 1;
			}
			
			//Try to load character
			if (UART_TX(status.currentChar))
			{
				//Uart failed
				status.dinDone = 0;
			}
		}
			
	} else if (!status.I2CDone) {
		if (!TXI2C(status.currentChar))
		{
			status.I2CDone = 1;
		}
	} else {
		if (!UART_TX(status.currentChar))
		{
			status.dinDone = 1;
		}
	}
}

ISR(TCA0_OVF_vect){
	//Clear interrupt flag
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
	
	//Byte timeout, switch source
	status.msgDone = 1;
}
