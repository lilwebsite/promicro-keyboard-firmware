#include <kbd/globals.h>
#include <interfaces/usb/keys.h>

const uint8_t PROGMEM keymap[KEYS] = KEYMAP_POS(
	                    TILDE, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,

	KEY_F13, KEY_F14,   KEY_ESC, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, HYPHEN, EQUAL, BACKSPACE,       HOME,   END,        NUMLOCK,  KP_SLH,   KP_AST,   KP_MIN,
	KEY_F15, KEY_F16,   KEY_TAB,   KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, L_BRACE, R_BRACE, BACKSLASH,  DELETE, PRTSCN,     KEYPAD_7, KEYPAD_8, KEYPAD_9, KP_PLS,
	KEY_F17, KEY_F18,   L_CTRL,     KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, SEMICOLON, QUOTE, PAUSE, ENTER,     PAGE_UP,PAGE_DN,    KEYPAD_4, KEYPAD_5, KEYPAD_6, KP_COM,
	KEY_F19, KEY_F20,   L_SHIFT,      L_ALT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, COMMA, PERIOD, SLASH, R_SHIFT,           UP,     DOWN,       KEYPAD_1, KEYPAD_2, KEYPAD_3, KP_ENT,
	KEY_F21, KEY_F22,   L_GUI,                              SPACE,                                           R_ALT, R_CTRL,            LEFT,   RIGHT,      KEYPAD_0,           KP_PRD
);
