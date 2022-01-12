/* USB Keyboard Example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2009 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Version 1.0: Initial Release
// Version 1.1: Add support for Teensy 2.0

#define USB_PRIVATE_INCLUDE
#include "usb.h"

//Mac OS-X and Linux automatically load the correct drivers.  On
//Windows, even though the driver is supplied by Microsoft, an
//INF file is needed to load the driver. These numbers need to
//match the INF file.
#define VENDOR_ID		0x16C0
#define PRODUCT_ID		0x047C

//USB devices are supposed to implment a halt feature, which is
//rarely (if ever) used.  If you comment this line out, the halt
//code will be removed, saving 102 bytes of space (gcc 4.3.0).
//This is not strictly USB compliant, but works with all major
//operating systems.
#define SUPPORT_ENDPOINT_HALT

#define REPORT_ID_CONSUMER  3

/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define KEYBOARD_INTERFACE	0
#define KEYBOARD_ENDPOINT	1
#ifndef PACKET_SIZE
#define PACKET_SIZE			32
#endif
#define KEYBOARD_BUFFER		EP_DOUBLE_BUFFER

#define EXTRA_INTERFACE		1
#define EXTRA_ENDPOINT		2
#define EXTRA_SIZE			8
#define EXTRA_BUFFER		EP_DOUBLE_BUFFER

#ifdef DEBUG_PRINT
#define DEBUG_TX_INTERFACE	2
#define DEBUG_TX_ENDPOINT	3
#define DEBUG_TX_SIZE		32
#define DEBUG_TX_BUFFER		EP_DOUBLE_BUFFER
#endif


static const uint8_t PROGMEM endpoint_config_table[] = {
	KEYBOARD_ENDPOINT, EP_TYPE_INTERRUPT_IN, EP_SIZE(PACKET_SIZE) | KEYBOARD_BUFFER,
	EXTRA_ENDPOINT, EP_TYPE_INTERRUPT_IN, EP_SIZE(EXTRA_SIZE) | EXTRA_BUFFER,
	#ifdef DEBUG_PRINT
	DEBUG_TX_ENDPOINT, EP_TYPE_INTERRUPT_IN,  EP_SIZE(DEBUG_TX_SIZE) | DEBUG_TX_BUFFER,
	#endif
	0
};

/**************************************************************************
*
*  Descriptor Data
*
**************************************************************************/

//Descriptors are the data that your computer reads when it auto-detects
//this USB device (called "enumeration" in USB lingo).  The most commonly
//changed items are editable at the top of this file.  Changing things
//in here should only be done by those who've read chapter 9 of the USB
//spec and relevant portions of any USB class specifications!
static const uint8_t PROGMEM device_descriptor[18] = {
	18,									//bLength
	1,									//bDescriptorType
	0x00, 0x02,							//bcdUSB
	0,									//bDeviceClass
	0,									//bDeviceSubClass
	0,									//bDeviceProtocol
	ENDPOINT_SIZE,						//bMaxPacketSize
	LSB(VENDOR_ID), MSB(VENDOR_ID),		//idVendor
	LSB(PRODUCT_ID), MSB(PRODUCT_ID),	//idProduct
	0x00, 0x01,							//bcdDevice
	1,									//iManufacturer
	2,									//iProduct
	0,									//iSerialNumber
	1									//bNumConfigurations
};

//Keyboard Protocol 1, HID 1.11 spec, Appendix B, page 59-60
static const uint8_t PROGMEM keyboard_hid_report_desc[46] = {
	0x05, 0x01,					// USAGE_PAGE (Generic Desktop)
	0x09, 0x06,					// USAGE (Keyboard)
	0xa1, 0x01,					// COLLECTION (Application)
	0x05, 0x07,					//   USAGE_PAGE (Keyboard)
	0x19, 0xe0,					//   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7,					//   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,					//   LOGICAL_MINIMUM (0)
	0x25, 0x01,					//   LOGICAL_MAXIMUM (1)
	0x75, 0x01,					//   REPORT_SIZE (1)
	0x95, 0x08,					//   REPORT_COUNT (8)
	0x81, 0x02,					//   INPUT (Data,Var,Abs)
	0x95, 0x01,					//   REPORT_COUNT (1)
	0x75, 0x08,					//   REPORT_SIZE (8)
	0x81, 0x03,					//   INPUT (Cnst,Var,Abs)
	0x95, REPORT_COUNT,			//   REPORT_COUNT (30)
	0x75, 0x08,					//   REPORT_SIZE (8)
	0x15, 0x00,					//   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,			//   LOGICAL_MAXIMUM (255)
	0x05, 0x07,					//   USAGE_PAGE (Keyboard)
	0x19, 0x00,					//   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,					//   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,					//   INPUT (Data,Ary,Abs)
	0xc0						// END_COLLECTION
};

