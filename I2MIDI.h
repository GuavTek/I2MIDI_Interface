/*
 * I2MIDI.h
 *
 * Created: 23-Sep-20 14:45:39
 *  Author: mikda
 */ 


#ifndef I2MIDI_H_
#define I2MIDI_H_

#include "RingBuffer.h"
#define I2MIDI_master_mode 1

extern RingBuffer I2Cbuffer_RX;

class I2MIDI
{
public:
	void Send(uint8_t msg);
	I2MIDI();
private:
	const uint8_t address = 0x6C;
};
void I2MIDI_Init();
void I2MIDI_Send(uint8_t msg);


#endif /* I2MIDI_H_ */