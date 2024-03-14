#include "usb.h"

// USB General Interrupt Request
//
// This interrupt is triggered on "general interrupts" coming from the USB
// host. If the host is not communicating with any specific endpoint this
// gets triggered. The host will trigger this interrupt on EOR and expects
// that EP0 is enabled at that point. The device will trigger an interrupt
// on SOF packets, which can be used to handle some additional functionality.
ISR(USB_GEN_vect)
{
	uint8_t interrupt = UDINT;
	// UDINT needs to be cleared for the interrupt to exit execution
	UDINT = 0;

	// we cannot use switch / case with interrupt flags
	if (interrupt & (1 << EORSTI))
	{
		UENUM = 0; // set endpoint to control (EP0)
		UECONX = 1 << EPEN; // enable EP0
		UECFG0X = 0; // set endpoint type to control
		UECFG1X = 1 << EPSIZE1 | 1 << EPSIZE0 | 1 << ALLOC; // (ALLOC 0b10) allocates the endpoint (by default endpoint is 8 bytes in size, single bank)
		configuration_ok &= (UESTA0X & (1 << CFGOK)) >> CFGOK; // check that the above settings are sane

		//UEIENX = 1 << RXSTPE | 1 << RXOUTE | 1 << STALLEDE;
		UEIENX = 1 << RXSTPE | 1 << STALLEDE;

		usb_configuration = 0;

		return;
	}

	//if (interrupt & (1 << SOFI))
	//{
	//	// start of frame interrupt should not be called at all when misconfigured, but check state regardless
	//	if (!configuration_ok || !usb_configuration)
	//	{return;}

	//	if (!endpoints_ready)
	//	{return;}

	//	return;
	//}

	//STALL();
}

// USB Endpoint/Pipe Interrupt Communication Request
//
// Here we make sure that all of the USB standard requests (which are
// required by the specification to be handled) are taken care of. We use
// USB_GEN_vect to enable the control endpoint, and those standard requests
// then can be handled in this interrupt. Apart from the standard requests,
// we also handle HID requests as this is an HID device. The interrupts
// handled here are those defined in the UEIENX register and are reflected
// in UEINTX register for each endpoint respectively.
ISR(USB_COM_vect)
{
	uint8_t endpoint = UEINT;

	// iterate through each endpoint that raised an interrupt flag
	for (uint8_t e = 0; e < 7; e++)
	{
		// select endpoint if interrupt triggered, otherwise continue
		if (endpoint & 1)
		{UENUM = e;}
		else
		{
			endpoint >>= 1;
			continue;
		}

		// reset the endpoint if STALLEDI triggered
		if (UEINTX & (1 << STALLEDI))
		{
			HANDSHAKE(STALLEDI);
			UECONX = 1 << STALLRQC | 1 << EPEN;
			UERST = endpoint & 1;
			UERST = 0;
			continue;
		}

		// process SETUP packets here
		if (UEINTX & (1 << RXSTPI))
		{
			struct setup_packet data0 = {0, 0, 0, 0, 0, 0, 0, 0};
			data0.bmRequestType = UEDATX;
			data0.bRequest = UEDATX;
			data0.wValue[0] = UEDATX;
			data0.wValue[1] = UEDATX;
			data0.wIndex[0] = UEDATX;
			data0.wIndex[1] = UEDATX;
			data0.wLength[0] = UEDATX;
			data0.wLength[1] = UEDATX;

			//HANDSHAKE(RXSTPI); // respond that we got the info
			UEINTX = ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));

			usb_setup(&data0);

			if (UEINTX & (1 << TXINI))
			{HANDSHAKE(TXINI);}
			//CLEAR_FIFOCON();

			continue;
		}


		// process IN packets here, packets sent to the host
		if (UEINTX & (1 << TXINI))
		{
			//if (e == 4 && action_ep_state.mode == 1)
			//{
			//	//UENUM = 4;

			//	//WAIT(TXINI);

			//	leddebug(0xFF, 1);

			//	//for (uint8_t d = 0; d < 8; d++)
			//	//{UEDATX = 0;}

			//	//HANDSHAKE(TXINI);
			//	//CLEAR_FIFOCON();

			//	//UENUM = e;
//			//	return;
			//}

			// Some endpoints (the HID ones) are constantly being polled by the
			// the host to see if there are any waiting interrupts. We need to
			// exclude them here as we don't want to send any interrupts until
			// we have data ready in a buffer.
			//const struct hid_descriptor *hid = get_hid_descriptor(e);

			//// we handshake FIFOCON directly here as RWAL may not unset by hardware
			//if (hid != 0)
			//{
			//	if (hid->hid == 0)
			//	{
			//		HANDSHAKE(TXINI);
			//		HANDSHAKE(FIFOCON);
			//	}
			//}

			if (action_ep_state.reply_ep != 0)
			{
				HANDSHAKE(TXINI);
				//if (pgm_read_byte(&(action_ep_state.reply_ep[0])) == e)
				//{
				//	if (usb_debug.length)
				//	{
				//		WAIT(TXINI);
				//		//for (uint8_t b = 0; b < usb_debug.length; b++)
				//		//{UEDATX = usb_debug.buffer[b];}
				//		//UEDATX = usb_debug.buffer[0];
				//		UEDATX = 0xFF;
				//		usb_debug.length = 0;
				//		HANDSHAKE(TXINI);
				//		CLEAR_FIFOCON();
				//		continue;
				//	}

				//	//out_ep_reply(action_ep_state.reply_ep);
				//}

				STALL();

				continue;
			}

			HANDSHAKE(TXINI);
			HANDSHAKE(FIFOCON);
			continue;
		}

		// process OUT packets here, packets received from the host
		if (UEINTX & (1 << RXOUTI))
		{
			if (!configuration_ok || !usb_configuration || !endpoints_ready || e == 0)
			{
				HANDSHAKE(RXOUTI);
				continue;
			}

			const struct hid_descriptor *hid = get_hid_descriptor(e);

			if (hid == 0)
			{
				HANDSHAKE(RXOUTI);
				continue;
			}

			uint16_t bytecount = UEBCHX << 8 | UEBCLX;
			uint8_t *buffer = malloc(bytecount);

			for (uint8_t b = 0; b < bytecount; b++)
			{buffer[b] = UEDATX;}

			HANDSHAKE(RXOUTI);

			action_ep_state.reply_ep = out_ep_handler(buffer, bytecount);

			CLEAR_FIFOCON();

			free(buffer);

			continue;
		}
	}
}

