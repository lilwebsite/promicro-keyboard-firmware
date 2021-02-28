//static const struct pin demux_pins[DEMUX_IN] = {{B, 1}, {B, 3}, {B, 2}, {B, 6}};
//A, B, C, D (pins 15, 14, 16, 10)

const struct pin demux_pins[DEMUX_IN] = {{F, 4}, {F, 5}, {F, 6}, {F, 7}};
//A, B, C, D (pins A0, A1, A2, A3)

static const struct pin input_pins[COLUMNS] = 
{
	{B, 5},//9
	{B, 4},//8
	{E, 6},//7
	{D, 7},//6
	{C, 6},//5
	{D, 4},//4
	{D, 0},//3
	{D, 1} //2
};

uint8_t init_pins(void)
{
	//pins are based on a pro micro in order
	//output pins for the decoder
	set_PINX_output(4, F);//pin A0
	set_PINX_output(5, F);//pin A1
	set_PINX_output(6, F);//pin A2
	set_PINX_output(7, F);//pin A3

	init_inputs(input_pins);
	
	//USB
	set_PINX_input(3, D, 0);
	set_PINX_input(2, D, 0);
	
	//not used
	set_PINX_input(6, B, 1);//pin 10
	set_PINX_input(2, B, 1);//pin 16
	set_PINX_input(3, B, 1);//pin 14
	set_PINX_input(1, B, 1);//pin 15
	set_PINX_input(0, F, 0);
	set_PINX_input(1, F, 0);
	set_PINX_input(2, E, 0);
	set_PINX_input(5, D, 0);
	set_PINX_input(6, D, 0);
	set_PINX_input(7, C, 0);
	set_PINX_input(0, B, 0);
	set_PINX_input(7, B, 0);
	
	return 1;
}
