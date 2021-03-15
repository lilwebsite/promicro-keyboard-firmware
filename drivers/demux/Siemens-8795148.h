// The Siemens keyboard this is designed for doesn't really have a name.
// keyboard model # 8795148 E 2508

// It polls the matrix by using a 74HCT00D to select between three 74HC138 chips.
// 

//scanvalues used to check each row 0 - 7 (for a 3 to 8 line demultiplexer)
// x indicates low signal
//CBA
//000 - x . . . . . . .
//001 - . x . . . . . .
//010 - . . x . . . . .
//011 - . . . x . . . .
//100 - . . . . x . . .
//101 - . . . . . x . .
//110 - . . . . . . x .
//111 - . . . . . . . x

#define SCANVALUES \
{\
	{0, 0, 0}\
	{0, 0, 1}\
	{0, 1, 0}\
	{0, 1, 1}\
	{1, 0, 0}\
	{1, 0, 1}\
	{1, 1, 0}\
	{1, 1, 1}\
}

extern uint8_t overrides(void);
extern void setup_keys(void);
extern void press_release(void);
extern void layer_select(void);
extern volatile uint8_t ispressed[COLUMNS];
extern volatile uint8_t previous_presses[ROWS][COLUMNS];
extern volatile struct kbstate kbd;
extern const struct pin input_pins[COLUMNS];
extern uint8_t row;

static volatile uint8_t pin_state;

volatile uint8_t DRIVER_ROWS;

void scan(void);
