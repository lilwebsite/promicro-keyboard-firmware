#include <kbd/globals.h>
#include <kbd/types.h>
#include <settings.h>

void scan(void);

//A, B, C, D (pins A3, A2, A1, A0)
const struct pin demux_pins[DEMUX_IN] = {{F, 4}, {F, 5}, {F, 6}, {F, 7}};

const struct pin input_pins[COLUMNS] = 
{
	{B, 5}, // pin 9
	{B, 4}, // pin 8
	{E, 6}, // pin 7
	{D, 7}, // pin 6
	{C, 6}, // pin 5
	{D, 4}, // pin 4
	{D, 0}, // pin 3
	{D, 1}  // pin 2
};

//SN74159N is a 4 to 16 line demultiplexer

///////////////
//USEFUL INFO//
///////////////
//		pro micro pin layout -> https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro16MHzv1.pdf
//
//		demultiplexer datasheet -> https://html.alldatasheet.com/html-pdf/27373/TI/SN74159N/22/1/SN74159N.html
//		OR
//		search for SN74159N

//scanvalues used to check each row 0 - 15 (for a 4 to 16 line demultiplexer)
//DCBA
//0000 - x . . . . . . . . . . . . . . .
//0001 - . x . . . . . . . . . . . . . .
//0010 - . . x . . . . . . . . . . . . .
//0011 - . . . x . . . . . . . . . . . .
//0100 - . . . . x . . . . . . . . . . .
//0101 - . . . . . x . . . . . . . . . .
//0110 - . . . . . . x . . . . . . . . .
//0111 - . . . . . . . x . . . . . . . .
//1000 - . . . . . . . . x . . . . . . .
//1001 - . . . . . . . . . x . . . . . .
//1010 - . . . . . . . . . . x . . . . .
//1011 - . . . . . . . . . . . x . . . .
//1100 - . . . . . . . . . . . . x . . .
//1101 - . . . . . . . . . . . . . x . .
//1110 - . . . . . . . . . . . . . . x .
//1111 - . . . . . . . . . . . . . . . x

#define SCANVALUES \
{\
	{0, 0, 0, 0}\
	{0, 0, 0, 1}\
	{0, 0, 1, 0}\
	{0, 0, 1, 1}\
	{0, 1, 0, 0}\
	{0, 1, 0, 1}\
	{0, 1, 1, 0}\
	{0, 1, 1, 1}\
	{1, 0, 0, 0}\
	{1, 0, 0, 1}\
	{1, 0, 1, 0}\
	{1, 0, 1, 1}\
	{1, 1, 0, 0}\
	{1, 1, 0, 1}\
	{1, 1, 1, 0}\
	{1, 1, 1, 1}\
}

extern uint8_t overrides(void);
extern void setup_keys(void);
extern void press_release(void);
extern void layer_select(void);
extern volatile uint8_t ispressed[COLUMNS];
extern volatile uint8_t previous_presses[ROWS][COLUMNS];
extern volatile struct kbstate kbd;
extern const struct pin input_pins[COLUMNS];
extern uint8_t row;

extern void usb_debug_write(uint8_t data);
extern void usb_debug_flush(void);

static volatile uint8_t pin_state;

volatile uint8_t DRIVER_ROWS;
