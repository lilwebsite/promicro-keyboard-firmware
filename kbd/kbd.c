#include "kbd.h"

int main(void)
{
	while(!init_promicro());
	usb_init();
	while(!usb_configured());

	init();

	while(1)
	{
		#ifndef NO_MATRIX
		reset_keys();//key queues need to be completely reset before loop starts from beginning
		if(kbd.last != kbd.state){kbd.last = kbd.state; kbd.changed = 1;}
		if(kbd.changed){kbsend();}//if the keyboard state has changed send the keys
		kbd.state = 0;
		#endif

		scan();
		//#ifdef ENABLE_LAYERS
		//if any of the layer keys were pressed this applies them to the variable `layer`
		//#endif

		if(reset)
		{
			do
			{
				wdt_enable(WDTO_15MS);
				for(;;) {}
			} while(0);
		}

		#ifndef NO_MATRIX
		//re-initialize the inputs
		init_inputs(input_pins);
		#endif
	}

	return 1;
}
