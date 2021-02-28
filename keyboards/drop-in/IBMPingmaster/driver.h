#include "scanners/drop-in/SN74159N.h"
//SN74159N is a 4 to 16 line demultiplexer

///////////////
//USEFUL INFO//
///////////////
//		pro micro pin layout -> https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro16MHzv1.pdf
//
//		demultiplexer datasheet -> https://html.alldatasheet.com/html-pdf/27373/TI/SN74159N/22/1/SN74159N.html
//		OR
//		search for SN74159N

//scanvalues used to check each row 0 - 15 (for a 4 to 16 line demultiplexer)
//DCBA
//0000 - x . . . . . . . . . . . . . . .
//0001 - . x . . . . . . . . . . . . . .
//0010 - . . x . . . . . . . . . . . . .
//0011 - . . . x . . . . . . . . . . . .
//0100 - . . . . x . . . . . . . . . . .
//0101 - . . . . . x . . . . . . . . . .
//0110 - . . . . . . x . . . . . . . . .
//0111 - . . . . . . . x . . . . . . . .
//1000 - . . . . . . . . x . . . . . . .
//1001 - . . . . . . . . . x . . . . . .
//1010 - . . . . . . . . . . x . . . . .
//1011 - . . . . . . . . . . . x . . . .
//1100 - . . . . . . . . . . . . x . . .
//1101 - . . . . . . . . . . . . . x . .
//1110 - . . . . . . . . . . . . . . x .
//1111 - . . . . . . . . . . . . . . . x

#define SCANVALUES \
{\
	{0, 0, 0, 0}\
	{0, 0, 0, 1}\
	{0, 0, 1, 0}\
	{0, 0, 1, 1}\
	{0, 1, 0, 0}\
	{0, 1, 0, 1}\
	{0, 1, 1, 0}\
	{0, 1, 1, 1}\
	{1, 0, 0, 0}\
	{1, 0, 0, 1}\
	{1, 0, 1, 0}\
	{1, 0, 1, 1}\
	{1, 1, 0, 0}\
	{1, 1, 0, 1}\
	{1, 1, 1, 0}\
	{1, 1, 1, 1}\
}

//static const struct pin demux_pins[DEMUX_IN] = {{B, 1}, {B, 3}, {B, 2}, {B, 6}};
//A, B, C, D (pins 15, 14, 16, 10)

const struct pin demux_pins[DEMUX_IN] = {{F, 4}, {F, 5}, {F, 6}, {F, 7}};
//A, B, C, D (pins A0, A1, A2, A3)

void init_IBMPingmaster(void)
{
	for(uint8_t x = 0; x < DEMUX_IN - 1; x++)
	{
		DRIVER_ROWS = (DRIVER_ROWS<<1);
		DRIVER_ROWS++;
	}
	return;
}

void set_IBMPingmaster(uint8_t row)
{
	uint8_t ABCD[4];
	ABCD[0] = (0b1000 & row)>>3;
	ABCD[1] = (0b0100 & row)>>2;
	ABCD[2] = (0b0010 & row)>>1;
	ABCD[3] = 0b0001 & row;
	for(uint8_t x = 0; x < DEMUX_IN; x++)
	{set_PINX_variable_output(demux_pins[x].position, demux_pins[x].port, ABCD[x]);}
	_delay_us(35);//>switching characteristics: from A, B, C (input) to any Y (output) - Vcc 4.5V, (TYP)18ns (MAX)36ns
	//				[programmer note]: this matches the switching characteristics of most demux chips
	return;
}