static const uint8_t PROGMEM extra_hid_report_desc[25] = {
	0x05, 0x0c,						// USAGE_PAGE (Consumer Devices)
	0x09, 0x01,						// USAGE (Consumer Control)
	0xa1, 0x01,						// COLLECTION (Application)
	0x85, REPORT_ID_CONSUMER,		//   REPORT_ID (3)
	0x15, 0x01,						//   LOGICAL_MINIMUM (0x1)
	0x26, 0x9c, 0x02,				//   LOGICAL_MAXIMUM (0x29c)
	0x19, 0x01,						//   USAGE_MINIMUM (0x1)
	0x2a, 0x9c, 0x02,				//   USAGE_MAXIMUM (0x29c)
	0x75, 0x10,						//   REPORT_SIZE (16)
	0x95, 0x01,						//   REPORT_COUNT (1)
	0x81, 0x00,						//   INPUT (Data,Array,Abs)
	0xc0							// END_COLLECTION
};

#ifdef DEBUG_PRINT
static const uint8_t PROGMEM debug_tx_hid_report_desc[] = {
	0x06, 0x31, 0xFF,			// Usage Page 0xFF31 (vendor defined)
	0x09, 0x74,				// Usage 0x74
	0xA1, 0x53,				// Collection 0x53
	0x75, 0x08,				// report size = 8 bits
	0x15, 0x00,				// logical minimum = 0
	0x26, 0xFF, 0x00,			// logical maximum = 255
	0x95, DEBUG_TX_SIZE,			// report count
	0x09, 0x75,				// usage
	0x81, 0x02,				// Input (array)
	0xC0					// end collection
};
#endif

#define KEYBOARD_HID_DESC_NUM 0
#define KEYBOARD_HID_DESC_OFFSET (9+(9+9+7)*KEYBOARD_HID_DESC_NUM+9)

#define EXTRA_HID_DESC_NUM (KEYBOARD_HID_DESC_NUM + 1)
#define EXTRA_HID_DESC_OFFSET (9+(9+9+7)*EXTRA_HID_DESC_NUM+9)

#ifdef DEBUG_PRINT 
#define DEBUG_TX_HID_DESC_NUM (EXTRA_HID_DESC_NUM + 1)
#define DEBUG_TX_HID_DESC_OFFSET (9+(9+9+7)*DEBUG_TX_HID_DESC_NUM+9)

#define NUM_INTERFACES (DEBUG_TX_HID_DESC_NUM + 1)
#else
#define NUM_INTERFACES (EXTRA_HID_DESC_NUM+ 1)
#endif
#define CONFIG1_DESC_SIZE (9+(9+9+7)*NUM_INTERFACES)
//#define KEYBOARD_HID_DESC_OFFSET (9+9)

