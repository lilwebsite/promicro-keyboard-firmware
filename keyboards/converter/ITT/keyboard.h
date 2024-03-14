#include <kbd/globals.h>
#include <kbd/types.h>
#include <settings.h>
#include <matrix.h>

static uint8_t counter = 0;
static uint8_t last_pressed_count = 0;
static uint8_t pressed_counter = 0;
static uint8_t received[KEYS];

//// the following is a binary mapping of the clock_data variable
//// this makes effective usage out of one variable to speed up the scanner
//// 0000 0001 -> clock bit
//// 0000 0010 -> data bit
//// 0000 0100 -> clock read
//// 0000 1000 -> data read
//// 0001 0000 -> candidate bit
//// 0010 0000 -> found bit
//// 0100 0000 -> keypress detected
//// 1000 0000 -> output state changed
static uint8_t clock_data;

//// 0001 -> solenoid enable
//// 0010 -> trigger solenoid
//// 0100 -> solenoid triggered
static uint8_t solenoid = 0b0;

//void solenoid_toggle(void);
extern struct kbstate kbd;
extern void kbd_press_release(uint8_t press, struct keypress key);
extern void reset_sending(void);

const struct pin ITT_CLOCK = {F, 4};
const struct pin ITT_EN = {F, 6};
const struct pin ITT_SOLENOID = {F, 5};
const struct pin ITT_DATA = {F, 7};
