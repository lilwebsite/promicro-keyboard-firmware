extern uint8_t init_inputs(const struct pin *input_pins);

//A, B, C (pins A3, A2, A1)
const struct pin demux_pins[DEMUX_IN] = {{F, 4}, {F, 5}, {F, 6}};

const struct pin input_pins[COLUMNS] = 
{
	{C, 6}, // pin 5
	{D, 7}, // pin 6
	{E, 6}, // pin 7
	{B, 4}, // pin 8
	{B, 5}, // pin 9
	{B, 3}, // pin 14
	{B, 2}, // pin 16
	{B, 6}  // pin 10
};

uint8_t init_pins(void)
{
	//output pins for the decoder
	set_PINX_output(6, F); // pin A1
	set_PINX_output(5, F); // pin A2
	set_PINX_output(4, F); // pin A3

	//demux selector pins
	set_PINX_output(7, F); // pin A0
	set_PINX_output(1, B); // pin 15

	init_inputs(input_pins);

	// not used
	set_PINX_input(1, D, 0); // pin 2
	set_PINX_input(0, D, 0); // pin 3
	set_PINX_input(4, D, 0); // pin 4
	set_PINX_input(3, D, 0); // pin TXO
	set_PINX_input(2, D, 0); // pin RXI

	set_PINX_input(0, B, 0); // RX LED
	set_PINX_input(5, D, 0); // TX LED

	// physically not connected
	set_PINX_input(0, F, 0);
	set_PINX_input(1, F, 0);
	set_PINX_input(2, E, 0);
	set_PINX_input(6, D, 0);
	set_PINX_input(7, C, 0);
	set_PINX_input(7, B, 0);

	return 1;
}
