#include "IBMPingmaster.h"

uint8_t init_inputs(void)
{
	uint8_t pullup = 1;
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		set_PINX_output(input_pins[x].position, input_pins[x].port);
		set_PINX_input(input_pins[x].position, input_pins[x].port, pullup);
	}

	return 1;
}

uint8_t init_pins(void)
{
	//pins are based on a pro micro in order
	//output pins for the decoder
	set_PINX_output(4, F);//pin A0
	set_PINX_output(5, F);//pin A1
	set_PINX_output(6, F);//pin A2
	set_PINX_output(7, F);//pin A3

	init_inputs();
	
	//USB
	set_PINX_input(3, D, 0);
	set_PINX_input(2, D, 0);
	
	//not used
	set_PINX_input(6, B, 1);//pin 10
	set_PINX_input(2, B, 1);//pin 16
	set_PINX_input(3, B, 1);//pin 14
	set_PINX_input(1, B, 1);//pin 15
	set_PINX_input(0, F, 0);
	set_PINX_input(1, F, 0);
	set_PINX_input(2, E, 0);
	set_PINX_input(5, D, 0);
	set_PINX_input(6, D, 0);
	set_PINX_input(7, C, 0);
	set_PINX_input(0, B, 0);
	set_PINX_input(7, B, 0);
	
	return 1;
}

uint8_t init_promicro(void)
{
	CPU_PRESCALE(CPU_16MHz);
	
	init_pins();
	
	return 1;
}