// initializes USB state
void usb_init(void)
{
	// In the atmega32u4 datasheet, it tells us to "perform some of the following operations"
	// those operations are as follows:
	// - Power-On USB pads regulator
	// - Configure PLL interface
	// - Enable PLL
	// - Check PLL lock
	// - Enable USB interface
	// - Configure USB interface (USB speed, Endpoints configuration...)
	// - Wait for USB VBUS information connection
	// - Attach USB device
	// We will perform all of the operations in the order given.

	// (0b1 UVREGE) enables USB pad regulators. I believe this is UVCC, UCAP
	UHWCON = 1 << UVREGE;

	// Wait until VBUS is fully connected. Not entirely necessary but I included it as a redundancy check
	while(!USBSTA & 1);

	USBCON = 1 << USBE | 1 << FRZCLK;

	// enables PLL (0b10 PLLE) and uses 16MHz clock source (0b10000 PINDIV)
	PLLCSR = 1 << PINDIV | 1 << PLLE;

	// wait until PLL is locked (0b1 PLOCK)
	while(!PLLCSR & (1 << PLOCK));

	// enable the USB controller (USBE) and enable VBUS pad (OTGPADE) for VBUS detection
	USBCON = 1 << USBE | 1 << OTGPADE;

	// this sets full-speed mode and attaches the device
	UDCON = 0;

	// we enable general USB interrupts here
	// (EORSTE) triggers when the USB host has finished issuing a reset command. This assumes the device needs 
	// to be reconfigured or has not yet been configured. The configuration is done on endpoint 0 (EP0 = control endpoint)
	// (SOFE) enables SOF (start of frame) interrupts (I believe this is when the host/device sends a general interrupt)
	// As far as I'm aware these must be enabled to negotiate with the host since the device is using interrupt mode.
	// The other interrupts are wakeup/suspend related so they are not relevant here.
	// SOFE is disabled here as it does not need to be enabled as of right now
	//UDIEN = 1 << EORSTE | 1 << SOFE;
	UDIEN = 1 << EORSTE;

	// enable global interrupt flag (SREG register)
	// no interrupts will execute without this
	sei();

	// when endpoints are reset, this must be set to 1
	configuration_ok = 1;

	// setup some variables that must be calculated in runtime
	//*(uint16_t *)wTotalLength = DC_LENGTH + (ID_LENGTH * INTERFACE_COUNT);
	wTotalLength = DC_LENGTH + (ID_LENGTH * INTERFACE_COUNT);

	for (uint8_t i = 0; i < INTERFACE_COUNT; i++)
	{
		for (uint8_t e = 0; e < usb_interfaces[i].hid_count; e++)
		{
			//*(uint16_t *)wTotalLength += ED_LENGTH;
			wTotalLength += ED_LENGTH;
			if (usb_interfaces[i].hid[e].hid != 0)
			//{*(uint16_t *)wTotalLength += HID_LENGTH;}
			{wTotalLength += HID_LENGTH;}
		}
	}

}

// returns the struct which contains hid, report, and endpoint descriptors
const struct hid_descriptor *get_hid_descriptor(uint8_t endpoint)
{
	//const struct hid_descriptor *hid = 0;
	for (uint8_t i = 0; i < descriptors.interface_count; i++)
	{
		const struct usb_interface *interface = &descriptors.interface[i];
		for (uint8_t x = 0; x < interface->hid_count; x++)
		{
			if ((pgm_read_byte(&(interface->hid[x].endpoint[0])) & 0x7) == endpoint)
			{return &interface->hid[x];}
		}
	}

	return 0;
}

