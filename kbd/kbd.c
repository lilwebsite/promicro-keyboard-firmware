#include "kbd.h"

int main(void)
{
	while(!init_promicro());
	usb_init();
	while(!usb_configured());

	static uint8_t data_lines = 0;
	static uint8_t counter = 0;
	static uint8_t last_pressed_count = 0;
	static uint8_t pressed_counter = 0;
	//static uint8_t solenoid = 1; // enables solenoid
	//static uint8_t solenoid = 0; // disables solenoid
	// 0001 -> solenoid enable
	// 0010 -> trigger solenoid
	// 0100 -> solenoid triggered
	static uint8_t clock_data = 0;
	// 0000 0001 -> clock bit
	// 0000 0010 -> data bit
	// 0000 0100 -> clock read
	// 0000 1000 -> data read
	// 0001 0000 -> candidate bit
	// 0010 0000 -> found bit
	// 0100 0000 -> keypress detected
	// 1000 0000 -> output state changed

	// solenoid enable
	//set_PINX_variable_output(6, B, solenoid & 0b1);
	// solenoid trigger
	//set_PINX_variable_output(2, B, 0);
	//input pin A4
	set_PINX_input(4, F, 0);
	//enable pin A3
	set_PINX_variable_output(5, F, 1);
	//trigger pin A2
	set_PINX_variable_output(6, F, 0);

	while(1)
	{
		//(PINB & 0b10) ? (clock_data |= 0b1) : (clock_data &= 0xFE);
		//clock_data &= 0x7F;
		clock_data ^= 0b1;
		//_delay_us(5);
		(PINF & 0b10000) ? (clock_data |= 0b10) : (clock_data &= 0xFD);

		//data_lines = (clock_data & 0b1) << 3;
		//data_lines = clock_data & 0b1;

		//DDRF = data_lines << 4;
		//PORTF = data_lines << 4;
		set_PINX_variable_output(7, F, (clock_data & 0b1));

		switch(clock_data & 0b101)
		{
			// if clock is still high and clock was already read or clock is low
			case 0b0:
			case 0b101:
				_delay_us(5);
				continue;
			// if clock is waiting to be low and is low: reset the clock and data read bit
			case 0b100:
				clock_data &= 0xF3;
			default:
				break;
		}

		//(PINB & 0b1000) ? (clock_data |= 0b10) : (clock_data &= 0xFD);
		init_pins();
		switch(clock_data & 0x3F)
		{
			// first step / reset stage
			case 0b11:
				set_PINX_variable_output(5, B, 1);
				counter = 0;
				clock_data |= 0b11100;
				continue;
			// signal candidate and data / clock is high
			case 0b010011:
				if(counter == 131)
				{
					counter = 0;
					clock_data |= 0b101100;
					continue;
				}
				set_PINX_variable_output(4, B, 1);
				counter = 0;
				clock_data &= 0xCF;
				clock_data |= 0b1100;
				continue;
			//if the signal found and there is a data point
			case 0b110011:
			case 0b110001:
				if(counter < 30)
				{
					clock_data &= 0x3F;
					if(clock_data & 0b10)
					{
						counter = 0;
						clock_data &= 0xCF;
						clock_data |= 0b1100;
						//set_PINX_variable_output(6, E, 1);
						//init_pins();
						break;
					}
				//	// this target is confirmed
				//	//set_PINX_variable_output(6, E, 1);
				}
				else if(counter != 130)
				{
					set_PINX_variable_output(6, C, 1);
					clock_data |= (0b10 & clock_data) << 5;
					clock_data |= (received[counter - 30] ^ ((0b10 & clock_data) >> 1)) << 7;
					pressed_counter += (0b10 & clock_data) >> 1;
					//clock_data |= 0x80;
					received[counter - 30] = ((0b10 & clock_data) >> 1);
				}
				if(counter == 131)
				{
					counter = 0;
					//if((clock_data & 0b1000011) == 0b1000011)
					if(clock_data & 0b10)
					{set_PINX_variable_output(7, D, 1); break;}
					else
					{
						clock_data &= 0xCF;
						clock_data |= 0b1100;
						continue;
					}
				}
				if(counter == 0)
				{
					last_pressed_count = pressed_counter;
					pressed_counter = 0;
				}
			case 0b010001:
				clock_data |= 0b100;
				counter++;
			default:
				continue;
		}

		// if no changes in currently pressed keys
		if((clock_data & 0x80) == 0)
		{continue;}

		// if a keypress was detected
		if((clock_data & 0x40) == 0x40)
		{solenoid |= 0b10; solenoid &= 0b11;}

		if(received[SOLENOID_DISABLE])
		{
			solenoid_toggle();
			//_delay_ms(500);
			continue;
		}

		for(; counter < 100; counter++)
		//for(uint8_t x = 0; x < 100; x++)
		{
			set_PINX_variable_output(6, E, received[counter]);
			struct keypress target = {keymap[counter], counter, counter};
			kbd_press_release(received[counter], target);
		}
		counter = 0;

		//solenoid = 0b11;
		if((solenoid & 0b111) == 0b11
		&& pressed_counter >= (last_pressed_count + 1))
		{
			set_PINX_variable_output(6, F, 1);
			set_PINX_variable_output(7, F, 1);
			//_delay_us(0);
			set_PINX_variable_output(7, F, 0);
			set_PINX_variable_output(6, F, 0);
			//solenoid = 0b101;
			solenoid |= 0b100;
			solenoid &= 0b101;
		}
		//continue;

		kbsend();
		reset_sending();
	}

	return 1;
}
