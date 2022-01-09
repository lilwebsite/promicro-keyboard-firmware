#ifndef usb_serial_h__
#define usb_serial_h__
#include <util/delay.h>
#include <kbd/globals.h>
#include "types.h"

//#define DEBUG_PRINT

#define STR_MANUFACTURER	L"PROMICRO"
#define STR_PRODUCT			L"PROMICRO KEYBOARD"

void reset_sending(void);
void usb_init(void);//initialize everything
uint8_t usb_configured(void);//is the USB port configured

void kbsend(void);

void kbd_press_release(uint8_t press, struct keypress keycode);
uint8_t usb_keyboard_press(uint8_t key, uint8_t modifier);
uint8_t usb_extra_press(uint8_t key);

uint8_t prepare_send(const uint8_t endpoint);
uint8_t usb_keyboard_send(void);
uint8_t usb_extra_consumer_send(void);
uint8_t usb_extra_send(uint8_t report_id, uint16_t data);

#ifdef DEBUG_PRINT
void print(const char *s);
int8_t usb_debug_putchar(uint8_t c);	// transmit a character
void usb_debug_flush_output(void);	// immediately transmit any buffered output
#endif

extern uint8_t keyboard_modifier_keys;
extern struct keypress keyboard_keys[KEY_ROLLOVER];
extern uint16_t consumer_key;
#endif

//defintions for keycodes
#include "keys.h"

//everything below this point is only used by usb.c
#ifdef USB_PRIVATE_INCLUDE
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#define EP_TYPE_CONTROL				0x00
#define EP_TYPE_BULK_IN				0x81
#define EP_TYPE_BULK_OUT			0x80
#define EP_TYPE_INTERRUPT_IN		0xC1
#define EP_TYPE_INTERRUPT_OUT		0xC0
#define EP_TYPE_ISOCHRONOUS_IN		0x41
#define EP_TYPE_ISOCHRONOUS_OUT		0x40

#define EP_SINGLE_BUFFER			0x02
#define EP_DOUBLE_BUFFER			0x06

#define EP_SIZE(s)	((s) == 64 ? 0x30 : \
					((s) == 32 ? 0x20 : \
					((s) == 16 ? 0x10 :	0x00)))

#define MAX_ENDPOINT 4

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

#define HW_CONFIG() (UHWCON = 0x01)
#define PLL_CONFIG() (PLLCSR = 0x12)
#define USB_CONFIG() (USBCON = ((1<<USBE)|(1<<OTGPADE)))
#define USB_FREEZE() (USBCON = ((1<<USBE)|(1<<FRZCLK)))

// standard control endpoint request types
#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR		6
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11

// HID (human interface device)
#define HID_GET_REPORT		1
#define HID_GET_IDLE		2
#define HID_GET_PROTOCOL	3
#define HID_SET_REPORT		9
#define HID_SET_IDLE		10
#define HID_SET_PROTOCOL	11

// CDC (communication class device)
#define CDC_SET_LINE_CODING			0x20
#define CDC_GET_LINE_CODING			0x21
#define CDC_SET_CONTROL_LINE_STATE	0x22

//lookup table for the 8bit consumer keys
const uint16_t consumer_lookup[7] = {
	0x00CD,//PLAY_PAUSE	
	0x00B7,//STOP		
	0x00B6,//PREV_TRACK	
	0x00B5,//NEXT_TRACK	
	0x00E2,//MUTE		
	0x00E9,//VOL_UP		
	0x00EA //VOL_DOWN	
};

struct keypress default_press = {0, 0xFF, 0xFF};

#endif