//void usb_abort(void)
//{
//	UEIENX ^= 1 << TXINE;
//	if (UESTA0X & 0b11)
//	{UEINTX = 0xFF;}
//	UEIENX ^= 1 << TXINE;
//}

// this processes information sent by the host
const uint8_t *out_ep_handler(uint8_t *buffer, uint16_t length)
{
	uint8_t endpoint = UENUM;
	const uint8_t *reply_endpoint = 0;

	for (uint8_t x = 0; x < REPLY_ENDPOINTS; x++)
	{
		if (pgm_read_byte(&(reply_endpoints[x].out_ep[0])) & 0xF == endpoint)
		{reply_endpoint = reply_endpoints[x].reply_ep;}
	}

	switch (endpoint)
	{
		case 5:
			if (length == 0)
			{
				if (action_ep_state.mode == 0)
				{
					action_ep_state = (struct action_state){1, 0, 0, 0, 0, 0};
					return reply_endpoint;
				}
				else if (action_ep_state.mode == 1 && action_ep_state.action)
				{
					free(action_ep_state.buffer);
					action_ep_state = (struct action_state){0, 0, 0, 0, 0, 0};
					return reply_endpoint;
				}
			}

			if (action_ep_state.mode == 1)
			{
				if (action_ep_state.action == 0 && length == 3)
				{
					action_ep_state.action = buffer[0];
					action_ep_state.length = *((uint16_t *)(&buffer[1]));

					if (action_ep_state.action == 0xFE &&
						action_ep_state.length == 0xFEFE
					) {
						device_reset = 1;
					}

					//action_ep_state.buffer = malloc(action_ep_state.length);

					//if (action_ep_state.buffer != 0)
					//{return // TODO: return reply ep}
				}
				//else if (
				//	action_ep_state.action != 0 &&
				//	action_ep_state.length != 0 &&
				//	action_ep_state.buffer != 0
				//) {
				//	for (uint16_t b = 0; b < length; b++)
				//	{
				//		uint16_t pos = action_ep_state.buffer_pos;
				//		action_ep_state.buffer[pos + b] = buffer[b];
				//	}

				//	action_ep_state.buffer_pos += length;

				//	return // TODO: return reply ep
				//}
			}

			free(action_ep_state.buffer);
			action_ep_state = (struct action_state){0xFF, 0xFF, 0xFF, 0, 0, 0};
			STALL();
			return 0;
	}

	return 0;
}

void out_ep_reply(const uint8_t *endpoint)
{
	uint8_t out_ep = UENUM;
	uint8_t reply_ep = pgm_read_byte(&(endpoint[0])) & 0xF;

	//leddebug(0xF0, 1);

	if (
		action_ep_state.mode == 0xFF &&
		action_ep_state.action == 0xFF &&
		action_ep_state.length == 0xFF
	) {return;}

	if (reply_ep < 7)
	{UENUM = reply_ep;}
	else
	{return;}

	if (action_ep_state.mode == 0 || action_ep_state.mode == 1)
	{
		if (action_ep_state.action == 0 && action_ep_state.length == 0)
		{
			WAIT(TXINI);
			HANDSHAKE(TXINI);
			CLEAR_FIFOCON();
		}
	}

	//leddebug(0xF2, 1);

	UENUM = out_ep;
}

