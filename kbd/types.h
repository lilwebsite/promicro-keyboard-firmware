#include <settings.h>
#ifndef __TYPES
struct kbstate
{
	uint8_t last;//last keyboard state
	uint8_t state;//keyboard state
	uint8_t row_state;
	uint8_t changed;//has keyboard state changed
};

//struct for active or non-active keys
struct keystate
{
	uint8_t row;
	uint8_t column;
	uint8_t pressed;
};

struct kblayer
{
	//whenever this type is accessed the program will know what characters belong to which layer
	const uint8_t layer;
	const uint8_t matrix[ROWS][COLUMNS];
};

struct kblayer_key
{
	uint8_t layer;//target layer
	uint8_t toggle;//toggle (1) or hold key (0) to get layer
	struct keystate key;//row and column data, pressed value ignored
};

struct pin
{
	const char port;
	const uint8_t position;
};

struct keypress {
	uint8_t keycode;
	uint8_t row;
	uint8_t column;
};

const inline uint8_t keypress_compare(struct keypress kp1, struct keypress kp2)
{return (kp1.row == kp2.row) && (kp1.column == kp2.column);}

struct twi_target {
	uint8_t slave;
	uint8_t addr;
	uint8_t rw;
	uint8_t data;
};

#define __TYPES
#endif
