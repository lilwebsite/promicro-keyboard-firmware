#include <kbd/globals.h>
#include <kbd/matrix.h>
#include <usb/keys.h>
#include <kbmatrix/PC8801.h>

#define LAYERS 2

struct kblayer_key layer_keys[LAYERS] = {
	base_layer,
	{1, 0, {8, 5, 0}} // secondary layer
};

const struct kblayer PROGMEM kblayer_list[LAYERS] = {
	{
		0,//primary layer
		MATRIX_POS(
			0,//NC = not connected, automatically assigned
			//base matrix layer config
			//0			1			2			3			4			5			6			7<-col		row
			KEYPAD_0,	KEYPAD_1,	KEYPAD_2,	KEYPAD_3,	KEYPAD_4,	KEYPAD_5,	KEYPAD_6,	KEYPAD_7,	//0
			KEYPAD_8,	KEYPAD_9,	KEYPAD_AST,	KEYPAD_PLS,	KEY_EQUAL,	KEYPAD_COM,	KEYPAD_PRD,	KEY_ENTER,	//1
			LBRACE,		KEY_A,		KEY_B,		KEY_C,		KEY_D,		KEY_E,		KEY_F,		KEY_G,		//2
			KEY_H,		KEY_I,		KEY_J,		KEY_K,		KEY_L,		KEY_M,		KEY_N,		KEY_O,		//3
			KEY_P,		KEY_Q,		KEY_R,		KEY_S,		KEY_T,		KEY_U,		KEY_V,		KEY_W,		//4
			KEY_X,		KEY_Y,		KEY_Z,		RBRACE,		KEY_TILDE,	BACKSLASH,	KEY_EQUAL,	KEY_MINUS,	//5
			KEY_0,		KEY_1,		KEY_2,		KEY_3,		KEY_4,		KEY_5,		KEY_6,		KEY_7,		//6
			KEY_8,		KEY_9,		KEY_QUOTE,	KEY_SEMI,	KEY_COMMA,	KEY_PERIOD,	KEY_SLASH,	RIGHT_ALT,	//7
			KEY_HOME,	NUMLOCK,	LEFT_GUI,	BACKSPACE,	LEFT_ALT,	0,			LSHIFT,		LCTRL,		//8
			KEY_PAUSE,	KEY_F1,		KEY_F2,		KEY_F3,		KEY_F4,		KEY_F5,		KEY_SPACE,	KEY_ESC,	//9
			KEY_TAB,	KEY_INSERT,	KEY_END,	KEY_DELETE,	PRTSCN,		KEYPAD_MIN,	KEYPAD_SLH, CAPS,		//10
			PAGE_UP,	PAGE_DOWN																			//11
		)
	},

	{
		1,//secondary layer
		MATRIX_POS(
			0,//NC = not connected, automatically assigned
			//base matrix layer config
			//0			1			2			3			4			5			6			7<-col		row
			KEY_LEFT,	KEYPAD_1,	KEY_UP,		KEYPAD_3,	KEYPAD_4,	KEYPAD_5,	KEYPAD_6,	KEYPAD_7,	//0
			KEYPAD_8,	KEYPAD_9,	KEYPAD_AST,	KEYPAD_PLS,	KEY_EQUAL,	KEY_DOWN,	KEY_RIGHT,	KEY_ENTER,	//1
			LBRACE,		KEY_A,		KEY_B,		KEY_C,		KEY_D,		KEY_E,		KEY_F,		KEY_G,		//2
			KEY_H,		KEY_I,		KEY_J,		KEY_K,		KEY_L,		KEY_M,		KEY_N,		KEY_O,		//3
			KEY_P,		KEY_Q,		KEY_R,		KEY_S,		KEY_T,		KEY_U,		KEY_V,		KEY_W,		//4
			KEY_X,		KEY_Y,		KEY_Z,		RBRACE,		KEY_TILDE,	BACKSLASH,	KEY_EQUAL,	KEY_MINUS,	//5
			KEY_0,		KEY_1,		KEY_2,		KEY_3,		KEY_4,		KEY_5,		KEY_6,		KEY_7,		//6
			KEY_8,		KEY_9,		KEY_QUOTE,	KEY_SEMI,	KEY_COMMA,	KEY_PERIOD,	KEY_SLASH,	RIGHT_ALT,	//7
			KEY_HOME,	NUMLOCK,	LEFT_GUI,	BACKSPACE,	LEFT_ALT,	0,			LSHIFT,		LCTRL,		//8
			KEY_PAUSE,	KEY_F6,		KEY_F7,		KEY_F8,		KEY_F9,		KEY_F10,	KEY_SPACE,	KEY_ESC,	//9
			KEY_TAB,	KEY_INSERT,	KEY_END,	KEY_DELETE,	PRTSCN,		KEYPAD_MIN,	KEYPAD_SLH, CAPS,		//10
			KEY_F11,	KEY_F12																				//11
		)
	},
};
