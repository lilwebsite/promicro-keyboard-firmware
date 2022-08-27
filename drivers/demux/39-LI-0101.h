//#include <drivers/twi/twi.h>
#include <kbd/globals.h>
#include <usb/usb.h>
#include <drivers/twi/globals.h>
#include <layout.h>

// these will be bitfields
// we need 9 outputs but output 1 can be ignored
uint8_t output_lines = 0;
// input data is a bitfield
uint8_t data = 0;
// you can calculate this but no point since it isn't large
uint8_t interval_length[8] = {1, 2, 4, 8, 16, 32, 64, 64};

uint8_t received[KEYS];
static uint8_t state_changed = 0;
//static uint8_t last_pressed_count = 0;
//static uint8_t pressed_counter = 0;

struct twi_target ABANK_GPIO = SET_TARGET(GPIOA, READ);
struct twi_target BBANK_GPIO = SET_TARGET(GPIOB, READ);

struct twi_target ABANK_OLAT = SET_TARGET(GPIOA, WRITE);
struct twi_target BBANK_OLAT = SET_TARGET(GPIOB, WRITE);

struct twi_target BANK_IOCON = SET_TARGET(IOCONA, WRITE);

extern uint8_t twi_action(struct twi_target *action);

extern struct pin pin_feedback;
extern struct pin pin_data;
extern struct pin pin_in1;
extern struct pin pin_in2;
extern struct pin pin_in3;

//extern volatile struct kbstate kbd;

static volatile uint8_t pin_state;

void scan(void);
