#include <keyboard.h>
#include <layout.h>

//void solenoid_toggle(void)
//{
//	solenoid ^= 0b1;
//	solenoid |= 0b100;
//	solenoid &= 0b101;
//	return;
//}

void init(void)
{
	// this pin needs to be HIGH for the keyboard to work, not sure what it's other functions are
	pin_value(ITT_EN.position, ITT_EN.port, 1);

	// solenoid trigger
	pin_value(ITT_SOLENOID.position, ITT_SOLENOID.port, 0); 

	// data signal
	pin_input(ITT_DATA.position, ITT_DATA.port, 0); 
}

void scan(void)
{

	clock_data ^= 0b1;
	read_pin(ITT_DATA.port, ITT_DATA.position) ? (clock_data |= 0b10) : (clock_data &= 0xFD); // record data line value
	pin_value(ITT_CLOCK.port, ITT_CLOCK.position, (clock_data & 0b1)); // set clock value

	switch(clock_data & 0b101)
	{
		// if clock is still high and clock was already read or clock is low
		case 0b0:
			if(counter == 0)
			{
				kbd.state = 1;
				last_pressed_count = pressed_counter;
				pressed_counter = 0;
			}
		case 0b101:
			_delay_us(5);
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

	for(; counter < 130; counter++)
	{
		struct keypress target = {pgm_read_byte(&(keymap[counter])), counter, counter};
		kbd_press_release(received[counter], target);
	}
	counter = 0;
	
	#if NO_SOLENOID==0
	if(received[SOLENOID_DISABLE])
	{
		solenoid ^= 0b1;
		solenoid |= 0b100;
		_delay_ms(50); // debounce
	}

	// if a keypress was detected
	if((clock_data & 0x40) == 0x40)
	{solenoid = (solenoid & 0b1) | 0b10;}

	if((solenoid & 0b111) == 0b11
	&& pressed_counter > last_pressed_count)
	{
		// I am using the function call overhead as a delay here
		// gives it just the right amount of time
		pin_value(ITT_SOLENOID.position, ITT_SOLENOID.port, 1);
		pin_value(ITT_CLOCK.position, ITT_CLOCK.port, 1);
		pin_value(ITT_CLOCK.position, ITT_CLOCK.port, 0);
		pin_value(ITT_SOLENOID.position, ITT_SOLENOID.port, 0);

		solenoid = 0b101;
	}
	#endif

	return;
}
