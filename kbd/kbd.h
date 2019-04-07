#include "base.h"
#include "../promicro/pinlogic.h"
#include "../promicro/promicro.h"
#include "../usb_keyboard/usb_keyboard.h"

#include "../LAYOUT_SELECT.h"

///////////////
//USEFUL INFO//
///////////////
//		pro micro pin layout -> https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro16MHzv1.pdf
//
//		demultiplexer datasheet -> https://html.alldatasheet.com/html-pdf/27373/TI/SN74159N/22/1/SN74159N.html
//								OR
//								search for SN74159N

static uint8_t DEMUX_BITS = 1;

//const pin definitions not required for PORTF as it can be set using bit operations
//any other pin layout needs to have pin definitions
//
//static const struct pin demux_pins[DEMUX_IN] = {{B, 1}, {B, 3}, {B, 2}, {B, 6}};//A, B, C, D (pins 15, 14, 16, 10)
//static const struct pin demux_pins[DEMUX_IN] = {{F, 4}, {F, 5}, {F, 6}, {F, 7}};//A, B, C, D (pins A0, A1, A2, A3)

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

//this array is for the keys that were defined as key proxies (see usb_keyboard/keys.h)
static volatile uint8_t keyproxy_lookup[11] = {
	39,
	30,
	31,
	32,
	33,
	34,
	35,
	36,
	37,
	38,
	55
};

static volatile struct kbstate kbd = {0, 0, 0, 0};//last state, current state, row state, has state changed
static volatile uint8_t ispressed[COLUMNS];
uint8_t flash_led(void);

uint8_t row;//the current row
struct kblayer_key *layer;//the current layer
static volatile uint8_t scanvalue = 0;
static volatile uint8_t readthepin = 0;

//predefined keys
struct keystate default_state = {0xFF, 0xFF, 0};//can't match any matrix so will never be pressed
const static struct keystate SHIFTR = {11, 5, 1};
const static struct keystate SHIFTL = {3, 6, 1};
const static struct keystate standby_sw = {4, 7, 0};
const static struct keystate keypad_00 = {6, 7, 0};
const static struct keystate volume_up = {12, 7, 0};
const static struct keystate volume_down = {12, 0, 0};

//layer keys, when pressed will activate a target layer
struct kblayer_key layer_keys[2] = {
	{0, {11, 7, 0}},//primary layer
	{1, {15, 6, 0}}//secondary layer
};

//determines if a key is SHIFTL or SHIFTR
uint8_t shiftkey(struct keystate key)
{
	if(key.row == SHIFTL.row
	&& key.column == SHIFTL.column)
	{return 1;}
	if(key.row == SHIFTR.row
	&& key.column == SHIFTR.column)
	{return 1;}
	return 0;
}

//determines if a key is vol up/down and sends their value (1 = up / 2 = down)
uint8_t volumekey(struct keystate key)
{
	if(key.row == volume_up.row
	&& key.column == volume_up.column)
	{return 1;}
	if(key.row == volume_down.row
	&& key.column == volume_down.column)
	{return 2;}
	return 0;
}

static volatile struct keystate currently_pressing[COLUMNS];//array of all currently pressed keys and their keystate info
static volatile uint8_t previous_presses[ROWS][COLUMNS];//an array to remember what was last pressed, 1 is equal to changed state, 0 is no change

uint8_t get_layer_key(uint8_t layer_num, uint8_t row, uint8_t column);
void layer_select(void);
void press_release(void);
void setup_keys(void);//will assign pressed keys to currently_pressing array
void reset_keys(void);

int main(void);

////////////////////////////////////
//define user functions below here//
////////////////////////////////////

//optional user functions for execution
#define __USER//comment this out if you don't want to use any user functions
#ifdef __USER
void functions(void);//this determines what user functions to execute, add custom key logic to this

static uint8_t standby = 0;
static struct keystate shiftcaps;

static void inline standby_switch(void)
{
	standby ^= 1;//0 <--> 1
	_delay_ms(250);//creates a gap so its hard to toggle it repeatedly
	return;
}
static void inline send_00(void)
{
	usb_keyboard_press(KEY_0, 0);
	_delay_ms(5);
	usb_keyboard_press(KEY_0, 0);
	_delay_ms(70);//delay to prevent spam - a keypress can be upwards of 120ms, but generally around 80ms
	return;
}
static void inline capslock(void)//will enable capslock when both SHIFTL and SHIFTR pressed
{
	usb_keyboard_press(KEY_CAPS_LOCK, 0);
	_delay_ms(110);
	return;
}
static void inline volume(const uint8_t UPorDOWN)
{
	if(UPorDOWN == 1)
	{usb_extra_press(VOL_UP);}
	if(UPorDOWN == 2)
	{usb_extra_press(VOL_DOWN);}
	_delay_ms(70);
	return;
}
#endif
