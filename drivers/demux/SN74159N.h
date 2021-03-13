//SN74159N is a 4 to 16 line demultiplexer

///////////////
//USEFUL INFO//
///////////////
//		pro micro pin layout -> https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/ProMicro16MHzv1.pdf
//
//		demultiplexer datasheet -> https://html.alldatasheet.com/html-pdf/27373/TI/SN74159N/22/1/SN74159N.html
//		OR
//		search for SN74159N

//scanvalues used to check each row 0 - 15 (for a 4 to 16 line demultiplexer)
//DCBA
//0000 - x . . . . . . . . . . . . . . .
//0001 - . x . . . . . . . . . . . . . .
//0010 - . . x . . . . . . . . . . . . .
//0011 - . . . x . . . . . . . . . . . .
//0100 - . . . . x . . . . . . . . . . .
//0101 - . . . . . x . . . . . . . . . .
//0110 - . . . . . . x . . . . . . . . .
//0111 - . . . . . . . x . . . . . . . .
//1000 - . . . . . . . . x . . . . . . .
//1001 - . . . . . . . . . x . . . . . .
//1010 - . . . . . . . . . . x . . . . .
//1011 - . . . . . . . . . . . x . . . .
//1100 - . . . . . . . . . . . . x . . .
//1101 - . . . . . . . . . . . . . x . .
//1110 - . . . . . . . . . . . . . . x .
//1111 - . . . . . . . . . . . . . . . x

#define SCANVALUES \
{\
	{0, 0, 0, 0}\
	{0, 0, 0, 1}\
	{0, 0, 1, 0}\
	{0, 0, 1, 1}\
	{0, 1, 0, 0}\
	{0, 1, 0, 1}\
	{0, 1, 1, 0}\
	{0, 1, 1, 1}\
	{1, 0, 0, 0}\
	{1, 0, 0, 1}\
	{1, 0, 1, 0}\
	{1, 0, 1, 1}\
	{1, 1, 0, 0}\
	{1, 1, 0, 1}\
	{1, 1, 1, 0}\
	{1, 1, 1, 1}\
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
