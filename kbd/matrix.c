#include "matrix.h"
#include <layout.h>

uint8_t get_layer_key(uint8_t layer_num, uint8_t row, uint8_t column)
{
	//if row and column are both max (255) then we are checking if layer exists
	//if(row == 0xFF && column == 0xFF)
	//{
	//	for(uint8_t x = 0; x < LAYERS; x++)
	//	{
	//		if(pgm_read_byte(&(kblayer_list[x].layer)) == layer_num)
	//		{return 1;}
	//	}
	//	return 0;
	//}

	for(uint8_t x = 0; x < LAYERS; x++)
	{
		if(pgm_read_byte(&(kblayer_list[x].layer)) == layer_num)
		{return pgm_read_byte(&(kblayer_list[x].matrix[row][column]));}
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
		//struct keypress current_key = {
		//	get_layer_key(0, currently_pressing[x].row, currently_pressing[x].column),
		//	row,
		//	x
		//};
		//if(get_layer_key(layer->layer, 0xFF, 0xFF))//if this evaluates to 0, something went terribly wrong or the layer does not exist
		//{return;}

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

	reset_user();

	return;
}
