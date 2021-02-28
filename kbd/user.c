
////////////////////////////////////
//define user functions below here//
////////////////////////////////////

void capslock(void)//will enable capslock when both SHIFTL and SHIFTR pressed
{
	usb_keyboard_press(KEY_CAPS_LOCK, 0);
	_delay_ms(110);
	return;
}

void volume(const uint8_t UPorDOWN)
{
	if(UPorDOWN == 1)
	{usb_extra_press(VOL_UP);}
	if(UPorDOWN == 2)
	{usb_extra_press(VOL_DOWN);}
	_delay_ms(70);
	return;
}

void play_pause_media(void)
{
	usb_extra_press(PLAY_PAUSE);
	_delay_ms(70);
	return;
}

void mute(void)
{
	usb_extra_press(MUTE);
	_delay_ms(70);
	return;
}

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

			#ifdef ENABLE_STDBY
			//standby will 'turn off' keyboard using the RESET key (pingmaster)
			if(keypress.row == standby_sw.row 
			&& keypress.column == standby_sw.column
			&& !layer->layer)//if not on layer 0, this will not execute
			{
				standby ^= 1; // 0 <--> 1
				_delay_ms(250); // creates a gap so it's hard to cycle repeatedly
				continue;
			}

			if(standby)
			{continue;}
			#endif

			#ifdef ENABLE_00
			//double zero key (pingmaster)
			if(keypress.row == keypad_00.row
			&& keypress.column == keypad_00.column)
			{
				usb_keyboard_press(KEY_0, 0);
				_delay_ms(5);
				usb_keyboard_press(KEY_0, 0);
				_delay_ms(70);//delay to prevent spam - a keypress can be upwards of 120ms, but generally around 80ms
			}
			#endif

			#ifdef ENABLE_SHIFT_CAPSLOCK
			//CAPSLOCK on simultaneous SHIFTL and SHIFTR press
			if((key.row == SHIFTL.row
			&& key.column == SHIFTL.column)
			|| (key.row == SHIFTR.row
			&& key.column == SHIFTR.column))
			{
				if(shiftcaps.row == default_state.row
				&& shiftcaps.column == default_state.column)
				{shiftcaps = keypress; continue;}//shiftcaps resets to default_state in function reset_keys()

				if(shiftcaps.row != keypress.row
				|| shiftcaps.column != keypress.column)
				{capslock(); continue;}
			}
			#endif

			#if ENABLE_VOLUME_KEYS
			//volume up/down buttons
			if(volume_key(keypress))
			{volume(volume_key(keypress)); continue;}
			#endif

			#ifdef ENABLE_TRACK_KEYS
			//prev/next track buttons
			if(prevnext_key(keypress))
			//{next_prev_track(prevnext_key(keypress)); continue;}
			{
				//determines if a key is a prev/next track key and sends their value (1 = prev / 2 = next)
				if(key.row == prev_track.row
				&& key.column == prev_track.column)
				{usb_extra_press(PREV_TRACK);}
				if(key.row == next_track.row
				&& key.column == next_track.column)
				{usb_extra_press(NEXT_TRACK);}
				_delay_ms(70);
				continue;
			}
			#endif

			#ifdef ENABLE_PLAY_PAUSE
			//play/pause button
			if(keypress.row == play_pause.row
			&& keypress.column == play_pause.column)
			{play_pause_media(); continue;}
			#endif

			#ifdef ENABLE_MUTE
			if(keypress.row == mute_key.row
			&& keypress.column == mute_key.column)
			{mute(); continue;}
			#endif
		}
	}
}
#endif
