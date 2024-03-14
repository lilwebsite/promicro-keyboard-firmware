#include <keyboard.h>
#include <layout.h>

extern void usb_debug_write(uint8_t data);
extern void usb_debug_flush(void);

extern void kbd_single_press(uint8_t keycode, uint8_t modifier);

void init(void)
{
	// polling output
	pin_output(C, 6);
	pin_output(C, 7);
	pin_output(B, 0);
	pin_output(B, 1);
	pin_output(B, 2);
	pin_output(B, 3);
	pin_output(B, 4);
	pin_output(B, 5);
	pin_output(B, 6);
	pin_output(E, 2);

	// data input
	pin_input(F, 7, 0);

	// tactile buttons
	pin_input(D, 0, 1);
	pin_input(D, 1, 1);
	pin_input(D, 2, 1);
	pin_input(D, 3, 1);
	pin_input(D, 4, 1);
	pin_input(D, 5, 1);

	// leds
	pin_output(F, 6);
	pin_output(F, 5);
	pin_output(F, 4);
}

void scan(void)
{
	kbd.state = 0;

	for (uint8_t step = 0; step < 128; step++)
	{
		output_lines ^= 1;
		for (uint8_t output = 1; output < 8; output++)
		{
			if (! (step % interval_length[output]))
			{
				switch(output)
				{
					case 7:
						if (!step)
						{output_lines &= 0x7F; break;}
					default:
						output_lines ^= 1 << output;
				}
			}
		}

		pin_value(C, 6, 0);                          // OUT1
		pin_value(C, 7, output_lines & 0x01);        // OUT2
		pin_value(B, 0, (output_lines & 0x02) >> 1); // OUT3
		pin_value(B, 1, (output_lines & 0x04) >> 2); // OUT4
		pin_value(B, 2, (output_lines & 0x08) >> 3); // OUT5
		pin_value(B, 3, (output_lines & 0x10) >> 4); // OUT6
		pin_value(B, 4, (output_lines & 0x20) >> 5); // OUT7
		pin_value(B, 5, (output_lines & 0x40) >> 6); // OUT8
		pin_value(B, 6, (output_lines & 0x80) >> 7); // OUT9

		// enable OUT1 and read data
		pin_value(C, 6, 1);
		data = read_pin(F, 7);

		// invert feedback pin
		pin_value(E, 2, (data & 1) ^ 1);

		kbd.state |= received[step] ^ (data & 1);
		received[step] = data & 1;
	}

	// read the tactile buttons
	for (uint8_t x = 0; x < 6; x++)
	{
		uint8_t value = read_pin(D, x) >> x;
		value ^= 1;

		kbd.state |= tactile_state ^ (value << x);
		if (value)
		{tactile_state |= 1 << x;}
		else
		{tactile_state &= ~(1 << x);}
	}

	// set LEDs
	pin_value(F, 6, usb.numlock ^ 1);
	pin_value(F, 5, usb.scrolllock ^ 1);
	pin_value(F, 4, usb.capslock ^ 1);

	// no need to loop and assign 128 keys if nothing changed
	if (!kbd.state)
	{return;}

	reset_sending();

	for (uint8_t x = 0; x < KEYS; x++)
	{
		struct keypress target = {pgm_read_byte(&(keymap[x])), x, x};
		kbd_press_release(received[x], target);
	}

	for (uint8_t x = 0; x < 6; x++)
	{
		struct keypress target = {pgm_read_byte(&(tactile_buttons[x])), 0xFE, x};
		kbd_press_release((tactile_state >> x) & 1, target);
	}
}
