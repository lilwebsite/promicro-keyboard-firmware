#include "globals.h"
#include "user.h"
#include <usb/usb.h>

uint8_t reset;

////////////////////////////////////
//define user functions below here//
////////////////////////////////////

void capslock(void)//will enable capslock when both SHIFTL and SHIFTR pressed
{
	usb_keyboard_press(KEY_CAPS_LOCK, 0);
	_delay_ms(110);
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

uint8_t overrides(void)
{
	functions();

	if(standby)
	{
		reset_sending();
		kbsend();
		return 0;
	}

	return 1;
}

void reset_user(void)
{
	shiftcaps = default_state;
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

			#ifdef STANDBY
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
			if((keypress.row == SHIFTL.row
			&& keypress.column == SHIFTL.column)
			|| (keypress.row == SHIFTR.row
			&& keypress.column == SHIFTR.column))
			{
				if(shiftcaps.row == default_state.row
				&& shiftcaps.column == default_state.column)
				{shiftcaps = keypress; continue;}//shiftcaps resets to default_state in function reset_keys()

				if(shiftcaps.row != keypress.row
				|| shiftcaps.column != keypress.column)
				{capslock(); continue;}
			}
			#endif

			#ifdef ENABLE_VOLUME_KEYS
			//volume up/down buttons
			if(keypress.row == volume_up.row
			&& keypress.column == volume_up.column)
			{usb_extra_press(VOL_UP); _delay_ms(70);}
			if(keypress.row == volume_down.row
			&& keypress.column == volume_down.column)
			{usb_extra_press(VOL_DOWN); _delay_ms(70);}
			#endif

			#ifdef ENABLE_TRACK_KEYS
			if(keypress.row == prev_track.row
			&& keypress.column == prev_track.column)
			{usb_extra_press(PREV_TRACK); _delay_ms(70);}
			if(keypress.row == next_track.row
			&& keypress.column == next_track.column)
			{usb_extra_press(NEXT_TRACK); _delay_ms(70);}
			#endif

			#ifdef ENABLE_PLAY_PAUSE
			//play/pause button
			if(keypress.row == play_pause.row
			&& keypress.column == play_pause.column)
			{play_pause_media();}
			#endif

			#ifdef ENABLE_MUTE
			if(keypress.row == mute_key.row
			&& keypress.column == mute_key.column)
			{mute();}
			#endif

			#ifdef ENABLE_RESET
			if(keypress.row == reset_key.row
			&& keypress.column == reset_key.column)
			{reset = 1;}
			#endif
		}
	}
}
