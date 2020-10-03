/*
 * I2C.h
 *
 * Created: 25-Sep-20 15:31:18
 *  Author: mikda
 */ 


#ifndef I2C_H_
#define I2C_H_

#include "RingBuffer.h"
#define I2CAddress 0x6c
extern RingBuffer I2Cbuffer_RX;


	void StartTransmission();
	void EndTransmission();
	uint8_t WordCountI2C();
	void WordReady(uint8_t length);
	void SendI2C(uint8_t ack);
	uint8_t ReadI2C();
	uint8_t RXCountI2C();

};


#endif /* I2C_H_ */