//configuration descriptor, USB spec 9.6.3, page 264-266, Table 9-10
static const uint8_t PROGMEM config1_descriptor[CONFIG1_DESC_SIZE] = {
	9,						//bLength
	2,						//bDescriptorType
	LSB(CONFIG1_DESC_SIZE),
	MSB(CONFIG1_DESC_SIZE),	//wTotalLength
	NUM_INTERFACES,			//bNumInterfaces
	1,						//bConfigurationValue
	0,						//iConfiguration
	0xC0,					//bmAttributes
	50,						//bMaxPower

	//interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,						// bLength
	4,						// bDescriptorType
	KEYBOARD_INTERFACE,		// bInterfaceNumber
	0,						// bAlternateSetting
	1,						// bNumEndpoints
	0x03,					// bInterfaceClass (0x03 = HID)
	0x01,					// bInterfaceSubClass (0x01 = Boot)
	0x01,					// bInterfaceProtocol (0x01 = Keyboard)
	0,						// iInterface
	
	//HID descriptor, HID 1.11 spec, section 6.2.1
	9,						//bLength
	0x21,					//bDescriptorType
	0x11, 0x01,				//bcdHID
	0,						//bCountryCode
	1,						//bNumDescriptors
	0x22,					//bDescriptorType
	sizeof(keyboard_hid_report_desc),//wDescriptorLength
	0,
	
	//endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,						//bLength
	5,						//bDescriptorType
	KEYBOARD_ENDPOINT|0x80,	//bEndpointAddress
	0x03,					//bmAttributes (0x03=intr)
	PACKET_SIZE, 0,			//wMaxPacketSize
	10,						//bInterval

	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,						//bLength
	4,						//bDescriptorType
	EXTRA_INTERFACE,		//bInterfaceNumber
	0,						//bAlternateSetting
	1,						//bNumEndpoints
	0x03,					//bInterfaceClass (0x03 = HID)
	0x00,					//bInterfaceSubClass
	0x00,					//bInterfaceProtocol
	0,						//iInterface
	
	// HID descriptor, HID 1.11 spec, section 6.2.1
	9,						//bLength
	0x21,					//bDescriptorType
	0x11, 0x01,				//bcdHID
	0,						//bCountryCode
	1,						//bNumDescriptors
	0x22,					//bDescriptorType
	sizeof(extra_hid_report_desc),//wDescriptorLength
	0,
	
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,						//bLength
	5,						//bDescriptorType
	EXTRA_ENDPOINT|0x80,	//bEndpointAddress
	0x03,					//bmAttributes (0x03=intr)
	EXTRA_SIZE, 0,			//wMaxPacketSize
	10,						//bInterval
	
	#ifdef DEBUG_PRINT 
	// interface descriptor, USB spec 9.6.5, page 267-269, Table 9-12
	9,					// bLength
	4,					// bDescriptorType
	DEBUG_TX_INTERFACE,					// bInterfaceNumber
	0,					// bAlternateSetting
	1,					// bNumEndpoints
	0x03,					// bInterfaceClass (0x03 = HID)
	0x00,					// bInterfaceSubClass
	0x00,					// bInterfaceProtocol
	0,					// iInterface
	
	// HID interface descriptor, HID 1.11 spec, section 6.2.1
	9,					// bLength
	0x21,					// bDescriptorType
	0x11, 0x01,				// bcdHID
	0,					// bCountryCode
	1,					// bNumDescriptors
	0x22,					// bDescriptorType
	sizeof(debug_tx_hid_report_desc),		// wDescriptorLength
	0,
	
	// endpoint descriptor, USB spec 9.6.6, page 269-271, Table 9-13
	7,					// bLength
	5,					// bDescriptorType
	DEBUG_TX_ENDPOINT | 0x80,		// bEndpointAddress
	0x03,					// bmAttributes (0x03=intr)
	DEBUG_TX_SIZE, 0,			// wMaxPacketSize
	1					// bInterval
	#endif
};

// If you're desperate for a little extra code memory, these strings
// can be completely removed if iManufacturer, iProduct, iSerialNumber
// in the device desciptor are changed to zeros.
static const struct usb_string_descriptor_struct PROGMEM string0 = {
	4,
	3,
	{0x0409}
};
static const struct usb_string_descriptor_struct PROGMEM string1 = {
	sizeof(STR_MANUFACTURER),
	3,
	STR_MANUFACTURER
};
static const struct usb_string_descriptor_struct PROGMEM string2 = {
	sizeof(STR_PRODUCT),
	3,
	STR_PRODUCT
};

