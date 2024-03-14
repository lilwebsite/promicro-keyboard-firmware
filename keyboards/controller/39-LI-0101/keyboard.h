#include <kbd/globals.h>
#include <kbd/types.h>
#include <interfaces/usb/types.h>
#include <settings.h>
#include <matrix.h>

// bitfield, we need 9 outputs but 1 can be ignored (TWI)
uint8_t output_lines = 0;
// bitfield, input data (TWI)
uint8_t data = 0;
// you can calculate this in real time but no point since it isn't large
uint8_t interval_length[8] = {1, 2, 4, 8, 16, 32, 64, 64};

uint8_t received[KEYS];
uint8_t tactile_state = 0;

extern struct kbstate kbd;
extern struct usb_state usb;
extern void kbd_press_release(uint8_t press, struct keypress key);
extern void reset_sending(void);
