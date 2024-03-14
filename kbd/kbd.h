#include "globals.h"
#include "user.h"

uint8_t device_reset;

struct kbstate kbd = {0, 0, 0, 0};

extern void usb_init(void);
extern void init(void);

int main(void);
