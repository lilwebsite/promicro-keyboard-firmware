#include "kbd.h"
#include <interfaces/usb/keys.h>
#include <util/atomic.h>

int main(void)
{
	usb_init();

	init();

	while(1)
	{
		if (kbd.state) {kbsend();}

		scan();

		// this triggers a MCU reset, set via USB endpoint or can be triggered by a key or input
		if(device_reset)
		{DEVICE_RESET();}
	}

	return 1;
}
