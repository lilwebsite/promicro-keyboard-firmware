#include <kbd/globals.h>
#include <kbd/types.h>
#include <matrix.h> // matrix file in keyboard folder
#include <settings.h>

extern struct kbstate kbd; // last state, current state, row state, has state changed
volatile uint8_t ispressed[COLUMNS];

struct kblayer
{
	// whenever this type is accessed the program will know what characters belong to which layer
	const uint8_t layer;
	const uint8_t matrix[ROWS][COLUMNS];
};

void reset_keys(void);

struct keystate default_state = {0xFF, 0xFF, 0}; // can't match any matrix so will never be pressed

uint8_t row; // the current row
extern const struct kblayer PROGMEM kblayer_list[];
const struct kblayer_key base_layer = {0, 0, {0xFF, 0xFF, 0}}; // the base layer for reference
const struct kblayer_key *layer = &base_layer; // points to the current layer

volatile struct keystate currently_pressing[COLUMNS]; // array of all currently pressed keys and their keystate info for the current column
volatile uint8_t previous_presses[ROWS][COLUMNS]; // an array to remember what was last pressed, 1 is equal to changed state, 0 is no change

void read_row(void); // will assign pressed keys to currently_pressing array

uint8_t get_layer_key(uint8_t layer_num, uint8_t row, uint8_t column);
void layer_select(void);
void layer_cycle(void);

void press_release(void);

// usb functions and variables
extern struct keypress keyboard_keys[KEY_ROLLOVER];
extern void kbd_press_release(uint8_t press, struct keypress key);
