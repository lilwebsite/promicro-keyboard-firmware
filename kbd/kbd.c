#include "kbd.h"

uint8_t get_layer_key(uint8_t layer_num, uint8_t row, uint8_t column)
{
	for(uint8_t x = 0; x < LAYERS; x++)
	{
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
			//{return pgm_read_byte(&(kblayer_list[x].layer_addr[row][column]));}
			//{return ((const uint8_t)pgm_read_byte(kblayer_list[x].layer_addr))[row][column];}
		}
	}
	return 0;
}

void layer_select(void)
{
	uint8_t x = 0;
	struct kblayer_key *last_layer;//points to a layer in layer_keys array
	for(; x < LAYERS; x++)
	{
		if(layer_keys[x].key.pressed)
		{last_layer = &layer_keys[x];}//we wana work with this layer as last layer
	}
	for(x = 0; x < COLUMNS; x++)
	{
		struct keystate keypress = currently_pressing[x];//get the current key
		if(keypress.pressed)//is the current key being pressed
		{
			for(uint8_t z = 0; z < LAYERS; z++)
			{
				if(layer_keys[z].key.row == keypress.row
				&& layer_keys[z].key.column == keypress.column)//if the key being pressed is a layer key
				{
					if(layer_keys[z].layer == last_layer->layer)
					{continue;}//if the layer key being pressed is the same as the last layer ignore it
					
					//update the layer keys
					layer_keys[z].key.pressed = 1;
					last_layer->key.pressed = 0;
					layer = &layer_keys[z];//end with setting the current layer to what was pressed
					break;
				}
			}
		}
		if(last_layer->layer != layer->layer)//if the new layer was selected complete the function
		{break;}
	}
	return;
}

void press_release(void)
{
	uint8_t found;
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		//uint8_t current_key = pgm_read_byte(&(primary_layer.matrix[currently_pressing[x].row][currently_pressing[x].column]));//read keycode from program space
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
	#ifdef __USER
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
			//standby will 'turn off' keyboard using the RESET key
			if(keypress.row == standby_sw.row 
			&& keypress.column == standby_sw.column)
			{standby_switch(); continue;}
			
			if(standby)
			{
				reset_sending();
				kbsend();
				continue;
			}
			//
			
			//double zero key
			if(keypress.row == keypad_00.row
			&& keypress.column == keypad_00.column)
			{send_00(); continue;}
			//
			
			//CAPSLOCK on both SHIFTL and SHIFTR press
			if(shiftkey(keypress))
			{
				if(shiftcaps.row == default_state.row
				&& shiftcaps.column == default_state.column)
				{shiftcaps = keypress; continue;}//shiftcaps resets to default_state in function reset_keys()
				
				if(shiftcaps.row != keypress.row
				|| shiftcaps.column != keypress.column)
				{capslock(); continue;}
			}
			//
			
			//volume up/down buttons
			if(volumekey(keypress))
			{volume(volumekey(keypress));}
			//
		}
	}
}
#endif

int main(void)
{
	while(!init_promicro());
	usb_init();
	while(!usb_configured());

	for(uint8_t x = 0; x < DEMUX_IN - 1; x++)
	{
		DEMUX_BITS = (DEMUX_BITS<<1);
		DEMUX_BITS++;
	}
	
	_delay_ms(250);//startup delay
	layer_keys[0].key.pressed = 1;
	layer = &layer_keys[0];//selects the primary layer as default
	usb_keyboard_press(NUMLOCK, 0);//default to numlock on
	while(1)//main program
	{
		uint8_t x;
		reset_keys();//key queues need to be completely reset before loop starts from beginning
		if(kbd.last != kbd.state){kbd.last = kbd.state; kbd.changed = 1;}
		if(kbd.changed){kbsend();}//if the keyboard state has changed send the keys
		kbd.state = 0;

		for(row = 0; row <= DEMUX_BITS; row++)
		{
			kbd.row_state = 0;
			readthepin = 0;//reads a pin bit
			PORTF = u4_reverse(row)<<4;//sets demux line, u4 switches from LSB to MSB for unsigned 4bit numbers
			//							[programmer note]: more complex logic is required for pins that are not on same port or not aligned next to each other in order
			
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
			
			setup_keys();//figure out what is being pressed in the row
			
			layer_select();//if any of the layer keys were pressed this changes the variable layer
			
			#ifdef __USER
			functions();//figure out if theres something we have to run first
			#endif
			
			press_release();
			
			_delay_us(35);//>switching characteristics: from A, B, C (input) to any Y (output) - Vcc 4.5V, (TYP)18ns (MAX)36ns
			//				[programmer note]: this matches the switching characteristics of most demux chips
			
			init_inputs();//re-initialize the inputs
		}
	}
	return 1;
}