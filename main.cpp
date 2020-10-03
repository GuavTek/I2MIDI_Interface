/*
 * I2MIDI_Interface.cpp
 *
 * Created: 23-Sep-20 16:18:43
 * Author : mikda
 */ 

#include "Includes.h"

void Merge();
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
			} else {
				//No data, change source
				status.currentSource = I2C;
				return;
			}
			
			if (bufferUART_RX.Peek() > 127)
			{
				//Next char is command byte
				status.currentSource = I2C;
				status.msgDone = 1;
			}
			
		} else {
			if (I2Cbuffer_RX.Count() > 0)
			{
				status.currentChar = I2Cbuffer_RX.Read();
			} else {
				//No data, change source
				status.currentSource = DIN5;
				return;
			}
			
			if (I2Cbuffer_RX.Peek() > 127)
			{
				//Next char is command byte
				status.currentSource = DIN5;
				status.msgDone = 1;
			}
			
		}
		
		//Check if char is command byte
		
		//Try to load character
		
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

