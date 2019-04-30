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
	struct keystate key;//row, column, is layer active
};

struct kblayer_cycle
{
	struct kblayer_key lk;
	const uint8_t count;
	uint8_t current;
	uint8_t layers[];
};

struct pin
{
	const char port;
	const uint8_t position;
};
#define __TYPES
#endif