void send_descriptor(struct setup_packet *data0, struct descriptor_stream *stream)
{
	uint8_t wIndex = data0->wIndex[0];
	uint16_t wLength = *((uint16_t *)(data0->wLength));

	// if the requested length is less than stream.length, shorten the transfer length.
	// the true length of the descriptor will still be sent to the host, which should
	// prompt the host to re-request the descriptor with the correct length
	uint16_t transfer_length = (wLength < stream->length) ? wLength : stream->length;

	// buffer index allows us to keep track if the buffer is still open or not, and how
	// much is left to fill in the buffer. I use a pointer so we can modify the property
	// of the descriptor stream directly
	uint8_t *buffer_index = &stream->buffer_index;

	// keeps track of where we are in the descriptor array
	uint8_t data_index = 0;

	// for the main loop, wait on each buffer to open and then send data
	for (uint16_t buffer = 0; buffer <= transfer_length / EP0_SIZE; buffer++)
	{
		// ensure the iteration does not open another buffer when there is no data left
		if (data_index >= transfer_length)
		{break;}

		// wait for buffer to open up
		WAIT(TXINI);

		// use buffer_index to save how much buffer room is left in the current transfer
		for (; *buffer_index < EP0_SIZE; *buffer_index++)
		{
			// if we hit the end of our transfer, exit the iteration
			if (data_index >= transfer_length)
			{break;}
			
			data_index++;

			switch (data_index - 1)
			{
				// bLength
				case 0:
					if (stream->type == TYPE_REPORT)
					{break;}
					UEDATX = stream->length;
					continue;

				// bDescriptorType
				case 1:
					if (stream->type == TYPE_REPORT)
					{break;} 
					UEDATX = stream->type;
					continue;
			}

			if (stream->type == TYPE_CONFIGURATION)
			{
				switch(data_index - 1)
				{
					// wTotalLength
					case 2:
						SEND16(wTotalLength);
					case 3:
						continue;

					// bNumInterfaces
					case 4:
						UEDATX = descriptors.interface_count;
						continue;
				}
			}

			if (stream->type == TYPE_HID)
			{
				switch(data_index - 1)
				{
					// wDescriptorLength
					case 7:
						SEND16(pgm_read_word(&(stream->descriptor[5])));
					case 8:
						continue;
				}
			}

			if (stream->type == TYPE_INTERFACE)
			{
				switch(data_index - 1)
				{
					// bInterfaceNumber
					case 2:
						UEDATX = wIndex;
						break;

					// bNumEndpoints
					case 4:
						UEDATX = descriptors.interface[wIndex].hid_count;
						break;

					// bInterfaceClass
					case 5:
						UEDATX = 0x03;
						break;

					// iInterface
					case 8:
						UEDATX = 0x01;
						break;

					// for unused variables
					default:
						UEDATX = 0;
				}

				continue;
			}

			// anything else (stored in the descriptor arrays)
			UEDATX = pgm_read_byte(&(stream->descriptor[data_index - stream->offset]));
		}

		// if the end of the buffer is reached, or 0, this will equal 0
		// otherwise, the remainder is kept
		*buffer_index %= 8;

		// if the buffer_index is non-zero and the transfer is complete for this descriptor,
		// stop short of clearing the TXINI flag so that more data can be buffered for the 
		// transfer if needed. This is used for the configuration descriptor, and also
		// requires that after use of this function has finished the programmer must clear
		// TXINI outside of this function as a final step, regardless if it has been cleared
		// or not
		if (*buffer_index && transfer_length == data_index)
		{break;}

		HANDSHAKE(TXINI);
	}
}

// this is paired with get_descriptor and is the data transfer stage
//uint8_t send_descriptor(struct setup_packet *data0, uint8_t type, const uint8_t *descriptor, uint16_t descriptor_length, uint8_t offset, uint8_t buffer_start)
//{
//	uint8_t index = data0->wIndex[0];
//	uint16_t length = *((uint16_t *)(data0->wLength));
//	uint16_t true_length = descriptor_length;
//
//	uint8_t len = (length < 256) ? length : 255;
//	if (len > descriptor_length) {len = descriptor_length;}
//	if (len < descriptor_length) {descriptor_length = len;}
//
//	uint8_t n = 0;
//	uint8_t i = 0;
//	uint8_t d = 0;
//
//	do {
//		do {i = UEINTX;} while (! (i & (1 << TXINI | 1 << RXOUTI)));
//		if (i & (1 << RXOUTI)) {return 0xFF;}
//
//		n = len < EP0_SIZE ? len : EP0_SIZE;
//
//		if (len == descriptor_length && buffer_start != 0)
//		{n = EP0_SIZE - buffer_start;}
//
//		// ignore wLength if device descriptor
//		//if (type == TYPE_DEVICE)
//		//{n = descriptor_length;}
//
//		if (type == TYPE_DEVICE)
//		{
//			if (length == 64)
//			{n = descriptor_length;}
//			if (length == 8)
//			{
//				n = 8;
//				descriptor_length = 8;
//				len = 8;
//			}
//		}
//
//		for (i = n; i; i--)
//		{
//			uint8_t c = (descriptor_length - len) + (n - i);
//			// every descriptor has a bLength and bDescriptorType except report descriptors
//			switch (c)
//			{
//				// bLength
//				case 0:
//					if (type == TYPE_REPORT)
//					{break;}
//					UEDATX = true_length;
//					continue;
//
//				// bDescriptorType
//				case 1:
//					if (type == TYPE_REPORT)
//					{break;} 
//					UEDATX = type;
//					continue;
//			}
//
//			if (type == TYPE_CONFIGURATION)
//			{
//				switch(c)
//				{
//					// wTotalLength
//					//case 2:
//					//case 3:
//					//	UEDATX = wTotalLength[c - 2];
//					//	continue;
//					case 2:
//						SEND16(wTotalLength);
//					case 3:
//						continue;
//
//					// bNumInterfaces
//					case 4:
//						UEDATX = descriptors.interface_count;
//						continue;
//				}
//			}
//
//			if (type == TYPE_HID)
//			{
//				switch(c)
//				{
//					// wDescriptorLength
//					case 7:
//						SEND16(pgm_read_word(&(descriptor[5])));
//					case 8:
//						continue;
//				}
//			}
//
//			if (type == TYPE_INTERFACE)
//			{
//				switch(c)
//				{
//					// bInterfaceNumber
//					case 2:
//						UEDATX = index;
//						break;
//
//					// bNumEndpoints
//					case 4:
//						UEDATX = descriptors.interface[index].hid_count;
//						break;
//
//					// bInterfaceClass
//					case 5:
//						UEDATX = 0x03;
//						break;
//
//					// iInterface
//					case 8:
//						UEDATX = 0x01;
//						break;
//
//					// for unused variables
//					default:
//						UEDATX = 0;
//				}
//
//				continue;
//			}
//
//			// anything else (stored in the descriptor arrays)
//			UEDATX = pgm_read_byte(&(descriptor[c - offset]));
//		}
//
//		len -= n;
//
//		//if (length == *((uint16_t *)(wTotalLength)) && n != EP0_SIZE)
//		//{return n;}
//		//if (n == 0)
//		//{break;}
//		if (! len && n != EP0_SIZE)
//		{break;}
//
//		HANDSHAKE(TXINI);
//	} while (len || n == EP0_SIZE);
//
//	return n;
//}

