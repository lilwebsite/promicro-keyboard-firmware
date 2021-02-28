#include "scanners/protocol/ITT.h"

static uint8_t data_lines;
static uint8_t counter;
static uint8_t last_pressed_count;
static uint8_t pressed_counter;

// the following is a binary mapping of the clock_data variable
// this makes effective usage out of one variable to speed up the scanner
// 0000 0001 -> clock bit
// 0000 0010 -> data bit
// 0000 0100 -> clock read
// 0000 1000 -> data read
// 0001 0000 -> candidate bit
// 0010 0000 -> found bit
// 0100 0000 -> keypress detected
// 1000 0000 -> output state changed
static uint8_t clock_data;

// 0001 -> solenoid enable
// 0010 -> trigger solenoid
// 0100 -> solenoid triggered
static uint8_t solenoid;

static inline void solenoid_toggle(void)
{
	solenoid ^= 0b1;
	solenoid |= 0b100;
	solenoid &= 0b101;
	return;
}

void init_ITT(void)
{
	data_lines = 0;
	counter = 0;
	last_pressed_count = 0;
	pressed_counter = 0;
	clock_data = 0;
	solenoid = 1; // enables the solenoid
	//solenoid = 0; // disables the solenoid
	return;
}
