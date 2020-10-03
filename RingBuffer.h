/*
 * RingBuffer.h
 *
 * Created: 23-Sep-20 16:02:22
 *  Author: mikda
 */ 


#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

class RingBuffer
{
public:
	uint8_t Read();
	uint8_t Peek();
	void Write(uint8_t in);
	uint8_t Count();
	void Flush();
	RingBuffer();
	
private:
	uint8_t buffer[256];
	uint8_t head;
	uint8_t tail;
};



#endif /* RINGBUFFER_H_ */