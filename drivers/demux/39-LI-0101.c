#include <kbd/globals.h>
#include "39-LI-0101.h"

void scan(void)
{
	uint8_t step;

	kbsend();

	state_changed = 0;

	// this is computed pretty fast, but could be more efficient if pre-computed into memory then referenced 
	for(step = 0; step < 128; step++)
	{
		// compute output 2
		output_lines ^= 1;
		// ignore output 2 (0) in loop
		for(uint8_t output = 1; output < 8; output++)
		{
			// if `step` modulo `length` is 0
			if(!(step % interval_length[output]))
			{
				// exception for output 9 (7)
				switch(output)
				{
					case 7:
						if(!step)
						{output_lines &= 0x7F; break;}
					default:
						output_lines ^= 1 << output;
				}
			}
		}

		// temporary test code

		//uint8_t abank_remap =
		ABANK_OLAT.data =
		(0x8 & output_lines) >> 3 // new 5 -> old 3 (0x1)
		| (0x10 & output_lines) >> 3 // new 6 -> old 4 (0x2)
		| (0x20 & output_lines) >> 3 // new 7 -> old 5 (0x4)
		| (0x40 & output_lines) >> 3 // new 8 -> old 6 (0x8)
		// skip feedback (0x10), output 3 set on old feedback
		| 0x10
		//| 0x20 // new 1 -> old 8 (0x20)
		| (0x1 & output_lines) << 6; // new 2 -> old 9 (0x40)
		// 0x80 not connected

		//uint8_t bbank_remap = 
		BBANK_OLAT.data =
		(0x4 & output_lines) << 4 // new 4 -> old 1
		| (0x80 & output_lines); // new 9 -> old 2

		set_PINX_variable_output(1, B, (output_lines & 0x2) >> 1); // set output 3
		twi_action(&BBANK_OLAT);
		twi_action(&ABANK_OLAT);

		//// set output pins to computed state, yes it's that easy
		//ABANK_OLAT.data = 0x7F & (output_lines >> 1);
		//twi_action(&ABANK_OLAT);

		//// then set outputs 1 and 2
		//BBANK_OLAT.data = 0xC0 & ((output_lines << 7) | (1 << 6));
		//twi_action(&BBANK_OLAT);

		ABANK_OLAT.data |= 0x20;
		twi_action(&ABANK_OLAT);

		// read from all data pins
		//data = read_PINX_bit(pin_data.position, pin_data.port);
		data = PINF;
		data = (data & (1 << 7)) >> 7;

		ABANK_OLAT.data &= 0xDF;
		twi_action(&ABANK_OLAT);

		data |= 1 << read_PINX_bit(pin_in1.position, pin_in1.port)
		| 2 << read_PINX_bit(pin_in2.position, pin_in2.port)
		| 3 << read_PINX_bit(pin_in3.position, pin_in3.port);

		// invert data pin on feedback
		//set_PINX_variable_output(
		//	pin_feedback.position,
		//	pin_feedback.port,
		//	(data & 1) ^ 1
		//);

		ABANK_OLAT.data = (ABANK_OLAT.data & 0xEF) | ((data & 1) ^ 1) << 4;
		twi_action(&ABANK_OLAT);

		// use data to read matrix
		// TODO
		state_changed |= received[step] ^ (data & 1);
		received[step] = data & 1;

		// reset feedback and output 1
		//ABANK_OLAT.data = 0;
		ABANK_OLAT.data &= 0xDF;
		twi_action(&ABANK_OLAT);

		//BBANK_OLAT.data &= 0xBF;
		//twi_action(&BBANK_OLAT);

		//set_PINX_variable_output(
		//	pin_feedback.position,
		//	pin_feedback.port,
		//	1
		//);
	}

	if(!state_changed)
	{return;}

	reset_sending();

	for(uint8_t x = 0; x < KEYS; x++)
	{
		struct keypress target = {keymap[x], x, x};
		kbd_press_release(received[x], target);
	}
}
