// the switch to disable solenoid, references the position in the array below
#define SOLENOID_DISABLE 99

#define LAYERS 1//edit this if more layers are added
const uint8_t keymap[COLUMNS] = {
	KEY_F12,	KEY_F11,	KEY_F10,	KEY_F9,		KEY_F8,		KEY_F7,		KEY_F6,		KEY_F5,		KEY_F3,		KEY_F2,		KEY_ESC,		KEY_EQUAL,
	RBRACE,		KEY_MINUS,	KEY_0,		0,			KEY_6,		KEY_4,		KEY_2,		KEY_F4,		KEY_F1,		KEY_F13,	BACKSPACE,		KEY_F13,
	LBRACE,		KEY_9,		KEY_8,		KEY_7,		KEY_5,		KEY_3,		KEY_1,		KEY_F14,	0,			BACKSLASH,	KEY_ENTER,		KEY_P,
	KEY_O,		KEY_U,		0,			KEY_T,		KEY_E,		KEY_Q,		KEY_TILDE,	KEY_F15,	PAGE_UP,	RSHIFT,		KEY_QUOTE,		KEY_SEMI,
	KEY_I,		KEY_Y,		KEY_R,		KEY_W,		0,			KEY_TAB,	KEY_F16,	KEY_HOME,	KEY_UP,		KEY_SLASH,	KEY_L,			KEY_J,
	KEY_G,		0,			KEY_D,		KEY_A,		LEFT_GUI,	KEY_F17,	DEL,		PAGE_DOWN,	RIGHT_ALT,	KEY_PERIOD,	KEY_K,			KEY_H,
	KEY_F,		KEY_S,		KEY_Z,		LCTRL,		KEY_F18,	KEY_END,	KEY_DOWN,	KEY_LEFT,	0,			KEY_COMMA,	KEY_N,			KEY_V,
	KEY_X,		LEFT_ALT,	LSHIFT,		KEY_F19,	KEY_INSERT,	KEY_RIGHT,	RCTRL,		KEY_SPACE,	KEY_M,		KEY_B,		KEY_C,			0,
	LEFT_GUI,	PRTSCN,		0,			0
};

// key map as seen above, uppercase matches text on keys literally
// PF12			PF11		PF10		PF9			PF8			PF7			PF6			PF5			PF3			PF2			blank			+=
// \|			-_			0						6			4			2			PF4			PF1			ATTN		backspace		}{
// !cent		9			8			7			5			3			1			CLEAR					backtab		tall enter		P
// O			U						T			E			Q			`~			CURSR SEL.	DUP			R SHIFT		'"				;:
// I			Y			R			W						tab			ERASE IN.	a^			up			/?			L				J
// G						D			A			ERASE EOF	CURS BLINK	PA1			FIELD MARK	ALT			..			K				H
// F			S			Z			capslock	TEST		ao thing	down		left					,,			N				V
// X			><			L SHIFT		ALT CURS	PA2			right		bot. enter	space		M			B			C				
// RESET		A,a			IDENT
