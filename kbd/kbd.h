#include "globals.h"
#include "user.h"
#include "../promicro/promicro.h"
#include "../usb/usb.h"

extern uint8_t reset;

extern void functions(void);

extern uint8_t init_promicro(void);
extern void usb_init(void);
extern uint8_t usb_configured(void);
extern void init(void);
extern void set(uint8_t row);
extern void scan(void);
extern void reset_keys(void);
extern void press_release(void);
extern const struct pin input_pins[COLUMNS];
extern volatile struct kbstate kbd;

#define ENABLE_LAYERS

int main(void);
