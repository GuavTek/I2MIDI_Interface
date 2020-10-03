/*
 * I2C.cpp
 *
 * Created: 25-Sep-20 15:31:00
 *  Author: mikda
 */ 

	#include "Includes.h"

	#define SCL_FREQ = 50000
	#define TWBR_VAL = F_CPU/(2*SCL_FREQ) - 8
	
	uint8_t currentChar;
	
	uint8_t wordLength[32];
	uint8_t wordHead = 0;
	uint8_t wordTail = 0;
	
	
	RingBuffer I2Cbuffer_RX;
	RingBuffer I2Cbuffer_TX;

void I2C_Init(){
	//Set SCL = 50kHz
	TWBR = TWBR_VAL;
	
	//Set slave address
	TWAR = I2CAddress << 1;
	
	//Enable TW and interrupt
	TWCR = (1 << TWEN)|(1 << TWIE)|(1 << TWEA);
	
}

uint8_t ReadI2C(){
	return I2Cbuffer_RX.Read();
}

void SendI2C(uint8_t ack){
	if (ack)
	{
		if (wordLength[wordTail] == 0)
		{
			EndTransmission();
			return;
		} else {
			wordLength[wordTail]--;
			currentChar = I2Cbuffer_TX.Read();
		}
	}
	TWDR = currentChar;
}

void WordReady(uint8_t length){
	
}

void StartTransmission(){
	
}

void EndTransmission(){
	
	
}

uint8_t RXCountI2C(){
	return I2Cbuffer_RX.Count();
}

uint8_t WordCountI2C(){
	uint8_t tempCount = wordHead - wordTail;
	if (tempCount > 31)
	{
		tempCount -= 255;
	}
	return tempCount;
}

ISR(TWI_vect){
	uint8_t status = TWSR & 0b11111000;
	if ((status  == 0x80)|(status == 0x88)|(status == 0x90)|(status == 0x98))
	{
		I2Cbuffer_RX.Write(TWDR);
	}
	else if ((status == 0x08)|(status == 0x10)|(status == 0x28))
	{
		//ACK received
		SendI2C(1);
	}
	else if ((status == 0x20)|(status == 0x30))
	{
		//not ack
		SendI2C(0);
	}
}