// return descriptor information to the host, depending on descriptor type
void get_descriptor(struct setup_packet *data0)
{
	uint8_t type = data0->wValue[1];
	uint8_t index = data0->wValue[0];
	uint16_t length = *((uint16_t *)(data0->wLength));

	const uint8_t no_descriptor = 0;
	//const uint8_t *descriptor = &no_descriptor;
	//uint16_t descriptor_length = 0;
	//uint8_t offset = 2;

	// below is a struct that keeps track of which descriptor we are on as well as
	// the current buffer index of the register, the offset (which skips descriptor
	// variables which are calculated in real-time), the current index of the descriptor
	// and the length of the descriptor.
	// The first two indicies, bLength and bDescriptorType, are always omitted, giving
	// an offset of at least 2. This prevents a lot of manual reconfiguration of
	// specific descriptor settings, allowing for basically any USB configuration
	// supported by USB 2.0. Just add and reference your descriptors in the 
	// `descriptors` variable in the usb.h file and the program will take care of
	// the annoying details.
	struct descriptor_stream stream = {
		&no_descriptor, // selected descriptor
		type, // descriptor type
		0, // descriptor length
		0, // current index
		2, // current offset, always 2 unless otherwise specified
		0  // current index of the data register
	};

	uint8_t endpoints = 0; // number of endpoints available

	switch(type)
	{

		case TYPE_DEVICE:
			if (index == 0)
			{
				//descriptor = descriptors.device;
				//descriptor_length = DD_LENGTH;
				stream.descriptor = descriptors.device;
				stream.length = DD_LENGTH;
			}
			break;

		case TYPE_CONFIGURATION:
			if (index == 0)
			{
				//descriptor = descriptors.configuration;
				//descriptor_length = DC_LENGTH;
				stream.descriptor = descriptors.configuration;
				stream.length = DC_LENGTH;
			}
			//offset = 5;
			stream.offset = 5;
			break;

		case TYPE_STRING:
			if (index == 0)
			{
				WAIT(TXINI);
				UEDATX = 4;
				UEDATX = TYPE_STRING;
				SEND16(LANGID);

				//UEDATX = 2;
				//UEDATX = TYPE_STRING;

				HANDSHAKE(TXINI);
				//CLEAR_FIFOCON();
				return;
			}
			else if (index <= STRING_COUNT)
			{
				//descriptor = (const uint8_t *)vendor_strings[index - 1].string;
				//descriptor_length = vendor_strings[index - 1].length;
				stream.descriptor = (const uint8_t *)vendor_strings[index - 1].string;
				stream.length = vendor_strings[index - 1].length;
			}
			else
			{STALL();}

			break;

		case TYPE_ENDPOINT:
		case TYPE_HID:
		case TYPE_REPORT:
			//endpoints = 0; // number of endpoints available
			//for (uint8_t i = 0; i < descriptors.interface_count; i++)
			//{endpoints += descriptors.interface[i].hid_count;}

			//uint8_t endpoint = 0; // the current endpoint we are on

			// iterate through the interfaces, and return the one requested
			for (uint8_t i = 0; i < descriptors.interface_count; i++)
			{
				const struct usb_interface *interface = &descriptors.interface[i];

				uint8_t interface_index = data0->wIndex[0];

				if (interface_index == i)
				{
					// endpoint, hid, report resolve similarly, but return differing descriptors
					const struct hid_descriptor *hid = &interface->hid[index];

					switch(type)
					{
						case TYPE_ENDPOINT:
							//descriptor = hid->endpoint;
							//descriptor_length = ED_LENGTH;
							stream.descriptor = hid->endpoint;
							stream.length = ED_LENGTH;
							break;

						case TYPE_HID:
							//descriptor = hid->hid;
							//descriptor_length = HID_LENGTH;
							stream.descriptor = hid->hid;
							stream.length = HID_LENGTH;
							break;

						case TYPE_REPORT:
							// report is the only exception to the offset variable
							stream.offset = 0;
							stream.descriptor = hid->report;
							//offset = 0;
							//descriptor = hid->report;
							// Report length is stored in the HID descriptor
							// as wDescriptorLength.
							// Takes into account HID_LENGTH - 2
							//
							//descriptor_length = pgm_read_byte(&(hid->hid[5]));
							stream.length = pgm_read_word(&(hid->hid[5]));

					}

					break;
				}

				//endpoint += interface->hid_count;
			}
			break;

		case TYPE_DEVICE_QUALIFIER:
		case TYPE_OTHER_SPEED_CONFIGURATION:
		case TYPE_INTERFACE_POWER:
		case TYPE_PHYSICAL:
			// not implemented
			STALL();
			break;

	}

	// no descriptor selected means something went wrong
	//if (descriptor == &no_descriptor)
	if (stream.descriptor == &no_descriptor)
	{
		STALL();
		return;
	}

	// spaghetti code
	//if (type == TYPE_CONFIGURATION && length == *((uint16_t *)(wTotalLength)))
	if (stream.type == TYPE_CONFIGURATION && stream.length == wTotalLength)
	{
		uint8_t buffer_start = 0;
		//WAIT(TXINI);
		//buffer_start = send_descriptor(data0, type, descriptor, descriptor_length, offset, buffer_start);
		send_descriptor(data0, &stream);
		//offset = 2;
		stream.offset = 2;
		for (uint8_t i = 0; i < descriptors.interface_count; i++)
		{
			// descriptors.interface is generic and describes multiple interfaces
			const struct usb_interface *interface = &descriptors.interface[i];
			// save the current wIndex value
			uint8_t index = data0->wIndex[0];
			// set the wIndex to what interface we are sending the host
			data0->wIndex[0] = i;
			//buffer_start = send_descriptor(data0, TYPE_INTERFACE, descriptor, ID_LENGTH, offset, buffer_start);
			// update stream.type and stream.length
			// for type interface, stream.descriptor is ignored
			stream.type = TYPE_INTERFACE;
			stream.length = ID_LENGTH;
			send_descriptor(data0, &stream);
			// restore the wIndex value
			data0->wIndex[0] = index;
			for (uint8_t e = 0; e < interface->hid_count; e++)
			{
				// retrieve the `hid` struct
				const struct hid_descriptor *hid = &interface->hid[e];
				// this skips the HID descriptor if it does not exist
				if (hid->hid != 0)
				//{buffer_start = send_descriptor(data0, TYPE_HID, hid->hid, HID_LENGTH, offset, buffer_start);}
				{
					stream.type = TYPE_HID;
					stream.length = HID_LENGTH;
					stream.descriptor = hid->hid;
					send_descriptor(data0, &stream);
				}
				// send the endpoint descriptor
				stream.type = TYPE_ENDPOINT;
				stream.length = ED_LENGTH;
				stream.descriptor = hid->endpoint;
				//buffer_start = send_descriptor(data0, TYPE_ENDPOINT, hid->endpoint, ED_LENGTH, offset, buffer_start);
				send_descriptor(data0, &stream);
			}
		}
		//HANDSHAKE(TXINI);
		//CLEAR_FIFOCON();
	}
	else
	{
		//WAIT(TXINI);
		//if (send_descriptor(data0, type, descriptor, descriptor_length, offset, 0) == 0xFF)
		//{return;}

		// if host is only querying one descriptor and wLength value exceeds descriptor length clamp it to descriptor length
		//if (length > descriptor_length)
		//{*((uint16_t *)(data0->wLength)) = descriptor_length;}

		//send_descriptor(data0, type, descriptor, descriptor_length, offset, 0);
		send_descriptor(data0, &stream);
	}

	HANDSHAKE(TXINI);
	//HANDSHAKE(FIFOCON);
}

