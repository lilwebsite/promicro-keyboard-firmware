#include <kbd/globals.h>
#include <usb/keys.h>

#define LAYERS 1//edit this if more layers are added
const uint8_t keymap[KEYS] = {
	// 1        2          3          4          5          6          7          8          9          10          11          12
	KEY_1,      KEYPAD_ENT,KEYPAD_PLS,KEYPAD_AST,KEYPAD_SLH,KEY_1,     KEYPAD_7,  KEYPAD_1,  KEYPAD_4,  KEYPAD_0,   KEYPAD_MIN, KEYPAD_PRD,
	KEYPAD_3,   KEYPAD_6,  KEYPAD_9,  KEYPAD_COM,KEYPAD_8,  KEY_1,     KEYPAD_2,  KEYPAD_5,  KEY_1,     KEY_1,      KEY_1,      KEY_1,
	KEY_1,      KEY_1,     KEY_1,     KEY_1,     KEY_1,     KEY_1,     KEY_F12,   KEY_F11,   KEY_F10,   KEY_F9,     KEY_F8,     KEY_F7,
	KEY_F6,     KEY_F5,    KEY_F3,    KEY_F2,    KEY_ESC,   KEY_EQUAL, RBRACE,    KEY_MINUS, KEY_0,     KEY_1,      KEY_6,      KEY_4,
	KEY_2,      KEY_F4,    KEY_F1,    KEY_1,     BACKSPACE, KEY_1,     LBRACE,    KEY_9,     KEY_8,     KEY_7,      KEY_5,      KEY_3,
	KEY_1,      KEY_1,     KEY_1,     BACKSLASH, KEY_ENTER, KEY_P,     KEY_O,     KEY_U,     KEY_1,     KEY_T,      KEY_E,      KEY_Q,
	KEY_TILDE,  KEY_1,     PAGE_UP,   RSHIFT,    KEY_QUOTE, KEY_SEMI,  KEY_I,     KEY_Y,     KEY_R,     KEY_W,      KEY_1,      KEY_TAB,
	KEY_1,      KEY_HOME,  KEY_UP,    KEY_SLASH, KEY_L,     KEY_J,     KEY_G,     KEY_1,     KEY_D,     KEY_A,      KEY_1,      KEY_1,        
	DEL,        PAGE_DOWN, RIGHT_ALT, KEY_PERIOD,KEY_K,     KEY_H,     KEY_F,     KEY_S,     KEY_Z,     LCTRL,      KEY_1,      KEY_END,
	KEY_DOWN,   KEY_LEFT,  KEY_1,     KEY_COMMA, KEY_N,     KEY_V,     KEY_X,     LEFT_ALT,  LSHIFT,    KEY_1,      KEY_INSERT, KEY_RIGHT,
	RCTRL,      KEY_SPACE, KEY_M,     KEY_B,     KEY_C,     KEY_1,     0,         0
};

// key map as seen above, uppercase matches text on keys literally
