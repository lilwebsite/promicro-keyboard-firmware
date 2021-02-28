void solenoid_toggle(void)
{
	solenoid ^= 0b1;
	solenoid |= 0b100;
	solenoid &= 0b101;
	return;
}

void init(void)
{
	data_lines = 0;
	counter = 0;
	last_pressed_count = 0;
	pressed_counter = 0;
	clock_data = 0;
	solenoid = 1; // enables the solenoid
	//solenoid = 0; // disables the solenoid
	return;
}
