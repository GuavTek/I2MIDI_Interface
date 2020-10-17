/*
 * I2C.h
 *
 * Created: 25-Sep-20 15:31:18
 *  Author: Davod
 */ 


#ifndef I2C_H_
#define I2C_H_

#include "RingBuffer.h"
#define I2CAddress 0x6c

	extern RingBuffer<32> I2Cbuffer_RX;

	void I2C_Init();
	void StartTransmission();
	void EndTransmission();
	uint8_t WordCountI2C();
	uint8_t WordReady(uint8_t length);
	void SendI2C(uint8_t ack);
	uint8_t ReadI2C();
	uint8_t RXCountI2C();
	uint8_t TXI2C(uint8_t msg);


#endif /* I2C_H_ */