//driver is selected by the compiler, check the make file for DRIVER

#include "../promicro/pinlogic.h"

#ifdef IBMPingmaster
#include "SN74159N.h"
#endif
#ifdef PC8801
#include "PC8801.h"
#endif

inline void init_driver(void)
{
	#ifdef IBMPingmaster
	init_SN74159N();
	#endif
	#ifdef PC8801
	init_PC8801();
	#endif
}

inline void set_row(uint8_t row)
{
	//PORTF = u4_reverse(row)<<4;//sets demux line, u4 switches from LSB to MSB for unsigned 4bit numbers
	#ifdef IBMPingmaster
	scan_SN74159N(row);
	#endif
	#ifdef PC8801
	scan_PC8801(row);
	#endif
}
