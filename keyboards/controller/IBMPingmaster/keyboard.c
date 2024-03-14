#include <keyboard.h>

void init(void)
{
	//DRIVER_ROWS = 0b1111;
	DRIVER_ROWS = 0xF;
	//output pins for the decoder
	pin_output(F, 7); // pin A0
	pin_output(F, 6); // pin A1
	pin_output(F, 5); // pin A2
	pin_output(F, 4); // pin A3

	for (uint8_t p = 0; p < COLUMNS; p++)
	{
		pin_output(input_pins[p].port, input_pins[p].position);
		pin_input(input_pins[p].port, input_pins[p].position, 1);
	}

	// not used
	pin_input(B, 6, 0); // pin 10
	pin_input(B, 2, 0); // pin 16
	pin_input(B, 3, 0); // pin 14
	pin_input(B, 1, 0); // pin 15
	pin_input(D, 3, 0); // pin TXO
	pin_input(D, 2, 0); // pin RXI

	pin_input(B, 0, 0); // RX LED
	pin_input(D, 5, 0); // TX LED

	// physically not connected
	pin_input(F, 0, 0);
	pin_input(F, 1, 0);
	pin_input(E, 2, 0);
	pin_input(D, 6, 0);
	pin_input(C, 7, 0);
	pin_input(B, 7, 0);
}

void set(uint8_t row)
{
	uint8_t ABCD[4];
	ABCD[0] = (0x8 & row) >> 3;
	ABCD[1] = (0x4 & row) >> 2;
	ABCD[2] = (0x2 & row) >> 1;
	ABCD[3] = (0x1 & row);
	//ABCD[0] = (0b1000 & row)>>3;
	//ABCD[1] = (0b0100 & row)>>2;
	//ABCD[2] = (0b0010 & row)>>1;
	//ABCD[3] = 0b0001 & row;
	for(uint8_t x = 0; x < DEMUX_IN; x++)
	{pin_value(demux_pins[x].port, demux_pins[x].position, ABCD[x]);}
	_delay_us(35);//>switching characteristics: from A, B, C (input) to any Y (output) - Vcc 4.5V, (TYP)18ns (MAX)36ns
	//				[programmer note]: this matches the switching characteristics of most demux chips
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

	for(row = 0; row <= DRIVER_ROWS; row++)//the amount of driver rows is decided by the selected driver in drivers/driver.h
	{
		//usb_debug_write(row);
		kbd.row_state = 0;
		pin_state = 0;//reads a pin bit

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
			{ispressed[x] = LOW;}//LOW == low state on pin(aka key press), HIGH == high state, or pullup(no press)
			//previous_presses is assigned the inverse of LOW(0) or HIGH(1), so if they are equal then it has changed state
			usb_debug_write(0xEE);
			usb_debug_write(row);
			usb_debug_write(x);
			if(previous_presses[row][x] == ispressed[x])
			{kbd.row_state = 1;}
		}

		if(!kbd.state && kbd.row_state)
		{kbd.state = 1;}

		//figure out what is being pressed in the row
		read_row();

		layer_select();

//		if(!overrides())
//		{continue;}
//
		press_release();

		_delay_us(35);//>switching characteristics: from A, B, C (input) to any Y (output) - Vcc 4.5V, (TYP)18ns (MAX)36ns
		//				[programmer note]: this matches the switching characteristics of most demux chips
	}
}
