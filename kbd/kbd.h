#include "base.h"
#include "../promicro/pinlogic.h"
#include "../promicro/promicro.h"
#include "../usb_keyboard/usb_keyboard.h"

#include "../LAYOUT_SELECT.h"

volatile uint8_t DRIVER_ROWS = 1; //driver rows is used by whichever chip we are driving the board with
#include "../drivers/driver.h"

#define ENABLE_LAYER_TOGGLE
//#define ENABLE_LAYER_KEYS
#if defined ENABLE_LAYER_KEYS || defined ENABLE_LAYER_TOGGLE
#define __LAYERS
#endif

static volatile struct kbstate kbd = {0, 0, 0, 0};//last state, current state, row state, has state changed
static volatile uint8_t ispressed[COLUMNS];
uint8_t flash_led(void);

uint8_t row;//the current row
struct kblayer_key *layer;//the current layer
struct kblayer_key *last_layer;//points to the last layer when get_last_layer is called
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
const static struct keystate prev_track = {11, 7, 0};
const static struct keystate next_track = {10, 7, 0};
const static struct keystate play_pause = {15, 6, 0};

//layer keys, when pressed will activate a target layer
struct kblayer_key layer_keys[2] = {
	{0, {11, 7, 0}},//primary layer
	{1, {15, 6, 0}}//secondary layer
};

//layer cycle, will iterate through the specified layers
struct kblayer_cycle cycle = {
	{0, {15, 7, 0}},//layer cycle key
	LAYERS,//amount of layers to iterate
	0,//the layer to start on, in reference to the array
	{
		0,//primary layer
		1//secondary layer
	}
};

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

//determines if a key is vol up/down and sends their value (1 = up / 2 = down)
uint8_t volume_key(struct keystate key)
{
	if(key.row == volume_up.row
	&& key.column == volume_up.column)
	{return 1;}
	if(key.row == volume_down.row
	&& key.column == volume_down.column)
	{return 2;}
	return 0;
}

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

static volatile struct keystate currently_pressing[COLUMNS];//array of all currently pressed keys and their keystate info for the current column
static volatile uint8_t previous_presses[ROWS][COLUMNS];//an array to remember what was last pressed, 1 is equal to changed state, 0 is no change

inline void update_layers(struct kblayer_key *lk)
{
	//update the layer keys
	last_layer->key.pressed = 0;
	lk->key.pressed = 1;
	layer = lk;//end with setting the current layer to what was pressed
	return;
}
inline void get_last_layer(void)
{
	#ifdef ENABLE_LAYER_KEYS
	for(uint8_t x = 0; x < LAYERS; x++)
	{
		if(layer_keys[x].key.pressed)
		{last_layer = &layer_keys[x]; break;}//we wana work with this layer as last layer
	}
	#endif
	#ifdef ENABLE_LAYER_TOGGLE
	if(cycle.lk.key.pressed)
	{last_layer = &cycle.lk;}
	#endif
	return;
}
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

static uint8_t standby = 0;
static struct keystate shiftcaps;

static inline void standby_switch(void)
{
	standby ^= 1;//0 <--> 1
	_delay_ms(250);//creates a gap so its hard to cycle it repeatedly
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
#endif
