#include "kbd.h"
#include "matrix.h"
#include "user.h"

int main(void)
{
	while(!init_promicro());
	usb_init();
	while(!usb_configured());

	init_driver();

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
		#ifndef PROTOCOL
		set_row();
		#endif

		scan_signal();

		kbsend();
		reset_sending();
	}

	return 1;
}
