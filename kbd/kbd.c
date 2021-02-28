int main(void)
{
	while(!init_promicro());
	usb_init();
	while(!usb_configured());

	init_driver();

	while(1)
	{
		#ifndef PROTOCOL
		set_row();
		#endif

		scan_signal();

		kbsend();
		reset_sending();
	}

	return 1;
}
