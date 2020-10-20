/*
 * Includes.h
 *
 * Created: 23-Sep-20 14:43:24
 *  Author: Davod
 */ 


#ifndef INCLUDES_H_
#define INCLUDES_H_

#define F_CPU 20000000
#define Debug 0

enum MIDISource {
	DIN5,
	I2C
};

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"
#include "I2C.h"
#include "RingBuffer.h"
#include "LedActivity.h"

struct MergeStatus{
	uint8_t currentChar;
	enum MIDISource currentSource;
	uint8_t dinDone;
	uint8_t I2CDone;
	uint8_t msgDone;
};

#endif /* INCLUDES_H_ */