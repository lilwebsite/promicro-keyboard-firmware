#include <kbd/globals.h>
#include "promicro.h"

uint8_t init_inputs(const struct pin *input_pins)
{
	uint8_t pullup = 1;
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		set_PINX_output(input_pins[x].position, input_pins[x].port);
		set_PINX_input(input_pins[x].position, input_pins[x].port, pullup);
	}

	return 1;
}

uint8_t init_promicro(void)
{
	CPU_PRESCALE(CPU_16MHz);

	init_pins();

	return 1;
}
