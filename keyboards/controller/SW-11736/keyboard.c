#include <keyboard.h>
#include <layout.h>

extern void usb_debug_write(uint8_t data);
extern void usb_debug_flush(void);

void init(void)
{
	twi_init();

	pin_input(F, 0, 0); // MAN
	pin_input(F, 1, 0); // AUTO
	pin_input(F, 4, 0); // LSHIFT
	pin_input(F, 5, 0); // SPACE
	pin_input(F, 6, 0); // RSHIFT

	pin_input(B, 7, 0); // EMD

	//for (uint8_t x = 0; x < 3; x++)
	//{
	//	slave_address = TWI_BASE_ADDR | x << 1;
	//	struct twi_target PORT0_DIR = SET_TARGET(CFG_PORT_0);
	//	struct twi_target PORT1_DIR = SET_TARGET(CFG_PORT_1);
	//}
}

void scan(void)
{
	kbd.state = 0;

	uint8_t pins[3][2];

	for (uint8_t slave = 0; slave < 3; slave++)
	{
		*((uint16_t*)pins[slave]) = PCA9535_READ_PINS(slave);

		if (pins[slave][0] == 0 && pins[slave][1] == 0)
		{continue;}
	}

	direct = PINF;

	// EMD will basically never trigger, unless massive amounts of electromagnetic
	// radiation hit the board. I was not able to get this to trigger with a magnet,
	// but my schematic matches that of the one I found online, so should trigger
	// if the interference is high enough.
	EMD = read_pin(B, 7) ^ 1;

	for (uint8_t x = 0; x < KEYS; x++)
	{
		uint8_t data = 0;
		uint8_t offset = x % 8;
		uint8_t last_state = received[x];

		if (EMD)
		{
			received[x] = 0;
			continue;
		}

		if (x < 8)
		{data = pins[0][0];}
		if (x >= 8 && x < 16)
		{data = pins[0][1];}

		if (x >= 16 && x < 24)
		{data = pins[1][0];}
		if (x >= 24 && x < 32)
		{data = pins[1][1];}

		if (x >= 32 && x < 40)
		{data = pins[2][0];}
		if (x >= 40 && x < 48)
		{data = pins[2][1];}

		switch(x)
		{
			case 48:
				data = direct & 0x01;
				break;
			case 49:
				data = direct & 0x02;
				break;
			case 50:
				data = direct & 0x10;
				offset = 4;
				break;
			case 51:
				data = direct & 0x20;
				offset = 5;
				break;
			case 52:
				data = direct & 0x40;
				offset = 6;
		}

		received[x] = (data >> offset) & 1;
		//else
		//{
		//	if (offset > 1)
		//	{received[x] = (direct >> offset) & 1;}
		//	else
		//	{received[x] = ((direct >> 3) >> offset) & 1;}
		//}

		kbd.state |= last_state ^ received[x];
	}

	if (!kbd.state)
	{return;}

	usb_debug_write(0xEE);
	usb_debug_write(kbd.state);
	usb_debug_flush();

	reset_sending();

	for (uint8_t x = 0; x < KEYS; x++)
	{
		struct keypress target = {pgm_read_byte(&(keymap[x])), x, x};
		kbd_press_release(received[x], target);
		if (received[x])
		{
			usb_debug_write(0x0C);
			usb_debug_write(x);
			usb_debug_flush();
		}
	}
}
