#include <keyboard.h>

void init(void)
{
	DRIVER_ROWS = 0xF;

	//output pins for the decoder
	pin_output(F, 7); // pin A0
	pin_output(F, 6); // pin A1
	pin_output(F, 5); // pin A2
	pin_output(F, 4); // pin A3

	twi_init();
}

void set(uint8_t row)
{
	uint8_t ABCD[4];

	ABCD[0] = (0x8 & row) >> 3;
	ABCD[1] = (0x4 & row) >> 2;
	ABCD[2] = (0x2 & row) >> 1;
	ABCD[3] = (0x1 & row);

	for(uint8_t x = 0; x < DEMUX_IN; x++)
	{pin_value(demux_pins[x].port, demux_pins[x].position, ABCD[x]);}
	_delay_us(35);

	usb_debug_write(0xAA);
	usb_debug_write(ABCD[0]);
	usb_debug_write(ABCD[1]);
	usb_debug_write(ABCD[2]);
	usb_debug_write(ABCD[3]);
	usb_debug_flush();

	return;
}

void scan(void)
{
	uint8_t x;

	for(row = 0; row <= DRIVER_ROWS; row++)
	{
		kbd.row_state = 0;
		pin_state = 0;

		set(row);

		for(x = 0; x < COLUMNS; x++)
		{
			pin_state = read_pin(input_pins[x].port, input_pins[x].position);

			if(pin_state)
			{
				ispressed[x] = HIGH;
				continue;
			}
			else
			{ispressed[x] = LOW;}

			usb_debug_write(0xEE);
			usb_debug_write(row);
			usb_debug_write(x);

			if(previous_presses[row][x] == ispressed[x])
			{kbd.row_state = 1;}
		}

		if(!kbd.state && kbd.row_state)
		{kbd.state = 1;}

		setup_keys();

		layer_select();

		press_release();

		_delay_us(35);
	}
}
