#include "../usb/usb.h"

extern volatile struct keystate currently_pressing[COLUMNS];
extern const struct kblayer_key *layer;
extern struct keystate default_state;
extern uint8_t received[130];

uint8_t overrides(void);
void functions(void);

#ifdef STANDBY
extern const struct keystate standby_sw;
#endif
#ifdef ENABLE_SHIFT_CAPSLOCK
extern const struct keystate SHIFTR;
extern const struct keystate SHIFTL;
#endif
#ifdef ENABLE_00
extern const struct keystate keypad_00;
#endif
#ifdef ENABLE_VOLUME_KEYS
extern const struct keystate volume_up;
extern const struct keystate volume_down;
#endif
#ifdef ENABLE_PLAY_PAUSE
extern const struct keystate play_pause;
#endif
#ifdef ENABLE_TRACK_KEYS
extern const struct keystate prev_track;
extern const struct keystate next_track;
#endif
#ifdef ENABLE_RESET
extern const struct keystate reset_key;
#endif
#ifdef UNDO_BUTTON
extern const struct keystate undo_key;
#endif
#ifdef REDO_BUTTON
extern const struct keystate redo_key;
#endif
#ifdef COPY_BUTTON
extern const struct keystate copy_key;
#endif
#ifdef CUT_BUTTON
extern const struct keystate cut_key;
#endif
#ifdef PASTE_BUTTON
extern const struct keystate paste_key;
#endif
#ifdef SKIP_BUTTON
extern const struct keystate skip_key;
#endif

static uint8_t standby = 0;
static struct keystate shiftcaps;
