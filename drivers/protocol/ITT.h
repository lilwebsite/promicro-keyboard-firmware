#include <kbd/globals.h>
#include <usb/usb.h>

static volatile uint8_t received[100];
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
static uint8_t clock_data = 0;

// 0001 -> solenoid enable
// 0010 -> trigger solenoid
// 0100 -> solenoid triggered
static uint8_t solenoid = 1;

// if set to > 1 then send keypress data over USB
static uint8_t send = 0;

//__attribute__((always_inline)) void scan(void);
void scan(void);
extern void solenoid_toggle(void);
extern uint8_t init_pins(void);
extern uint8_t overrides(void);