// enables all defined endpoints
uint8_t activate_endpoints(void)
{
	uint8_t endpoint_num = 0;
	uint8_t endpoint_type = 0;
	endpoints_ready = 1;

	for (uint8_t i = 0; i < INTERFACE_COUNT; i++)
	{
		for (uint8_t e = 0; e < usb_interfaces[i].hid_count; e++)
		{
			const struct hid_descriptor *endpoint = &usb_interfaces[i].hid[e];
			endpoint_num = pgm_read_byte(&(endpoint->endpoint[0])) & 0xF;
			endpoint_type = pgm_read_byte(&(endpoint->endpoint[0])) & 0x80;

			UENUM = endpoint_num;
			UECONX = 1 << EPEN;

			// interrupt mode
			uint8_t direction = endpoint_type >> 7;
			UECFG0X = 0x3 << EPTYPE0 | direction;

			// sets the endpoint buffer size, single bank mode
			//UECFG1X = endpoint->ep_size << EPSIZE0 | 1 << EPBK0 | 1 << ALLOC;

			// You could use double bank but it is pointless in this scenario.
			// The only time it is useful is large data transfers for something
			// like isochoronous transfers.
			UECFG1X = endpoint->ep_size << EPSIZE0 | 0 << EPBK0 | 1 << ALLOC;
			configuration_ok &= (UESTA0X & (1 << CFGOK)) >> CFGOK;

			// Some endpoints (the HID ones) are constantly being polled by the
			// the host to see if there are any waiting interrupts. We need to
			// exclude them here as we don't want to send any interrupts until
			// we have data ready in a buffer.
			const struct hid_descriptor *hid = get_hid_descriptor(endpoint_num);
			uint8_t enable_in = 0;
			//if (hid != 0)
			//{
			//	if (hid->hid == 0)
			//	{enable_in = 1;}
			//}

			UEIENX = (direction ^ 1) << RXOUTE | enable_in << TXINE;
			//UEIENX = (direction ^ 1) << RXOUTE;

			//if (endpoint_num == 4)
			//{leddebug(UEIENX, 1);}
			//UEIENX = (direction ^ 1) << RXOUTE;

			if (!configuration_ok)
			{endpoints_ready = 0;}
		}
	}

	// reset all endpoints except endpoint 0
	UERST = 0x7E;
	UERST = 0;

	return endpoints_ready;
}

