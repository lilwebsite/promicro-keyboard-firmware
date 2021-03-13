#include "keyboard.h"

void init(void)
{
	DRIVER_ROWS = 0b1111;
	return;
}

void set(uint8_t row)
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
