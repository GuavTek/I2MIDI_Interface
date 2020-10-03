/*
 * RingBuffer.cpp
 *
 * Created: 23-Sep-20 16:27:59
 *  Author: mikda
 */ 

#include "Includes.h"

RingBuffer::RingBuffer(){
	tail = 0;
	head = 0;
}

uint8_t RingBuffer::Read(){
	if (Count() > 0)
	{
		tail++;
		return buffer[tail];
	}
	return 0;
}

void RingBuffer::Write(uint8_t in){
	if (Count() < 255)
	{
		head++;
		buffer[head] = in;
	}
}

uint8_t RingBuffer::Count(){
	return head - tail;
}

void RingBuffer::Flush(){
	tail = 0;
	head = 0;
}