// Send report data with the syntax specified by the target interface
// report descriptors.
void send_report(uint8_t iface_num)
{
	// if endpoint configuration failed or usb configuration failed, do nothing
	if (!configuration_ok || !usb_configuration)
	{return;}

	//if (sent_reports < REPORT_COUNT)
	//{return;}

	// enable all endpoints before continuing
	//if (!endpoints_ready)
	//{return;}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		const struct usb_interface *interface = &descriptors.interface[iface_num];
		uint8_t address = pgm_read_byte(&(interface->hid[0].endpoint[0])) & 0xF;
		uint8_t timeout = UDFNUML + (pgm_read_byte(&(interface->hid[0].endpoint[4])) * 4);
		//leddebug(address, 1);

		UENUM = address;

		// wait until read/write allow bit is set
		// WAIT() can be used as a while loop
		WAIT(RWAL)
		{
			NONATOMIC_BLOCK(NONATOMIC_RESTORESTATE)
			{
				if (UDFNUML == timeout)
				{return;}
			}
		}

		HANDSHAKE(TXINI);

		switch(iface_num)
		{

			// send our keyboard reports here
			case KBD_INTERFACE:
				UEDATX = keyboard_modifier_keys;
				UEDATX = 0;
				for (uint8_t k = 0; k < KEY_ROLLOVER; k++)
				{UEDATX = keyboard_keys[k].keycode;}

				break;

			case APP_INTERFACE:
				consumer_key.previous = consumer_key.current;
	
				if (consumer_key.current != consumer_key.previous)
				{SEND16(consumer_key.current);}
				else
				{UEDATX = 0; UEDATX = 0;}

				break;

			// send mouse reports here
			case MSE_INTERFACE:
				UEDATX = mouse.buttons & 0x7;
				UEDATX = mouse.x_axis;
				UEDATX = mouse.y_axis;

				break;
	
			// "vendor" endpoint (communicates with software on host)
			case VEN_INTERFACE:
				//if (vendor_buffer.state & 1)
				//{
				//	for (uint8_t d = 0; d < 8; d++)
				//	{UEDATX = vendor_buffer.data[d];}
				//}

				for (uint8_t b = 0; b < usb_debug.length; b++)
				{UEDATX = usb_debug.buffer[b];}

				//UEDATX = usb_debug.buffer[0];

				//for (uint8_t b = 0; b < 8; b++)
				//{UEDATX = usb_debug.buffer[0];}

				break;
		}

		// no need to wait for RWAL with CLEAR_FIFOCON here
		HANDSHAKE(FIFOCON);
	}
}

// is used a few times by different request types
void get_status(void)
{
	// wait until USB host sends IN packet
	WAIT(TXINI);
	// GET_STATUS expects 2 bytes, we don't
	// have any special configuration so just
	// leave it at 0.
	UEDATX = 0;
	UEDATX = 0;
	// setting TXINI to 0 will handshake a IN packet
	HANDSHAKE(TXINI);
	//CLEAR_FIFOCON();
}