//This table defines which descriptor data is sent for each specific
//request from the host (in wValue and wIndex).
static const struct descriptor_list_struct PROGMEM descriptor_list[] = {
	//DEVICE descriptor
	{0x0100, 0x0000, device_descriptor, sizeof(device_descriptor)},
	//CONFIGURATION descriptor
	{0x0200, 0x0000, config1_descriptor, sizeof(config1_descriptor)},
	//HID/REPORT descriptors
	{0x2100, KEYBOARD_INTERFACE, config1_descriptor+KEYBOARD_HID_DESC_OFFSET, 9},
	{0x2200, KEYBOARD_INTERFACE, keyboard_hid_report_desc, sizeof(keyboard_hid_report_desc)},
	//Extra HID Descriptor
	{0x2100, EXTRA_INTERFACE, config1_descriptor+EXTRA_HID_DESC_OFFSET, 9},
	{0x2200, EXTRA_INTERFACE, extra_hid_report_desc, sizeof(extra_hid_report_desc)},
	#ifdef DEBUG_PRINT 
	//DEBUG TX HID Descriptor
	{0x2100, DEBUG_TX_INTERFACE, config1_descriptor+DEBUG_TX_HID_DESC_OFFSET, 9},
	{0x2200, DEBUG_TX_INTERFACE, debug_tx_hid_report_desc, sizeof(debug_tx_hid_report_desc)},
	#endif
	//STRING descriptors
	{0x0300, 0x0000, (const uint8_t *)&string0, 4},
	{0x0301, 0x0409, (const uint8_t *)&string1, sizeof(STR_MANUFACTURER)},
	{0x0302, 0x0409, (const uint8_t *)&string2, sizeof(STR_PRODUCT)}
};
#define NUM_DESC_LIST (sizeof(descriptor_list)/sizeof(struct descriptor_list_struct))

/**************************************************************************
*
*  Variables - these are the only non-stack RAM usage
*
**************************************************************************/

//zero when we are not configured, non-zero when enumerated
static volatile uint8_t usb_configuration = 0;

//which modifier keys are currently pressed
//1=left ctrl,    2=left shift,   4=left alt,    8=left gui
//16=right ctrl, 32=right shift, 64=right alt, 128=right gui
uint8_t keyboard_modifier_keys;

//which keys are currently pressed, up to 6 keys may be down at once
struct keypress keyboard_keys[KEY_ROLLOVER];

//protocol setting from the host.  We use exactly the same report
//either way, so this variable only stores the setting since we
//are required to be able to report which setting is in use.
static uint8_t keyboard_protocol = 1;

//the idle configuration, how often we send the report to the
//host (ms * 4) even when it hasn't changed
static uint8_t keyboard_idle_config = 125;

//count until idle timeout
static uint8_t keyboard_idle_count = 0;

//which consumer key is currently pressed
uint16_t consumer_key;
uint16_t last_consumer_key;

//used for a couple usb registers when sending key codes
uint8_t intr_state;

#ifdef DEBUG_PRINT 
// the time remaining before we transmit any partially full
// packet, or send a zero length packet.
static volatile uint8_t debug_flush_timer=0;
#endif

/**************************************************************************
*
*  Public Functions - these are the API intended for the user
*
**************************************************************************/

//resets the keys being sent to the computer
void reset_sending(void)
{
	for(uint8_t x = 0; x < KEY_ROLLOVER; x++)
	{keyboard_keys[x] = default_press;}
	keyboard_modifier_keys = 0;
	consumer_key = 0;
	return;
}

//initialize USB
void usb_init(void)
{
	reset_sending();
	HW_CONFIG();
	USB_FREEZE();//enable USB
	PLL_CONFIG();//config PLL
	do{}while(!(PLLCSR & (1<<PLOCK)));//wait for PLL lock
	USB_CONFIG();//start USB clock
	UDCON = 0;//enable attach resistor
	usb_configuration = 0;
	UDIEN = (1<<EORSTE)|(1<<SOFE);
	sei();
	return;
}

//return 0 if the USB is not configured, or the configuration
//number selected by the HOST
uint8_t usb_configured(void)
{return usb_configuration;}

// Misc functions to wait for ready and send/receive packets
static inline void usb_wait_in_ready(void)
{while(!(UEINTX & (1<<TXINI)));}

