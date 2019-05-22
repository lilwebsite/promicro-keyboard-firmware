//driver is selected by the compiler, check the make file for DRIVER

#include "../promicro/pinlogic.h"

#include "SN74159N.h"

inline void init_driver(void)
{
	#ifdef SN74159N
	init_SN74159N();
	#endif
}

inline void set_row(uint8_t row)
{
	//PORTF = u4_reverse(row)<<4;//sets demux line, u4 switches from LSB to MSB for unsigned 4bit numbers
	#ifdef SN74159N
	scan_SN74159N(row);
	#endif
}