#include "debug.h"

void leddebug(uint8_t val, uint8_t override)
{
	if (!override)
	{
		if (val == debug_point) {return;}
		if (val > debug_point) {debug_point = val;}
	}
	else
	{debug_point = val;}
	PORTF = 0xF0 & debug_point;
	DDRF = 0xF0;
	PORTD = ((0x2 & debug_point) >> 1) | ((0x1 & debug_point) << 1) | ((0x4 & debug_point) << 2);
	DDRD = 0x13;
	PORTC = (0x8 & debug_point) << 3;
	DDRC = 0x40;
}
