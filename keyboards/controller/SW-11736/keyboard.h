#include <kbd/globals.h>
#include <kbd/types.h>
#include <interfaces/usb/types.h>
#include <interfaces/twi/devices/PCA9535.h>
#include <settings.h>
#include <matrix.h>

static uint8_t direct = 0; // switches which are directly connected
static uint8_t EMD = 0; // if a strong magnetic field is detected this disables the keyboard

uint8_t received[KEYS];

extern struct kbstate kbd;
extern struct usb_state usb;
extern void kbd_press_release(uint8_t press, struct keypress key);
extern void reset_sending(void);

extern void twi_init(void);
