void scan_signal(void)
{
	uint8_t x;
	reset_keys();//key queues need to be completely reset before loop starts from beginning
	if(kbd.last != kbd.state){kbd.last = kbd.state; kbd.changed = 1;}
	if(kbd.changed){kbsend();}//if the keyboard state has changed send the keys
	kbd.state = 0;

	for(row = 0; row <= DRIVER_ROWS; row++)//the amount of driver rows is decided by the selected driver in drivers/driver.h
	{
		kbd.row_state = 0;
		pin_state = 0;//reads a pin bit
		
		set_row(row);
		
		for(x = 0; x < COLUMNS; x++)
		{
			pin_state = read_PINX_bit(input_pins[x].position, input_pins[x].port);
			
			if(pin_state)
			{ispressed[x] = HIGH; continue;}
			else
			{ispressed[x] = LOW;}//LOW == low state on pin(aka key press), HIGH == high state, or pullup(no press)
		
			//previous_presses is assigned the inverse of LOW(0) or HIGH(1), so if they are equal then it has changed state
			if(previous_presses[row][x] == ispressed[x])
			{kbd.row_state = 1;}
		}
		
		if(!kbd.state && kbd.row_state)
		{kbd.state = 1;}
		
		//figure out what is being pressed in the row
		setup_keys();

		#ifdef ENABLE_LAYERS
		//if any of the layer keys were pressed this applies them to the variable `layer`
		layer_select();
		#endif
		
		#ifdef __USER
		functions();//figure out if theres something we have to run first
		#ifdef ENABLE_STDBY
		if(standby)
		{
			reset_sending();
			kbsend();
			continue;
		}
		#endif
		#endif
		
		press_release();
		
		_delay_us(35);//>switching characteristics: from A, B, C (input) to any Y (output) - Vcc 4.5V, (TYP)18ns (MAX)36ns
		//				[programmer note]: this matches the switching characteristics of most demux chips
		
		//re-initialize the inputs
		init_inputs(input_pins);
	}
}
