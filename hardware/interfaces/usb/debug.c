#include <interfaces/usb/debug.h>

void usb_debug_flush(void)
{
	if (! usb_debug.length)
	{return;}
	uint8_t ep = UENUM;
	send_report(3);
	usb_debug.length = 0;
	UENUM = ep;
}

void usb_debug_write(uint8_t data)
{
	if (usb_debug.length >= 8)
	{usb_debug_flush();}

	usb_debug.buffer[usb_debug.length] = data;
	usb_debug.length++;

	//usb_debug.buffer[0] = data;
}
