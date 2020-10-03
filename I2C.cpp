/*
 * I2C.cpp
 *
 * Created: 25-Sep-20 15:31:00
 *  Author: mikda
 */ 

	#include "Includes.h"

	#define SCL_FREQ 100000
	#define TWBR_VAL F_CPU/(2*SCL_FREQ) - 8
	#define RepeatNoAck false
	
	uint8_t currentChar;
	
	uint8_t wordLength[32];
	uint8_t wordHead = 0;
	uint8_t wordTail = 0;
	
	bool isTransmitting = false;
	
	RingBuffer I2Cbuffer_RX;
	RingBuffer I2Cbuffer_TX;

//Initialize I2C
void I2C_Init(){
	//Set SCL
	TWBR = TWBR_VAL;
	
	//Set slave address
	TWAR = I2CAddress << 1;
	
	//Enable TW and interrupt
	TWCR = (1 << TWEN)|(1 << TWIE)|(1 << TWEA);
	
}

//Read from RX buffer
uint8_t ReadI2C(){
	return I2Cbuffer_RX.Read();
}

//Loads next byte into I2C data register
void SendI2C(uint8_t ack){
	
	if (ack | !RepeatNoAck)
	{
		if (wordLength[wordTail] == 0)
		{
			EndTransmission();
			
			wordTail++;
			if (wordTail > 31)
			{
				wordTail = 0;
			}
			
			return;
		} else {
			wordLength[wordTail]--;
			currentChar = I2Cbuffer_TX.Read();
		}
	}
	TWDR = currentChar;
}

//Indicates that a message has been loaded into TX buffer
uint8_t WordReady(uint8_t length){
	if (WordCountI2C() > 31)
	{
		return 1; //Failed, full
	}
	wordHead++;
	if (wordHead > 31)
	{
		wordHead = 0;
	}
	wordLength[wordHead] = length;
	
	return 0;	//Success
}

//Starts I2C transmission if there is none ongoing
void StartTransmission(){
	if (!isTransmitting)
	{
		TWCR |= (1 << TWSTA);
		isTransmitting = true;
	}
}

//Stops I2C transmission
void EndTransmission(){
	TWCR |= (1 << TWSTO);
	isTransmitting = false;
}

//Returns amount of data in RX buffer
uint8_t RXCountI2C(){
	return I2Cbuffer_RX.Count();
}

//Returns how many MIDI messages are loaded in buffers
uint8_t WordCountI2C(){
	uint8_t tempCount = wordHead - wordTail;
/*	if (tempCount > 31)
	{
		tempCount -= 255;
	}*/
	return tempCount;
}

//Loads I2C byte into buffer
uint8_t TXI2C(uint8_t msg){
	if (I2Cbuffer_TX.Count() > 254)
	{
		return 1;
	}
	I2Cbuffer_TX.Write(msg);
	return 0;
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
	TWCR |= 1 << TWINT;
}