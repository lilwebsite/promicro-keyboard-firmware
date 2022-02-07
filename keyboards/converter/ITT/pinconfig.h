uint8_t init_pins(void)
{
	set_PINX_variable_output(5, F, 1); // this pin needs to be HIGH for the keyboard to work, not sure what it's other functions are
	set_PINX_variable_output(6, F, 0); // solenoid trigger

	set_PINX_input(4, F, 0); // clock signal

	return 1;
}
