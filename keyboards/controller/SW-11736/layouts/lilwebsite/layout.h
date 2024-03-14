#include <kbd/globals.h>
#include <interfaces/usb/keys.h>

#define KP_COM KEYPAD_COMMA

#define LAYERS 1//edit this if more layers are added

const uint8_t PROGMEM keymap[KEYS] = KEYMAP_POS(
	KEY_1,   KEY_2,   KEY_3,   KEY_4,   KEY_5,   KEY_6,   KEY_7,   KEY_8,   KEY_9,   KEY_0,   HYPHEN,  KEY_EQUAL, BACKSPACE,
	KEY_TAB, KEY_Q,   KEY_W,   KEY_E,   KEY_R,   KEY_T,   KEY_Y,   KEY_U,   KEY_I,   KEY_O,   KEY_P,   L_BRACE,   R_BRACE,
	L_CTRL,  KEY_A,   KEY_S,   KEY_D,   KEY_F,   KEY_G,   KEY_H,   KEY_J,   KEY_K,   KEY_L,   KEY_SEMI,KEY_QUOTE, BACKSLASH, KEY_ENTER,
	L_SHIFT, KEY_Z,   KEY_X,   KEY_C,   KEY_V,   KEY_B,   KEY_N,   KEY_M,   COMMA,   PERIOD,  SLASH,   R_SHIFT,
	SPACE
);