// This is the logic for resolving responses for SETUP packets
void usb_setup(struct setup_packet *data0)
{
	switch(0x7F & data0->bmRequestType)
	{

		case DEVICE_REQUEST:
			if (0x80 & data0->bmRequestType)
			{
				switch(data0->bRequest)
				{

					case GET_STATUS:
						get_status();
						return;

					case GET_DESCRIPTOR:
						get_descriptor(data0);
						return;

					case GET_CONFIGURATION:
						// we need everything to be 0 except data0->wLength[0]
						if (data0->wLength[0] != 1)
						{STALL(); return;}

						if (
							*(uint16_t *)data0->wValue +
							*(uint16_t *)data0->wIndex
						)
						{STALL(); return;}

						WAIT(TXINI);
						UEDATX = usb_configuration;
						HANDSHAKE(TXINI);
						//CLEAR_FIFOCON();
						return;

					default:
						STALL();
						return;
				}
			}
			else
			{
				switch(data0->bRequest)
				{

					case SET_ADDRESS:
						// stall here as behaviour is undefined
						if (usb_configuration)
						{STALL(); return;}

						UDADDR = data0->wValue[0];

						HANDSHAKE(TXINI);
						WAIT(TXINI);

						UDADDR = (1 << ADDEN) | data0->wValue[0];
						HANDSHAKE(TXINI); // send zero length packet
						return;

					case SET_CONFIGURATION:
						if (*((uint16_t *)(data0->wLength)))
						{STALL(); return;}

						usb_configuration = data0->wValue[0];
						HANDSHAKE(TXINI);

						// quick note about activating USB endpoints, that is not
						// mentioned in the atmega32u4 datasheet, is that USB
						// expects the endpoints to be activated "as soon as
						// possible" after usb_configuration has been set. I read
						// this as "when the endpoints need to be used" and
						// activated them right before the keyboard IN interrupt.
						// If the activation takes too long requests are made to
						// endpoints that are not enabled, the USB host will fail
						// to communicate any interrupt data. This failure state
						// as far as I can tell cannot be recovered from so enabling
						// endpoints directly after setting the configuration is
						// the only reasonable solution.
						activate_endpoints();
						return;

					case CLEAR_FEATURE:
					case SET_FEATURE:
					case SET_DESCRIPTOR:
					default:
						// This should be implemented for full USB 
						// spec compliance but I ain't got time for that.
						// Maybe later.
						STALL();
						return;
				}
			}

			STALL();
			return;

		case INTERFACE_REQUEST:
			if (0x80 & data0->bmRequestType)
			{
				switch(data0->bRequest)
				{
					case GET_STATUS:
						get_status(); // must always send two 0 values
						return;

					case GET_INTERFACE:
						// not implemented
						STALL();
						return;

					case GET_DESCRIPTOR:
						get_descriptor(data0);
						return;
				}
			}
			else
			{
				switch(data0->bRequest)
				{
					case CLEAR_FEATURE:
					case SET_FEATURE:
					case SET_INTERFACE:
						// not implemented
						STALL();
						return;

				}
			}

			STALL();
			return;

		case ENDPOINT_REQUEST:
			if (0x80 & data0->bmRequestType)
			{
				switch(data0->bRequest)
				{
					case GET_STATUS:
						// you can enable a "halt" feature here.
						// that feature is not used.
						get_status();
						return;

					case SYNCH_FRAME:
						// this is for isochronous transfers, not implemented
						STALL();
						return;

					// Windows will request report descriptors using an endpoint request
					case GET_DESCRIPTOR:
						get_descriptor(data0);
						return;

				}
			}
			else
			{
				switch(data0->bRequest)
				{
					case CLEAR_FEATURE:
					case SET_FEATURE:
						// not implemented
						STALL();
						return;

				}
			}

			STALL();
			return;

		case CLASS_REQUEST:
			if (0x80 & data0->bmRequestType)
			{
				switch(data0->bRequest)
				{
					// This allows the host to receive device reports
					// via the default control pipe. We use this to
					// respond to occasional polling from the host...
					// I'm not sure if there is any reason at all to
					// use this if I am reading the HID spec correctly,
					// but I don't think you can reply with a stall here.
					case GET_REPORT:
						WAIT(TXINI);
						send_report(data0->wIndex[0] - 1);
						return;

					// For full HID compliance this would check for the
					// "Report ID" setting but since we do not use that
					// in any of the descriptors it is ignored completely
					// here.
					case GET_IDLE:
						WAIT(TXINI);
						// if get idle is requested, tell the host we have
						// no idle timeout
						UEDATX = 0;
						HANDSHAKE(TXINI);
						//CLEAR_FIFOCON();
						return;

					// This tells devices that are requesting the boot
					// protocol what protocol the device is using. HID
					// spec requires this to be 1 initially, but host
					// can later set the value with SET_PROTOCOL.
					case GET_PROTOCOL:
						WAIT(TXINI);
						UEDATX = device_protocol;
						HANDSHAKE(TXINI);
						return;

				}
			}
			else
			{
				switch(data0->bRequest)
				{

					case SET_PROTOCOL:
						device_protocol = data0->wValue[0];
						HANDSHAKE(TXINI);
						return;

					// this was a HUGE PAIN to figure out since atmega32u4 datasheet does not explain it properly at all
					case SET_REPORT:
						// set "transmit ready"
						HANDSHAKE(TXINI);

						// wait until data is sent by host
						WAIT(TXINI);

						// read data
						led_state = UEDATX;

						// set the LEDs
						usb.numlock = led_state & 1;
						usb.capslock = (led_state & 2) >> 1;
						usb.scrolllock = (led_state & 4) >> 1;

						// clear TXINI so we can exit with RXOUTI
						HANDSHAKE(TXINI);

						// tell host we received data
						HANDSHAKE(RXOUTI);
						return;

					// refuse set idle request. This is not required to
					// be implemented and rarely used.
					case SET_IDLE:
					default:
						// not implemented
						STALL();
						return;
				}
			}

			STALL();
			return;
	}
}
