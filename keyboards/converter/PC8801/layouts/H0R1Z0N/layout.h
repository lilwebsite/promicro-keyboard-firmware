#include <kbd/globals.h>
#include <kbd/matrix.h>
#include <usb/keys.h>
#include <kbmatrix/PC8801.h>

#define LAYERS 3

const static struct keystate volume_up = {9, 3, 0};
const static struct keystate volume_down = {9, 2, 0};
const static struct keystate mute_key = {9, 1, 0};

struct kblayer_key layer_keys[LAYERS] = {
	base_layer,
	{1, 0, {8, 5, 0}},//secondary layer
	{2, 0, {9, 4, 0}}//tertiary layer
};

const struct kblayer PROGMEM kblayer_list[LAYERS] = {
	{
		0,//primary layer
		MATRIX_POS(
			0,//NC = not connected, automatically assigned
			//base matrix layer config
			//0			1			2			3			4			5			6			7<-col		row
			KEYPAD_0,	KEYPAD_1,	KEYPAD_2,	KEYPAD_3,	KEYPAD_4,	KEYPAD_5,	KEYPAD_6,	KEYPAD_7,	//0
			KEYPAD_8,	KEYPAD_9,	KEYPAD_AST,	KEYPAD_MIN,	KEYPAD_PLS,	KEYPAD_PRD,	KEYPAD_COM,	KEY_ENTER,	//1
			LBRACE,		KEY_A,		KEY_B,		KEY_C,		KEY_D,		KEY_E,		KEY_F,		KEY_G,		//2
			KEY_H,		KEY_I,		KEY_J,		KEY_K,		KEY_L,		KEY_M,		KEY_N,		KEY_O,		//3
			KEY_P,		KEY_Q,		KEY_R,		KEY_S,		KEY_T,		KEY_U,		KEY_V,		KEY_W,		//4
			KEY_X,		KEY_Y,		KEY_Z,		RBRACE,		KEY_DELETE,	BACKSLASH,	KEY_EQUAL,	KEY_MINUS,	//5
			KEY_0,		KEY_1,		KEY_2,		KEY_3,		KEY_4,		KEY_5,		KEY_6,		KEY_7,		//6
			KEY_8,		KEY_9,		KEY_QUOTE,	KEY_SEMI,	KEY_COMMA,	KEY_PERIOD,	KEY_SLASH,	RIGHT_ALT,	//7
			LEFT_GUI,	KEY_UP,		KEY_RIGHT,	BACKSPACE,	LEFT_ALT,	0,			LSHIFT,		LCTRL,		//8
			KEY_ESC,	0,			0,			0,			0,			PRTSCN,		KEY_SPACE,	KEY_TILDE,	//9
			KEY_TAB,	KEY_DOWN,	KEY_LEFT,	KEY_APP,	0,			0,			KEYPAD_SLH, LOCKING_C,	//10
			PAGE_UP,	PAGE_DOWN																			//11
		)
	},
	
	{
		1,//secondary layer
		MATRIX_POS(
			0,//NC = not connected, automatically assigned
			//base matrix layer config
			//0			1			2			3			4			5			6			7<-col		row
			KEY_LEFT,	0,			KEY_UP,		0,			0,			0,			0,			0,			//0
			0,			0,			0,			0,			0,			KEY_RIGHT,	KEY_DOWN,	KEY_ENTER,	//1
			LBRACE,		KEY_A,		KEY_B,		KEY_C,		KEY_D,		KEY_E,		KEY_F,		KEY_G,		//2
			KEY_H,		KEY_I,		KEY_J,		KEY_K,		KEY_L,		KEY_M,		KEY_N,		KEY_O,		//3
			KEY_P,		KEY_Q,		KEY_R,		KEY_S,		KEY_T,		KEY_U,		KEY_V,		KEY_W,		//4
			KEY_X,		KEY_Y,		KEY_Z,		RBRACE,		KEY_DELETE,	BACKSLASH,	KEY_EQUAL,	KEY_MINUS,	//5
			KEY_0,		KEY_1,		KEY_2,		KEY_3,		KEY_4,		KEY_5,		KEY_6,		KEY_7,		//6
			KEY_8,		KEY_9,		KEY_QUOTE,	KEY_SEMI,	KEY_COMMA,	KEY_PERIOD,	KEY_SLASH,	RIGHT_ALT,	//7
			LEFT_GUI,	KEY_HOME,	0,			BACKSPACE,	LEFT_ALT,	0,			LSHIFT,		LCTRL,		//8
			KEY_ESC,	0,			0,			0,			0,			PRTSCN,		KEY_SPACE,	KEY_TILDE,	//9
			KEY_TAB,	KEY_END,	0,			KEY_APP,	0,			0,			KEY_INSERT, LOCKING_C,	//10
			PAGE_UP,	PAGE_DOWN																			//11
		)
	},
	
	{
		2,//tertiary layer
		MATRIX_POS(
			0,//NC = not connected, automatically assigned
			//base matrix layer config
			//0			1			2			3			4			5			6			7<-col		row
			KEYPAD_0,	KEYPAD_1,	KEYPAD_2,	KEYPAD_3,	KEYPAD_4,	KEYPAD_5,	KEYPAD_6,	KEYPAD_7,	//0
			KEYPAD_8,	KEYPAD_9,	KEYPAD_AST,	KEYPAD_MIN,	KEYPAD_PLS,	KEYPAD_PRD,	KEYPAD_COM,	KEY_ENTER,	//1
			KEY_F23,	KEY_A,		KEY_B,		KEY_C,		KEY_D,		KEY_F15,	KEY_F,		KEY_G,		//2
			KEY_H,		KEY_F20,	KEY_J,		KEY_K,		KEY_L,		KEY_M,		KEY_N,		KEY_F21,	//3
			KEY_F22,	KEY_F13,	KEY_F16,	KEY_S,		KEY_F17,	KEY_F19,	KEY_V,		KEY_F14,	//4
			KEY_X,		KEY_F18,	KEY_Z,		KEY_F24,	KEY_DELETE,	BACKSLASH,	KEY_F12,	KEY_F11,	//5
			KEY_F10,	KEY_F1,		KEY_F2,		KEY_F3,		KEY_F4,		KEY_F5,		KEY_F6,		KEY_F7,		//6
			KEY_F8,		KEY_F9,		KEY_QUOTE,	KEY_SEMI,	KEY_COMMA,	KEY_PERIOD,	KEY_SLASH,	RIGHT_ALT,	//7
			LEFT_GUI,	KEY_UP,		KEY_RIGHT,	BACKSPACE,	LEFT_ALT,	0,			LSHIFT,		LCTRL,		//8
			KEY_ESC,	0,			0,			0,			0,			PRTSCN,		KEY_SPACE,	KEY_TILDE,	//9
			KEY_TAB,	KEY_DOWN,	KEY_LEFT,	KEY_APP,	0,			0,			KEYPAD_SLH, LOCKING_C,	//10
			PAGE_UP,	PAGE_DOWN																			//11
		)
	}
};