static inline void usb_send_in(void)
{UEINTX = ~(1<<TXINI);}

static inline void usb_wait_receive_out(void)
{while (!(UEINTX & (1<<RXOUTI)));}

static inline void usb_ack_out(void)
{UEINTX = ~(1<<RXOUTI);}

void kbsend(void)
{
	usb_keyboard_send();
	usb_extra_consumer_send();
	return;
}

void kbd_press_release(uint8_t press, struct keypress key)
{
	if(key.keycode == 0){return;}
	//if(keycode == 0){keycode = KEY_0;}
	
	switch(key.keycode)
	{
		case KEY_LEFT_CTRL: (press)
			? (keyboard_modifier_keys |= (1<<0)) : (keyboard_modifier_keys &= ~(1<<0));
			return;
		case KEY_LEFT_SHIFT: (press)
			? (keyboard_modifier_keys |= (1<<1)) : (keyboard_modifier_keys &= ~(1<<1));
			return;
		case KEY_LEFT_ALT: (press)
			? (keyboard_modifier_keys |= (1<<2)) : (keyboard_modifier_keys &= ~(1<<2));
			return;
		case KEY_LEFT_GUI: (press)
			? (keyboard_modifier_keys |= (1<<3)) : (keyboard_modifier_keys &= ~(1<<3));
			return;
		case KEY_RIGHT_CTRL: (press)
			? (keyboard_modifier_keys |= (1<<4)) : (keyboard_modifier_keys &= ~(1<<4));
			return;
		case KEY_RIGHT_SHIFT: (press)
			? (keyboard_modifier_keys |= (1<<5)) : (keyboard_modifier_keys &= ~(1<<5));
			return;
		case KEY_RIGHT_ALT: (press)
			? (keyboard_modifier_keys |= (1<<6)) : (keyboard_modifier_keys &= ~(1<<6));
			return;
		case KEY_RIGHT_GUI: (press)
			? (keyboard_modifier_keys |= (1<<7)) : (keyboard_modifier_keys &= ~(1<<7));
			return;
	}

	for(uint8_t x = 0; x < KEY_ROLLOVER; x++)
	{
		if(press)
		{
			if(keyboard_keys[x].keycode == 0)
			{
				keyboard_keys[x] = key;
				return;
			}
			continue;
		}
		if(keypress_compare(keyboard_keys[x], key))
		{
			keyboard_keys[x] = default_press;
			return;
		}
	}
	return;
}

//perform a single keystroke
uint8_t usb_keyboard_press(uint8_t key, uint8_t modifier)
{
	uint8_t r;
	//backup what is really being pressed
	struct keypress t1 = keyboard_keys[0];
	uint8_t t2 = keyboard_modifier_keys;
	
	//send the key specified
	keyboard_modifier_keys = modifier;
	keyboard_keys[0] = (struct keypress){key, 0, 0};
	r = usb_keyboard_send();
	
	if(r){return r;}
	_delay_ms(50);//min amount of time for keypress
	
	//clear the output
	keyboard_keys[0].keycode = 0;
	keyboard_modifier_keys = 0;
	usb_keyboard_send();
	//set the key back to its former state
	keyboard_keys[0] = t1;
	keyboard_modifier_keys = t2;
	return 0;
}

uint8_t usb_extra_press(uint8_t key)
{
	uint8_t r;
	
	//get the real 16bit value of the key and send it
	consumer_key = consumer_lookup[key];
	r = usb_extra_consumer_send();
	if(r){return r;}
	_delay_ms(50);
	
	//clear the output
	consumer_key = 0;
	usb_extra_consumer_send();
	return 0;
}

uint8_t usb_extra_consumer_send(void)
{
	uint8_t result = 0;
	//don't resend the same key repeatedly if held, only send it once.
	if(consumer_key != last_consumer_key)
	{
		result = usb_extra_send(REPORT_ID_CONSUMER, consumer_key);
		if(!result)
		{last_consumer_key = consumer_key;}
	}
	return result;
}

