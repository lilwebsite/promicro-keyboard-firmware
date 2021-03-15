#include "keyboard.h"

void init(void)
{
	DRIVER_ROWS = 0b111;
	return;
}

void set(uint8_t row)
{
	uint8_t ABC[3];
	uint8_t chip_select = 0b11; // selects nothing if set to this
	// 0b01 <-- selects chip 1
	// 0b10 <-- selects chip 2
	// 0b00 <-- selects chip 3


	switch(row & 0b11000)
	{
		case 0b10000:
			chip_select = 0b00;
			ABC[0] = (0b100 & (row - 16))>>2;
			ABC[1] = (0b010 & (row - 16))>>1;
			ABC[2] = (0b001 & (row - 16));
		case 0b1000:
			chip_select = 0b10;
			ABC[0] = (0b100 & (row - 8))>>2;
			ABC[1] = (0b010 & (row - 8))>>1;
			ABC[2] = (0b001 & (row - 8));
		default:
			chip_select = 0b01;
			ABC[0] = (0b100 & row)>>2;
			ABC[1] = (0b010 & row)>>1;
			ABC[2] = (0b001 & row);
	}

	set_PINX_variable_output(7, F, (chip_select >> 1) & 0b1); // pin A0, chip select 1
	set_PINX_variable_output(1, B, chip_select & 0b1); // pin 15, chip select 2

	// switching characteristics for 74HCT00 is 6ns but I put a NOP call here for good measure
	_NOP();

	for(uint8_t x = 0; x < DEMUX_IN; x++)
	{set_PINX_variable_output(demux_pins[x].position, demux_pins[x].port, ABC[x]);}

	// switching characteristics for 74hc138 should be an absolute maximum of 265ns, so it gets a NOP as well
	_NOP();

	return;
}
