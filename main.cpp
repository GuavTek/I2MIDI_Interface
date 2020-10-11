/*
 * I2MIDI_Interface.cpp
 *
 * Created: 23-Sep-20 16:18:43
 * Author : mikda
 */ 

#include "Includes.h"

void Merge();
void TimerInit();
struct MergeStatus status;
bool thruUART = false;

int main(void)
{
	UART_Init();
	I2C_Init();
	PORTB = 1 << PINB4;	//Pullup, insert button
	DDRB = 1 << PINB3;	//Insert led
	
	status.currentSource = DIN5;
	status.dinDone = 1;
	status.I2CDone = 1;
	
	
    while (1) 
    {
		thruUART = PINB & (1 << PINB4);	//Insert button here
		Merge();
		
		if (WordCountI2C() > 0)
		{
			StartTransmission();
		}
		
    }
}

void TimerInit(){
	//Set clock and CTC mode.
	TCCR1B = (1 << WGM12)|(1 << CS10);
	
	//Set timer period = 1ms
	OCR1A = 16000;
	
	//Enable interrupt
	TIMSK1 = 1 << OCIE1A;
}

//To do: implement timeout function
//Prevent source-switching caused by data not received yet
void Merge(){
	if (status.dinDone && status.I2CDone) {
		//Get character
		if (status.currentSource == DIN5)
		{
			if (bufferUART_RX.Count() > 0)
			{
				status.currentChar = bufferUART_RX.Read();	
				//reset timer
				TCNT1 = 0;
			} else {
				//No data
				return;
			}
			
			if (bufferUART_RX.Peek() > 127)
			{
				//Next char is command byte
				status.currentSource = I2C;
			}
			
			//Try to load character
			if (TXI2C(status.currentChar))
			{
				//I2C failed
				status.I2CDone = 0;
			}
			
			if (thruUART)
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
				status.currentChar = I2Cbuffer_RX.Read();
				//reset timer
				TCNT1 = 0;
			} else {
				//No data
				return;
			}
			
			if (I2Cbuffer_RX.Peek() > 127)
			{
				//Next char is command byte
				status.currentSource = DIN5;
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

ISR(TIMER1_COMPA_vect){
	//Byte timeout, switch source
	if (status.currentSource == DIN5)
	{
		status.currentSource = I2C;
	} 
	else
	{
		status.currentSource = DIN5;
	}
}