uint8_t prepare_send(const uint8_t endpoint)
{
	uint8_t timeout;
	intr_state = 0;

	if(!usb_configuration){return 1;}
	intr_state = SREG;
	cli();
	UENUM = endpoint;
	timeout = UDFNUML + 50;
	
	while(1)
	{
		// are we ready to transmit?
		if (UEINTX & (1<<RWAL)) break;
		SREG = intr_state;
		// has the USB gone offline?
		if (!usb_configuration) return 1;
		// have we waited too long?
		if (UDFNUML == timeout) return 1;
		// get ready to try checking again
		intr_state = SREG;
		cli();
		UENUM = endpoint;
	}
	return 0;
}

// send the contents of keyboard_keys and keyboard_modifier_keys
uint8_t usb_keyboard_send(void)
{
	if(prepare_send(KEYBOARD_ENDPOINT))
	{return 1;}
	
	UEDATX = keyboard_modifier_keys;
	UEDATX = 0;
	
	for(uint8_t i = 0; i < KEY_ROLLOVER; i++)
	{UEDATX = keyboard_keys[i].keycode;}
	
	UEINTX = 0x3A;
	SREG = intr_state;
	keyboard_idle_count = 0;
	return 0;
}

uint8_t usb_extra_send(uint8_t report_id, uint16_t data)
{
	if(prepare_send(EXTRA_ENDPOINT))
	{return 1;}

	UEDATX = report_id;
	UEDATX = data&0xFF;
	UEDATX = (data>>8)&0xFF;

	UEINTX = 0x3A;
	SREG = intr_state;
	return 0;
}

#ifdef DEBUG_PRINT 
// transmit a character.  0 returned on success, -1 on error
int8_t usb_debug_putchar(uint8_t c)
{
	static uint8_t previous_timeout=0;
	uint8_t timeout, intr_state;

	// if we're not online (enumerated and configured), error
	if (!usb_configuration) return -1;
	// interrupts are disabled so these functions can be
	// used from the main program or interrupt context,
	// even both in the same program!
	intr_state = SREG;
	cli();
	UENUM = DEBUG_TX_ENDPOINT;
	// if we gave up due to timeout before, don't wait again
	if (previous_timeout) {
		if (!(UEINTX & (1<<RWAL))) {
			SREG = intr_state;
			return -2;
		}
		previous_timeout = 0;
	}
	// wait for the FIFO to be ready to accept data
	timeout = UDFNUML + 4;
	while (1) {
		// are we ready to transmit?
		if (UEINTX & (1<<RWAL)) break;
		SREG = intr_state;
		// have we waited too long?
		if (UDFNUML == timeout) {
			previous_timeout = 1;
			return -3;
		}
		// has the USB gone offline?
		if (!usb_configuration) return -4;
		// get ready to try checking again
		intr_state = SREG;
		cli();
		UENUM = DEBUG_TX_ENDPOINT;
	}
	// actually write the byte into the FIFO
	UEDATX = c;
	// if this completed a packet, transmit it now!
	if (!(UEINTX & (1<<RWAL))) {
		UEINTX = 0x3A;
		debug_flush_timer = 0;
	} else {
		debug_flush_timer = 2;
	}
	SREG = intr_state;
	return 0;
}


// immediately transmit any buffered output.
void usb_debug_flush_output(void)
{
	uint8_t intr_state;

	intr_state = SREG;
	cli();
	if (debug_flush_timer) {
		UENUM = DEBUG_TX_ENDPOINT;
		while ((UEINTX & (1<<RWAL))) {
			UEDATX = 0;
		}
		UEINTX = 0x3A;
		debug_flush_timer = 0;
	}
	SREG = intr_state;
}

void print(const char *s){
	char *c = (char *)s;
	while(*c){
		//if(*c++=='\n'){usb_debug_putchar('\r');}
		usb_debug_putchar(*c++);
	}
	usb_debug_putchar('\n');
}
#endif

/**************************************************************************
*
*  Private Functions - not intended for general user consumption....
*
**************************************************************************/

// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{
	uint8_t intbits, i;
	#ifdef DEBUG_PRINT 
	uint8_t t;
	#endif
	
	static uint8_t div4=0;

	intbits = UDINT;
	UDINT = 0;
	if (intbits & (1<<EORSTI))
	{
		UENUM = 0;
		UECONX = 1;
		UECFG0X = EP_TYPE_CONTROL;
		UECFG1X = EP_SIZE(ENDPOINT_SIZE) | EP_SINGLE_BUFFER;//set up endpoint to send large packets, though only single buffer
		UEIENX = (1<<RXSTPE);
		usb_configuration = 0;
	}
	if((intbits & (1<<SOFI)) && usb_configuration)
	{
		if(keyboard_idle_config && (++div4 & 3) == 0)
		{
			UENUM = KEYBOARD_ENDPOINT;
			if(UEINTX & (1<<RWAL))
			{
				keyboard_idle_count++;
				if(keyboard_idle_count == keyboard_idle_config)
				{
					keyboard_idle_count = 0;
					UEDATX = keyboard_modifier_keys;//send modifier keys to computer
					UEDATX = 0;
					for(i = 0; i < KEY_ROLLOVER; i++)
					{UEDATX = keyboard_keys[i].keycode;}//send all pressed keys
					UEINTX = 0x3A;
				}
			}
		}
		#ifdef DEBUG_PRINT 
		t = debug_flush_timer;
		if (t) {
			
			debug_flush_timer = -- t;
			if (!t) {
				UENUM = DEBUG_TX_ENDPOINT;
				while ((UEINTX & (1<<RWAL))) {
					UEDATX = 0;
				}
				UEINTX = 0x3A;
			}
		}
		#endif
	}
}

// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR(USB_COM_vect)
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
	uint8_t intbits;
	const uint8_t *list;
	const uint8_t *cfg;
	uint8_t i, n, len, en;
	uint16_t desc_val;
	const uint8_t *desc_addr;
	uint8_t	desc_length;

	UENUM = 0;
	intbits = UEINTX;
	if(intbits & (1<<RXSTPI))
	{
		bmRequestType = UEDATX;
		bRequest = UEDATX;
		wValue = UEDATX;
		wValue |= (UEDATX << 8);
		wIndex = UEDATX;
		wIndex |= (UEDATX << 8);
		wLength = UEDATX;
		wLength |= (UEDATX << 8);
		UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
		
		if(bRequest == GET_DESCRIPTOR)
		{
			list = (const uint8_t *)descriptor_list;
			for(i = 0;; i++)
			{
				if(i >= NUM_DESC_LIST)
				{
					UECONX = (1<<STALLRQ)|(1<<EPEN);//stall
					return;
				}
				desc_val = pgm_read_word(list);
				if(desc_val != wValue)
				{
					list += sizeof(struct descriptor_list_struct);
					continue;
				}
				list += 2;
				desc_val = pgm_read_word(list);
				if(desc_val != wIndex)
				{
					list += sizeof(struct descriptor_list_struct)-2;
					continue;
				}
				
				//below executes if the request the host sent matches the descriptors in memory
				list += 2;
				desc_addr = (const uint8_t *)pgm_read_word(list);//read contents of the descriptor
				list += 2;
				desc_length = pgm_read_byte(list);//read size of the descriptor
				break;
			}
			len = (wLength < 256) ? wLength : 255;
			if(len > desc_length){len = desc_length;}
			do{
				do{i = UEINTX;}
				while(!(i & ((1<<TXINI)|(1<<RXOUTI))));//wait for host ready for IN packet
				if(i & (1<<RXOUTI)){return;}//abort
				
				//send IN packet
				n = len < ENDPOINT_SIZE ? len : ENDPOINT_SIZE;
				for(i = n; i; i--)
				{UEDATX = pgm_read_byte(desc_addr++);}
				len -= n;
				usb_send_in();
			}while(len || n == ENDPOINT_SIZE);
			return;
		}
		
		if(bRequest == SET_ADDRESS)
		{
			usb_send_in();
			usb_wait_in_ready();
			UDADDR = wValue | (1<<ADDEN);//sets the address to whatever the host sent
			//UDADDR - address register
			//ADDEN - address enable bit
			return;
		}
		
		if(bRequest == SET_CONFIGURATION && bmRequestType == 0)
		{
			usb_configuration = wValue;
			usb_send_in();
			cfg = endpoint_config_table;
			for(i=1; i<5; i++)
			{
				UENUM = i;//number of endpoints
				en = pgm_read_byte(cfg++);
				UECONX = en;//endpoint number (eg. endpoint 0 is pipe)
				if(en)
				{
					UECFG0X = pgm_read_byte(cfg++);//define endpoint type
					UECFG1X = pgm_read_byte(cfg++);//define enpoint size
				}
			}
			UERST = 0x1E;//reset/clear endpoint
			UERST = 0;
			return;
		}
		
		if(bRequest == GET_CONFIGURATION && bmRequestType == 0x80)
		{
			usb_wait_in_ready();
			UEDATX = usb_configuration;
			usb_send_in();
			return;
		}

		if(bRequest == GET_STATUS)
		{
			usb_wait_in_ready();
			i = 0;
			#ifdef SUPPORT_ENDPOINT_HALT
			if(bmRequestType == 0x82)
			{
				UENUM = wIndex;
				if(UECONX & (1<<STALLRQ)){i = 1;}
				UENUM = 0;
			}
			#endif
			UEDATX = i;
			UEDATX = 0;
			usb_send_in();
			return;
		}
		
		#ifdef SUPPORT_ENDPOINT_HALT
		if((bRequest == CLEAR_FEATURE || bRequest == SET_FEATURE)
		&& bmRequestType == 0x02 && wValue == 0)
		{
			i = wIndex & 0x7F;
			if (i >= 1 && i <= MAX_ENDPOINT)
			{
				usb_send_in();
				UENUM = i;
				if (bRequest == SET_FEATURE)
				{UECONX = (1<<STALLRQ)|(1<<EPEN);}
				else
				{
					UECONX = (1<<STALLRQC)|(1<<RSTDT)|(1<<EPEN);
					UERST = (1 << i);
					UERST = 0;
				}
				return;
			}
		}
		#endif
		
		if(wIndex == KEYBOARD_INTERFACE)
		{
			if(bmRequestType == 0xA1)
			{
				if(bRequest == HID_GET_REPORT)
				{
					usb_wait_in_ready();
					UEDATX = keyboard_modifier_keys;
					UEDATX = 0;
					for (i=0; i<KEY_ROLLOVER; i++)
					{UEDATX = keyboard_keys[i].keycode;}
					usb_send_in();
					return;
				}
				if (bRequest == HID_GET_IDLE)
				{
					usb_wait_in_ready();
					UEDATX = keyboard_idle_config;
					usb_send_in();
					return;
				}
				if(bRequest == HID_GET_PROTOCOL)
				{
					usb_wait_in_ready();
					UEDATX = keyboard_protocol;
					usb_send_in();
					return;
				}
			}
			if (bmRequestType == 0x21) {
				if(bRequest == HID_SET_IDLE)
				{
					keyboard_idle_config = (wValue >> 8);
					keyboard_idle_count = 0;
					usb_send_in();
					return;
				}
				if(bRequest == HID_SET_PROTOCOL) {
					keyboard_protocol = wValue;
					usb_send_in();
					return;
				}
			}
		}
		#ifdef DEBUG_PRINT 
		if (wIndex == DEBUG_TX_INTERFACE){
			if (bRequest == HID_GET_REPORT && bmRequestType == 0xA1) {
				len = wLength;
				do {
					// wait for host ready for IN packet
					do {
						i = UEINTX;
					} while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
					if (i & (1<<RXOUTI)) return;	// abort
					// send IN packet
					n = len < ENDPOINT_SIZE ? len : ENDPOINT_SIZE;
					for (i = n; i; i--) {
						UEDATX = 0;
					}
					len -= n;
					usb_send_in();
				} while (len || n == ENDPOINT_SIZE);
				return;
			}
		}
		#endif
	}
	UECONX = (1<<STALLRQ) | (1<<EPEN);//stall
}
