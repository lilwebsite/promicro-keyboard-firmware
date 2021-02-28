#define KBD_ITT
#define COLUMNS 100
#define KEYS 96

//input pin A4
set_PINX_input(4, F, 0);
//enable pin A3
set_PINX_variable_output(5, F, 1);
//trigger pin A2
set_PINX_variable_output(6, F, 0);


static const struct pin input_pins[COLUMNS] =
{
	{D, 1},//2
	{D, 0},//3
	{D, 4},//4
	{C, 6},//5
	{D, 7},//6
	{E, 6},//7
};

uint8_t init_pins(void)
{
	//pins are based on a pro micro in order
	//output pins for the decoder
//	set_PINX_output(7, F);//pin A0
//	set_PINX_output(6, F);//pin A1
//	set_PINX_output(5, F);//pin A2
//	set_PINX_output(4, F);//pin A3

	set_PINX_variable_output(5, B, 0); // debug 1
	set_PINX_variable_output(4, B, 0); // debug 2
	set_PINX_variable_output(6, E, 0); // debug 3
	set_PINX_variable_output(7, D, 0); // debug 4
	set_PINX_variable_output(6, C, 0); // debug 5

	//solenoid
	//set_PINX_output(6, B);// solenoid enable
	//set_PINX_output(2, B);// solenoid trigger

	//init_inputs(input_pins);
//	set_PINX_input(3, B, 1);
//	set_PINX_input(1, B, 1);
	//set_PINX_input(6, B, 1);
	//set_PINX_input(2, B, 1);

	//USB
	//set_PINX_input(3, D, 0);
	//set_PINX_input(2, D, 0);

	//not used
	//set_PINX_input(6, B, 1);//pin 10
	//set_PINX_input(3, B, 1);//pin 14
//	set_PINX_input(0, F, 0);
//	set_PINX_input(1, F, 0);
//	set_PINX_input(2, E, 0);
	//set_PINX_input(5, D, 0);
	//set_PINX_input(6, D, 0);
//	set_PINX_input(7, C, 0);
	//set_PINX_input(0, B, 0);
	//set_PINX_input(7, B, 0);

	return 1;
}
