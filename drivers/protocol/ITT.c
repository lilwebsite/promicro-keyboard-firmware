#include "ITT.h"
#include <layout.h>

void scan(void)
{

	clock_data ^= 0b1;
	read_PINX_bit(ITT_DATA.position, ITT_DATA.port) ? (clock_data |= 0b10) : (clock_data &= 0xFD); // record data line value
	set_PINX_variable_output(ITT_C1.position, ITT_C1.port, (clock_data & 0b1)); // set clock value

	switch(clock_data & 0b101)
	{
		// if clock is still high and clock was already read or clock is low
		case 0b0:
		case 0b101:
			_delay_us(20);
			return;
		// if clock is waiting to be low and is low: reset the clock and data read bit
		case 0b100:
			clock_data &= 0xF3;
		default:
			break;
	}

	switch(clock_data & 0x3F)
	{
		// first step / reset stage
		case 0b11:
			counter = 0;
			clock_data |= 0b11100;
			return;
		// signal candidate, data / clock are both high
		case 0b010011:
			// if the counter is at the end
			if(counter == 131)
			{
				counter = 0;
				clock_data |= 0b101100;
				return;
			}
			counter = 0;
			clock_data &= 0xCF;
			clock_data |= 0b1100;
			return;
		//if the signal found and there is a data point
		case 0b110011:
		case 0b110001:
			if(counter != 130)
			{
				clock_data |= (0b10 & clock_data) << 5;
				clock_data |= (received[counter] ^ ((0b10 & clock_data) >> 1)) << 7;
				pressed_counter += (0b10 & clock_data) >> 1;
				received[counter] = ((0b10 & clock_data) >> 1);
			}
			if(counter == 131)
			{
				counter = 0;
				if(clock_data & 0b10)
				{break;}
				else
				{
					clock_data &= 0xCF;
					clock_data |= 0b1100;
					return;
				}
			}
			if(counter == 0)
			{
				if(last_pressed_count != pressed_counter || !pressed_counter)
				{
					//overrides();
					kbsend();
				}
				last_pressed_count = pressed_counter;
				pressed_counter = 0;
				clock_data &= 0x3F;
			}
		case 0b010001:
			clock_data |= 0b100;
			counter++;
		default:
			return;
	}

	// if no changes in currently pressed keys
	if((clock_data & 0x80) == 0)
	{return;}

	reset_sending();

	if(received[SOLENOID_DISABLE])
	{
		solenoid_toggle();
	}

	for(; counter < COLUMNS; counter++)
	{
		struct keypress target = {keymap[counter], counter, counter};
		kbd_press_release(received[counter], target);
	}
	counter = 0;
	
	#if NO_SOLENOID==0
	// if a keypress was detected
	if((clock_data & 0x40) == 0x40)
	{solenoid = 0b11;}

	if((solenoid & 0b111) == 0b11
	&& pressed_counter >= (last_pressed_count + 1))
	{
		// I am using the function call overhead as a delay here
		// gives it just the right amount of time
		//set_PINX_variable_output(6, F, 1);
		//set_PINX_variable_output(7, F, 1);
		//set_PINX_variable_output(7, F, 0);
		//set_PINX_variable_output(6, F, 0);
		set_PINX_variable_output(ITT_SOLENOID.position, ITT_SOLENOID.port, 1);
		set_PINX_variable_output(ITT_C1.position, ITT_C1.port, 1);
		set_PINX_variable_output(ITT_C1.position, ITT_C1.port, 0);
		set_PINX_variable_output(ITT_SOLENOID.position, ITT_SOLENOID.port, 0);

		solenoid = 0b101;
	}
	#endif

	return;
}
