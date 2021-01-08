/*
 * I2C.cpp
 *
 * Created: 25-Sep-20 15:31:00
 *  Author: Davod
 */ 

	#include "Includes.h"

	#define SCL_FREQ 100000
	#define TWBAUD (F_CPU-10)/(2*SCL_FREQ)
	#define CareAck false
	
	uint8_t currentChar;
	
	RingBuffer<16> wordLength;
	uint8_t currentWord;
		
	bool isTransmitting = false;
	bool addressDone = false;
	
	RingBuffer<32> I2Cbuffer_RX;
	RingBuffer<32> I2Cbuffer_TX;

//Initialize I2C
void I2C_Init(){
	//Set pins high because weird bug
	PORTB.OUTSET = PIN0_bm|PIN1_bm;
	
	//Set SCL
	TWI0.MBAUD = TWBAUD;
	//TWI0_MBAUD = TWBAUD;
	
	//Set master interrupt
	TWI0.MCTRLA = TWI_WIEN_bm|TWI_ENABLE_bm;
	//TWI0_MCTRLA = TWI_WIEN_bm|TWI_ENABLE_bm;
	
	TWI0.MCTRLB = TWI_FLUSH_bm;
	
	//Force bus-state to idle
	TWI0.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	//TWI0_MSTATUS = TWI_BUSSTATE_IDLE_gc|TWI_WIF_bm;
		
	//Set slave address
	TWI0.SADDR = I2CAddress << 1;
	//TWI0_SADDR = I2CAddress << 1;
	
	//Set slave interrupt
	TWI0.SCTRLA = TWI_DIEN_bm|TWI_SMEN_bm|TWI_ENABLE_bm;
	//TWI0_SCTRLA = TWI_DIEN_bm|TWI_SMEN_bm|TWI_ENABLE_bm;	
}

//Read from RX buffer
uint8_t ReadI2C(){
	return I2Cbuffer_RX.Read();
}

//Loads next byte into I2C data register
void SendI2C(uint8_t ack){
	
	if (ack)
	{
		if (currentWord == 0)
		{
			EndTransmission();		
			return;
		} else {
			currentWord--;
			currentChar = I2Cbuffer_TX.Read();
		}
	}
	
	addressDone = true;
	//Put byte in TWI buffer
	TWI0.MDATA = currentChar;
	//TWI0_MDATA = currentChar;
	
	I2C_Activity();
}

//Indicates that a message has been loaded into TX buffer
uint8_t WordReady(uint8_t length){
	if (wordLength.Count() >= wordLength.length - 1)
	{
		return 1; //Failed, full
	}
	
	wordLength.Write(length);
	
	return 0;	//Success
}

//Starts I2C transmission if there is none ongoing
void StartTransmission(){
	if (!isTransmitting)
	{
		if ((TWI0.MSTATUS & 0x03) == TWI_BUSSTATE_BUSY_gc)
		{
			return;
		}
		
		TWI0.MADDR = I2CAddress << 1;
		
		currentWord = wordLength.Read();
		addressDone = false;
		isTransmitting = true;
		SendI2C(1);
	}
}

//Stops I2C transmission
void EndTransmission(){
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
	//TWI0_MCTRLB |= TWI_MCMD_STOP_gc;
	isTransmitting = false;
}

//Returns amount of data in RX buffer
uint8_t RXCountI2C(){
	return I2Cbuffer_RX.Count();
}

//Returns how many MIDI messages are loaded in buffers
uint8_t WordCountI2C(){
	return wordLength.Count();
}

//Loads I2C byte into buffer
uint8_t TXI2C(uint8_t msg){
	if (I2Cbuffer_TX.Count() > I2Cbuffer_TX.length)
	{
		return 1;
	}
	I2Cbuffer_TX.Write(msg);
	return 0;
}

//Slave interrupt
ISR(TWI0_TWIS_vect){
	I2Cbuffer_RX.Write(TWI0.SDATA);
	TWI0.SCTRLB = TWI_SCMD_RESPONSE_gc;
	//TWI0_SCTRLB = TWI_SCMD_RESPONSE_gc;
}

//Master interrupt
ISR(TWI0_TWIM_vect){
	uint8_t status = TWI0.MSTATUS;
	//uint8_t status = TWI0_MSTATUS;
	
	if ((status & TWI_ARBLOST_bm)) {
		//Lost arbitration, resend
		if (addressDone)
		{
			SendI2C(0);
		}
		
		//Clear arblost flag
		TWI0.MSTATUS |= TWI_ARBLOST_bm;
		//TWI0_MSTATUS |= TWI_ARBLOST_bm;
	} else if ((status & TWI_RXACK_bm) || !CareAck) {
		//ack
		SendI2C(1);
	} else {
		//No ack
		SendI2C(0);
	}
	
}