void functions(void);

#ifdef ENABLE_STDBY
static uint8_t standby = 0;
#endif

#ifdef ENABLE_SHIFT_CAPSLOCK
static struct keystate shiftcaps;
