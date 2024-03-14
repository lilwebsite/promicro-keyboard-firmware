//////////////////////////////////////////
//////////////////////////////////////////
//DO NOT USE THIS FILE WITHOUT MODIFYING//
//////////////////////////////////////////
//////////////////////////////////////////

//MODIFY THIS FILE BEFORE USE... WILL NOT WORK WITHOUT MODIFICATION

#define LAYERS 2//edit this if more layers are added
static const struct kblayer PROGMEM kblayer_list[LAYERS] = {
	{
		0,//primary layer
		{
			0,//NC = not connected, automatically assigned
			//base matrix layer config
			//0			1			2			3			4			5			6			7<-col	row
			NC,			NC,			KEY_2,		KEY_W,		KEY_S,		KEY_Z,		KEY_TAB,	KEY_ESC,	//0  {2, W, S, Z, TAB, ESC}
			NC,			NC,			KEY_3,		KEY_E,		KEY_D,		KEY_X,		KEY_Q,		KEY_1,		//1  {3, E, D, X, Q, 1}
			NC,			NC,			KEY_4,		KEY_R,		KEY_F,		KEY_C,		NC,			LCTRL,		//2  {4, R, F, C, NC, LCTRL}
			NC,			NC,			KEY_5,		KEY_T,		KEY_G,		KEY_V,		LSHIFT,		KEY_A,		//3  {5, T, G, V, LSHFT, A}
			NC,			NC,			KEY_6,		KEY_Y,		KEY_H,		KEY_B,		LGUI,		RESET,		//4  {6, Y, H, B, LGUI, RESET}
			NC,			NC,			KEY_7,		KEY_U,		KEY_J,		KEY_N,		NC,			LALT,		//5  {7, U, J, N, NC, LALT}
			NC,			NC,			KEY_8,		KEY_I,		KEY_K,		KEY_M,		KEY_SPACE,	N00,		//6  {8, I, K, M, SPACE, N00}
			NC,			NC,			KEY_9,		KEY_O,		KEY_L,		COMMA,		RGUI,		RALT,		//7  {9, O, L, ,<, RGUI, RALT}
			NC,			NC,			KEY_0,		KEY_P,		KEY_SEMI,	PERIOD,		KEY_0,		ENTER,		//8  {0, P, ;:, .>, N0, ENTER}
			NC,			NC,			KEY_EQUAL,	KEY_MINUS,	KEY_QUOTE,	KEY_SLASH,	PAGE_UP,	KP_PERIOD,	//9  {=+, -_, '", /?, PGUP, .>}
			NC,			NC,			KEY_TILDE,	LBRACE,		RBRACE,		PRTSCN,		PAGE_DOWN,	E16,		//10 {`~, [{, ]}, PRTSCN, PGDN, E16}
			NC,			NC,			NUMLOCK,	BACKSLASH,	RCTRL,		RSHIFT,		KEY_F11,	E15,		//11 {S0, \|, RCTRL, RSHFT, E11, E15}
			E18,		KEY_F12,	BACKSPACE,	KEY_LEFT,	KEY_RIGHT,	KEY_DOWN,	KEY_F10,	E14,		//12 {E18, E12, BCKSPC, LEFT, RIGHT, DOWN, E10, E14}
			KEY_UP,		KEYPAD_5,	KEY_F2,		KEY_F6,		KEY_F9,		KEYPAD_1,	KEY_F7,		KEY_F3,		//13 {UP, N5, E2, E6, E9, N1, E7, E3}
			KEY_HOME,	KEYPAD_7,	KEY_F1,		KEY_F5,		KEYPAD_4,	KEYPAD_2,	KEY_F8,		KEY_F4,		//14 {S5, N7, E1, E5, N4, N2, E8, E4}
			KEY_END,	KEYPAD_8,	DEL,		KEYPAD_9,	KEYPAD_6,	KEYPAD_3,	E19,		E20			//15 {S6, N8, S7, N9, N6, N3, E19, E20}
		}
	},
	
	{
		1,//secondary layer
		{
			0,//NC = not connected, automatically assigned
			//base matrix layer config
			//0			1			2			3			4			5			6			7<-col		row
			NC,			NC,			KEY_2,		KEY_W,		KEY_S,		KEY_Z,		KEY_TAB,	KEY_ESC,	//0  {2, W, S, Z, TAB, ESC}
			NC,			NC,			KEY_3,		KEY_E,		KEY_D,		KEY_X,		KEY_Q,		KEY_1,		//1  {3, E, D, X, Q, 1}
			NC,			NC,			KEY_4,		KEY_R,		KEY_F,		KEY_C,		NC,			LCTRL,		//2  {4, R, F, C, NC, LCTRL}
			NC,			NC,			KEY_5,		KEY_T,		KEY_G,		KEY_V,		LSHIFT,		KEY_A,		//3  {5, T, G, V, LSHFT, A}
			NC,			NC,			KEY_6,		KEY_Y,		KEY_H,		KEY_B,		LGUI,		RESET,		//4  {6, Y, H, B, LGUI, RESET}
			NC,			NC,			KEY_7,		KEY_U,		KEY_J,		KEY_N,		NC,			LALT,		//5  {7, U, J, N, NC, LALT}
			NC,			NC,			KEY_8,		KEY_I,		KEY_K,		KEY_M,		KEY_SPACE,	N00,		//6  {8, I, K, M, SPACE, N00}
			NC,			NC,			KEY_9,		KEY_O,		KEY_L,		COMMA,		RGUI,		RALT,		//7  {9, O, L, ,<, RGUI, RALT}
			NC,			NC,			KEY_0,		KEY_P,		KEY_SEMI,	PERIOD,		KEY_0,		ENTER,		//8  {0, P, ;:, .>, N0, ENTER}
			NC,			NC,			KEY_EQUAL,	KEY_MINUS,	KEY_QUOTE,	KEY_SLASH,	PAGE_UP,	KP_PERIOD,	//9  {=+, -_, '", /?, PGUP, .>}
			NC,			NC,			KEY_TILDE,	LBRACE,		RBRACE,		PRTSCN,		PAGE_DOWN,	E16,		//10 {`~, [{, ]}, PRTSCN, PGDN, E16}
			NC,			NC,			NUMLOCK,	BACKSLASH,	RCTRL,		RSHIFT,		KEY_F11,	E15,		//11 {S0, \|, RCTRL, RSHFT, E11, E15}
			E18,		KEY_F12,	BACKSPACE,	KEY_LEFT,	KEY_RIGHT,	KEY_DOWN,	KEY_F10,	E14,		//12 {E18, E12, BCKSPC, LEFT, RIGHT, DOWN, E10, E14}
			KEY_UP,		KEYPAD_5,	KEY_F2,		KEY_F6,		KEY_F9,		KEYPAD_1,	KEY_F7,		KEY_F3,		//13 {UP, N5, E2, E6, E9, N1, E7, E3}
			KEY_HOME,	KEYPAD_7,	KEY_F1,		KEY_F5,		KEYPAD_4,	KEYPAD_2,	KEY_F8,		KEY_F4,		//14 {S5, N7, E1, E5, N4, N2, E8, E4}
			KEY_END,	KEYPAD_8,	DEL,		KEYPAD_9,	KEYPAD_6,	KEYPAD_3,	E19,		E20			//15 {S6, N8, S7, N9, N6, N3, E19, E20}
		}
	}
};
