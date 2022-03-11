#include <kbd/globals.h>
#include <usb/keys.h>

const struct keystate prev_track = {0, 85, 0};
const struct keystate next_track = {0, 74, 0};
const struct keystate play_pause = {0, 62, 0};
const struct keystate redo_key = {0, 6, 0};
const struct keystate skip_key = {0, 52, 0};
const struct keystate undo_key = {0, 96, 0};
const struct keystate paste_key = {0, 107, 0};
const struct keystate copy_key = {0, 118, 0};
const struct keystate cut_key = {0, 129, 0};

#define LAYERS 1//edit this if more layers are added
const uint8_t keymap[COLUMNS] = {
	// 1		2			3			4			5			6			7			8			9			10			11			12
	0,			KEYPAD_ENT,	KEYPAD_PLS,	KEYPAD_AST,	KEYPAD_SLH,	0,			KEYPAD_7,	KEYPAD_1,	KEYPAD_4,	KEYPAD_0,	KEYPAD_MIN,	KEYPAD_PRD,
	KEYPAD_3,	KEYPAD_6,	KEYPAD_9,	KEYPAD_COM,	KEYPAD_8,	0,			KEYPAD_2,	KEYPAD_5,	0,			0,			0,			0,
	0,			0,			0,			0,			0,			0,			KEY_F12,	KEY_F11,	KEY_F10,	KEY_F9,		KEY_F8,		KEY_F7,
	KEY_F6,		KEY_F5,		KEY_F3,		KEY_F2,		KEY_ESC,	KEY_EQUAL,	RBRACE,		KEY_MINUS,	KEY_0,		0,			KEY_6,		KEY_4,
	KEY_2,		KEY_F4,		KEY_F1,		0,			BACKSPACE,	KEY_F13,	LBRACE,		KEY_9,		KEY_8,		KEY_7,		KEY_5,		KEY_3,
	KEY_1,		0,			0,			BACKSLASH,	KEY_ENTER,	KEY_P,		KEY_O,		KEY_U,		0,			KEY_T,		KEY_E,		KEY_Q,
	KEY_TILDE,	0,			PAGE_UP,	RSHIFT,		KEY_QUOTE,	KEY_SEMI,	KEY_I,		KEY_Y,		KEY_R,		KEY_W,		0,			KEY_TAB,
	0,			KEY_HOME,	KEY_UP,		KEY_SLASH,	KEY_L,		KEY_J,		KEY_G,		0,			KEY_D,		KEY_A,		LEFT_GUI,	0,		
	DEL,		PAGE_DOWN,	RIGHT_ALT,	KEY_PERIOD,	KEY_K,		KEY_H,		KEY_F,		KEY_S,		KEY_Z,		LCTRL,		0,			KEY_END,
	KEY_DOWN,	KEY_LEFT,	0,			KEY_COMMA,	KEY_N,		KEY_V,		KEY_X,		LEFT_ALT,	LSHIFT,		0,			KEY_INSERT,	KEY_RIGHT,
	RCTRL,		KEY_SPACE,	KEY_M,		KEY_B,		KEY_C,		0,			LEFT_GUI,	PRTSCN,		0,			0
};

// key map as seen above, uppercase matches text on keys literally
// 				ENTER		|<-			BLANK		->|			ERASE EOF	NUMPAD 7	NUMPAD 1	NUMPAD 4	NUMPAD 0	NUMPAD -	NUMPAD .
// NUMPAD 3		NUMPAD 6	NUMPAD 9	NUMPAD ,	NUMPAD 8				NUMPAD 2	NUMPAD 5
// 																			PF12		PF11		PF10		PF9			PF8			PF7
// PF6			PF5			PF3			PF2			blank		+=			\|			-_			0						6			4
// 2			PF4			PF1			ATTN		backspace	}{			!cent		9			8			7			5			3
// 1			CLEAR					backtab		tall enter	P			O			U						T			E			Q
// `~			CURSR SEL.	DUP			R SHIFT		'"			;:			I			Y			R			W						tab
// ERASE IN.	a^			up			/?			L			J			G						D			A			ERASE EOF	CURS BLINK
// PA1			FIELD MARK	ALT			..			K			H			F			S			Z			capslock	TEST		ao thing
// down			left					,,			N			V			X			><			L SHIFT		ALT CURS	PA2			right
// bot. enter	space		M			B			C						RESET		A,a			IDENT
