#include <kbd/globals.h>
#include "Siemens-8795148.h"

void scan(void)
{
	uint8_t x;

	for(row = 0; row <= DRIVER_ROWS; row++)//the amount of driver rows is decided by the selected driver in drivers/driver.h
	{
		kbd.row_state = 0;
		pin_state = 0;//reads a pin bit

		set(row);

		for(x = 0; x < COLUMNS; x++)
		{
			pin_state = read_PINX_bit(input_pins[x].position, input_pins[x].port);

			if(pin_state)
			{ispressed[x] = HIGH; continue;}
			else
			{ispressed[x] = LOW;}//LOW == low state on pin(aka key press), HIGH == high state, or pullup(no press)

			//previous_presses is assigned the inverse of LOW(0) or HIGH(1), so if they are equal then it has changed state
			if(previous_presses[row][x] == ispressed[x])
			{kbd.row_state = 1;}
		}

		if(!kbd.state && kbd.row_state)
		{kbd.state = 1;}

		//figure out what is being pressed in the row
		setup_keys();

		layer_select();

		if(!overrides())
		{continue;}

		press_release();

		_delay_us(35);//>switching characteristics: from A, B, C (input) to any Y (output) - Vcc 4.5V, (TYP)18ns (MAX)36ns
		//				[programmer note]: this matches the switching characteristics of most demux chips
	}
}
