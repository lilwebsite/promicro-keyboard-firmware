#include "base.h"
#include "../promicro/pinlogic.h"
#include "../usb_keyboard/usb_keyboard.h"

#ifdef KBD_IBMPingmaster
//the purpose of the keyboard matrix is to tell the layout where the keys are positioned in the rows / columns
#include "../kbmatrix/IBMPingmaster.h"//selected keyboard matrix
//the purpose of the layout is to define layers and what keycode to send when pressed, needs to match the included matrix
#include "../layouts/IBMPingmaster/bigwebsite_layout.h"//selected key layout
//pin configuration file
#include "../promicro/pinconfig/IBMPingmaster.c"
#endif 

#ifdef KBD_PC8801
#include "../kbmatrix/PC8801.h"
#include "../layouts/PC8801/H0R1Z0N-layout.h"
#include "../promicro/pinconfig/PC8801.c"
#endif

#ifdef KBD_ITT
//#include "../layouts/ITT/Erderm-layout.h"
#include "../layouts/ITT/lilwebsite-layout.h"
#include "../promicro/pinconfig/ITT.c"
#endif

volatile uint8_t DRIVER_ROWS = 1; //driver rows is used by whichever chip we are driving the board with
#include "../drivers/driver.h"

#define ENABLE_LAYERS

static volatile struct kbstate kbd = {0, 0, 0, 0};//last state, current state, row state, has state changed
static volatile uint8_t ispressed[COLUMNS];
uint8_t flash_led(void);

struct keystate default_state = {0xFF, 0xFF, 0};//can't match any matrix so will never be pressed

uint8_t row;//the current row
const struct kblayer_key *layer;//points to the current layer
const static struct kblayer_key base_layer = {0, 0, {0xFF, 0xFF, 0}};//the base layer for reference
static volatile uint8_t readthepin = 0;

static volatile uint8_t received[100];


//predefined keys
#ifdef KBD_IBMPingmaster
#define __STDBY
const static struct keystate SHIFTR = {11, 5, 1};
const static struct keystate SHIFTL = {3, 6, 1};
const static struct keystate standby_sw = {4, 7, 0};
const static struct keystate keypad_00 = {6, 7, 0};
const static struct keystate volume_up = {12, 7, 0};
const static struct keystate volume_down = {12, 0, 0};
const static struct keystate prev_track = {11, 7, 0};
const static struct keystate next_track = {10, 7, 0};
const static struct keystate play_pause = {15, 6, 0};

const static struct kblayer_key layer_keys[LAYERS] = {
	base_layer,
	{1, 0, {15, 7, 0}}
};
#endif

#ifdef KBD_PC8801
const static struct keystate volume_up = {9, 3, 0};
const static struct keystate volume_down = {9, 2, 0};
const static struct keystate mute_key = {9, 1, 0};

struct kblayer_key layer_keys[LAYERS] = {
	base_layer,
	{1, 0, {8, 5, 0}},//secondary layer
	{2, 0, {9, 4, 0}}//tertiary layer
};
#endif

#ifdef KBD_IBMPingmaster
static struct keystate shiftcaps;
//determines if a key is SHIFTL or SHIFTR
uint8_t shift_key(struct keystate key)
{
	if((key.row == SHIFTL.row
	&& key.column == SHIFTL.column)
	|| (key.row == SHIFTR.row
	&& key.column == SHIFTR.column))
	{return 1;}
	return 0;
}
#endif

////determines if a key is vol up/down and sends their value (1 = up / 2 = down)
//uint8_t volume_key(struct keystate key)
//{
//	if(key.row == volume_up.row
//	&& key.column == volume_up.column)
//	{return 1;}
//	if(key.row == volume_down.row
//	&& key.column == volume_down.column)
//	{return 2;}
//	return 0;
//}

#ifdef KBD_IBMPingmaster
//determines if a key is a prev/next track key and sends their value (1 = prev / 2 = next)
uint8_t prevnext_key(struct keystate key)
{
	if(key.row == prev_track.row
	&& key.column == prev_track.column)
	{return 1;}
	if(key.row == next_track.row
	&& key.column == next_track.column)
	{return 2;}
	return 0;
}
#endif

static volatile struct keystate currently_pressing[COLUMNS];//array of all currently pressed keys and their keystate info for the current column
//static volatile uint8_t previous_presses[ROWS][COLUMNS];//an array to remember what was last pressed, 1 is equal to changed state, 0 is no change

uint8_t get_layer_key(uint8_t layer_num, uint8_t row, uint8_t column);
void layer_select(void);
void layer_cycle(void);

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

#ifdef __STDBY
static uint8_t standby = 0;
static inline void standby_switch(void)
{
	standby ^= 1;//0 <--> 1
	_delay_ms(250);//creates a gap so its hard to cycle it repeatedly
	return;
}
#endif

#define SOLENOID_DISABLE 67
static uint8_t solenoid = 1; // enables solenoid
//static uint8_t solenoid = 0; // disables solenoid
static inline void solenoid_toggle(void)
{
	solenoid ^= 0b1;
	solenoid |= 0b100;
	solenoid &= 0b101;
	return;
}

static inline void send_00(void)
{
	usb_keyboard_press(KEY_0, 0);
	_delay_ms(5);
	usb_keyboard_press(KEY_0, 0);
	_delay_ms(70);//delay to prevent spam - a keypress can be upwards of 120ms, but generally around 80ms
	return;
}
static inline void capslock(void)//will enable capslock when both SHIFTL and SHIFTR pressed
{
	usb_keyboard_press(KEY_CAPS_LOCK, 0);
	_delay_ms(110);
	return;
}
static inline void volume(const uint8_t UPorDOWN)
{
	if(UPorDOWN == 1)
	{usb_extra_press(VOL_UP);}
	if(UPorDOWN == 2)
	{usb_extra_press(VOL_DOWN);}
	_delay_ms(70);
	return;
}
static inline void play_pause_media(void)
{
	usb_extra_press(PLAY_PAUSE);
	_delay_ms(70);
	return;
}
static inline void next_prev_track(const uint8_t PREVorNEXT)
{
	if(PREVorNEXT == 1)
	{usb_extra_press(PREV_TRACK);}
	if(PREVorNEXT == 2)
	{usb_extra_press(NEXT_TRACK);}
	_delay_ms(70);
	return;
}
static inline void mute(void)
{
	usb_extra_press(MUTE);
	_delay_ms(70);
	return;
}
#endif
