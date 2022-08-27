#include "keyboard.h"

void init(void)
{
	twi_init();

	ABANK_DIR.data = 0; // set output mode on all pins
	twi_action(&ABANK_DIR);

	BBANK_DIR.data = 0x3F; // set input mode on all except last 2
	twi_action(&BBANK_DIR);

	ABANK_GPPU.data = 0xFF; // enable pullup
	twi_action(&ABANK_GPPU);

	BBANK_GPPU.data = 0xFF;
	twi_action(&BBANK_GPPU);

	return;
}

void set(uint8_t row)
{
	return;
}
