#include "../kbd/base.h"
#include "pinlogic.h"

// processor frequency (from <http://www.pjrc.com/teensy/prescaler.html>)
#define  CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define  CPU_16MHz			0x00
#define  CPU_8MHz			0x01
#define  CPU_4MHz			0x02
#define  CPU_2MHz			0x03
#define  CPU_1MHz			0x04
#define  CPU_500kHz			0x05
#define  CPU_250kHz			0x06
#define  CPU_125kHz			0x07
#define  CPU_62kHz			0x08

static const struct pin input_pins[COLUMNS] = 
{
	{B, 5},//9
	{B, 4},//8
	{E, 6},//7
	{D, 7},//6
	{C, 6},//5
	{D, 4},//4
	{D, 0},//3
	{D, 1} //2
};

uint8_t init_promicro(void);
uint8_t init_pins(void);
uint8_t init_inputs(void);