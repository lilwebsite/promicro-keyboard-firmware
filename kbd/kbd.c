#include "kbd.h"

uint8_t get_layer_key(uint8_t layer_num, uint8_t row, uint8_t column)
{
	for(uint8_t x = 0; x < LAYERS; x++)
	{
		//if row and column are both max (255) then we are checking if layer exists
		if(row == 0xFF && column == 0xFF)
		{
			for(uint8_t x = 0; x < LAYERS; x++)
			{
				if(pgm_read_byte(&(kblayer_list[x].layer)) == layer_num)
				{return 1;}
			}
		}
		else
		{
			if(pgm_read_byte(&(kblayer_list[x].layer)) == layer_num)
			{return pgm_read_byte(&(kblayer_list[x].matrix[row][column]));}
		}
	}
	return 0;
}

void layer_select(void)
{
	for(uint8_t x = 1; x < LAYERS; x++)
	{
		const struct kblayer_key *target_layer = &layer_keys[x];
		uint8_t is_pressed = 0;

		// if row doesn't match the key, then don't proceed
		if(row != target_layer->key.row)
		{continue;}
		// prevent the user from descending more than one layer simultaneously
		if(target_layer->layer != layer->layer
		&& layer->layer != base_layer.layer)
		{continue;}

		for(uint8_t y = 0; y < COLUMNS; y++)
		{
			struct keystate keypress = currently_pressing[y];
			if(target_layer->key.row == keypress.row
			&& target_layer->key.column == keypress.column
			&& keypress.pressed)
			{is_pressed = 1; break;}
		}

		if(is_pressed)
		{
			//if(target_layer->toggle)
			//{
			//	// toggle between base_layer to target_layer
			//	if(target_layer->layer != base_layer.layer)
			//	{layer = target_layer;}
			//	else
			//	{layer = &base_layer;}
			//	_delay_us(75); // debounce protection
			//	continue;
			//}
			// we have passed the toggle logic, so now we check
			// for if the layer is actively being pressed.
			// if the the target layer is already active, we don't
			// need to set it twice.
			if(layer->layer == base_layer.layer)
			{layer = target_layer;}
		}
		else
		{
			// toggle logic does not apply here.
			// we only check if we need to reset 
			// to the base layer
			if(layer->layer != base_layer.layer)
			//&& !target_layer->toggle)
			{layer = &base_layer;}
		}
	}

	return;
}

void press_release(void)
{
	uint8_t found;
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		if(!get_layer_key(layer->layer, 0xFF, 0xFF))//if this evaluates to 0, something went terribly wrong or the layer does not exist
		{return;}
		struct keypress current_key = {
			get_layer_key(layer->layer, currently_pressing[x].row, currently_pressing[x].column),
			row,
			x
		};
		if(kbd.row_state)//if a key is being pressed in the row
		{
			if(currently_pressing[x].pressed)//if the key is physically being pressed
			{
				found = 0;
				for(uint8_t z = 0; z < KEY_ROLLOVER; z++)
				{
					if(keypress_compare(keyboard_keys[z], current_key)
					&& keyboard_keys[z].keycode == current_key.keycode)
					{found = 1; break;}
				}
				if(!found)
				{kbd_press_release(1, current_key);}
				continue;
			}
			kbd_press_release(0, current_key);//if the current key isn't being physically pressed, make sure its released
			continue;
		}
		kbd_press_release(0, current_key);//if nothing is being pressed in the row, release all the keys for that row
	}
	return;
}

void setup_keys(void)
{
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		previous_presses[row][x] = currently_pressing[x].pressed;//save the last state before assigning a new one
		currently_pressing[x] = (struct keystate){row, x, 0};
		if(!ispressed[x])
		{currently_pressing[x].pressed = 1;}
	}
	return;
}

void reset_keys(void)
{
	uint8_t x = 0;
	
	for(; x < ROWS; x++)
	{
		for(uint8_t z = 0; z < COLUMNS; z++)
		{
			previous_presses[x][z] = 0;
		}
	}
	
	for(x = 0; x < COLUMNS; x++)
	{
		currently_pressing[x] = default_state;
	}
	
	//everything under here is used in user functions
	#if defined __USER && defined KBD_IBMPingmaster
	shiftcaps = default_state;//used in functions()
	#endif
	
	return;
}

#ifdef __USER
void functions(void)
{
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		struct keystate keypress = currently_pressing[x];
		if(keypress.pressed)
		{
			////////////////////////////////////////////////////
			//add custom logic for user defined functions here//
			////////////////////////////////////////////////////
			#ifdef __STDBY
			//standby will 'turn off' keyboard using the RESET key (pingmaster)
			if(keypress.row == standby_sw.row 
			&& keypress.column == standby_sw.column
			&& !layer->layer)//if not on layer 0, this will not execute
			{standby_switch(); continue;}
			
			if(standby)
			{continue;}
			#endif
			
			#ifdef KBD_IBMPingmaster
			//double zero key (pingmaster)
			if(keypress.row == keypad_00.row
			&& keypress.column == keypad_00.column)
			{send_00(); continue;}
			#endif
			
			#ifdef KBD_IBMPingmaster
			//CAPSLOCK on both SHIFTL and SHIFTR press
			if(shift_key(keypress))
			{
				if(shiftcaps.row == default_state.row
				&& shiftcaps.column == default_state.column)
				{shiftcaps = keypress; continue;}//shiftcaps resets to default_state in function reset_keys()
				
				if(shiftcaps.row != keypress.row
				|| shiftcaps.column != keypress.column)
				{capslock(); continue;}
			}
			#endif
			
			#if defined KBD_IBMPingmaster || defined KBD_PC8801
			//volume up/down buttons
			if(volume_key(keypress))
			{volume(volume_key(keypress)); continue;}
			#endif
			
			#ifdef KBD_IBMPingmaster
			//prev/next track buttons
			if(prevnext_key(keypress))
			{next_prev_track(prevnext_key(keypress)); continue;}
			#endif
			
			#ifdef KBD_IBMPingmaster
			//play/pause button
			if(keypress.row == play_pause.row
			&& keypress.column == play_pause.column)
			{play_pause_media(); continue;}
			#endif

			#ifdef KBD_PC8801
			if(keypress.row == mute_key.row
			&& keypress.column == mute_key.column)
			{mute(); continue;}
			#endif
		}
	}
}
#endif

int main(void)
{
	while(!init_promicro());
	usb_init();
	while(!usb_configured());
	
	init_driver();
	
	_delay_ms(250);//startup delay
	#ifdef ENABLE_LAYERS
	layer = &base_layer;//selects the primary layer as default
	//last_layer = &base_layer;
	#endif
	//usb_keyboard_press(NUMLOCK, 0);//default to numlock on
	while(1)//main program
	{
		uint8_t x;
		reset_keys();//key queues need to be completely reset before loop starts from beginning
		if(kbd.last != kbd.state){kbd.last = kbd.state; kbd.changed = 1;}
		if(kbd.changed){kbsend();}//if the keyboard state has changed send the keys
		kbd.state = 0;

		for(row = 0; row <= DRIVER_ROWS; row++)//the amount of driver rows is decided by the selected driver in drivers/driver.h
		{
			kbd.row_state = 0;
			readthepin = 0;//reads a pin bit
			
			set_row(row);
			
			for(x = 0; x < COLUMNS; x++)
			{
				readthepin = read_PINX_bit(input_pins[x].position, input_pins[x].port);
				
				if(readthepin)
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
			#ifdef __STDBY
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
	return 1;
}
