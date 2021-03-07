extern volatile struct keystate currently_pressing[COLUMNS];
extern const struct kblayer_key *layer;

uint8_t overrides(void);
void functions(void);

extern const struct keystate standby_sw;
static uint8_t standby = 0;

#ifdef ENABLE_SHIFT_CAPSLOCK
static struct keystate shiftcaps;
#endif
