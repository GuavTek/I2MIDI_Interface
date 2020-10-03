/*
 * I2MIDI.cpp
 *
 * Created: 23-Sep-20 16:29:20
 *  Author: mikda
 */ 



ISR(TWI_vect){
	uint8_t status = TWSR >> 3;
}