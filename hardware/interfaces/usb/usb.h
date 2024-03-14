// this document describes USB features that should be universal for all devices.
// if a chip is to be added as a compatible part with this firmware, it is required
// to implement these properties if this USB driver is to be used.

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/atomic.h>
#include <kbd/globals.h>
#include <interfaces/usb/types.h>
#include <interfaces/usb/globals.h>

// save some memory by re-using some sizes
#define DD_LENGTH 0x12
#define DC_LENGTH 0x09
#define ID_LENGTH DC_LENGTH
#define ED_LENGTH 0x07
#define HID_LENGTH DC_LENGTH

// some helper functions
#define STALL() UECONX = (1 << STALLRQ) | (1 << EPEN) // AKA request error
#define HANDSHAKE(flag) UEINTX = (uint8_t)(~(1 << flag)) // complete a transaction
// FIFOCON does not need to be manually cleared but it ensures the handshake is sent properly
#define CLEAR_FIFOCON() while(UEINTX & (1 << RWAL)); HANDSHAKE(FIFOCON)
#define WAIT(flag) while (!(UEINTX & (1 << flag))) // wait for packet from host
#define ISSET(flag) UEINTX & (1 << flag)
#define SETEP(ep) UENUM = ep ; while (!UEINTX & (1 << RWAL)) // set endpoint and wait for read/write flag
// sends a 16 bit value
#define SEND16(data) \
	UEDATX = (uint8_t)(data & 0xFF); \
	UEDATX = (uint8_t)((data >> 8) & 0xFF);

// USB standard device requests
//
// bmRequestType
#define DEVICE_REQUEST 0x00
// bRequest values
#define GET_STATUS 0x00
#define CLEAR_FEATURE 0x01
#define SET_FEATURE 0x03
#define SET_ADDRESS 0x05
#define GET_DESCRIPTOR 0x06
#define SET_DESCRIPTOR 0x07
#define GET_CONFIGURATION 0x08
#define SET_CONFIGURATION 0x09

// USB standard interface requests
//
// bmRequestType
#define INTERFACE_REQUEST 0x01
// bRequest
#define GET_INTERFACE 0x0A
#define SET_INTERFACE 0x11

// USB standard endpoint requests
//
// bmRequestType
#define ENDPOINT_REQUEST 0x02
// bRequest
#define SYNCH_FRAME 0x12

// USB class specific requests
//
// bmRequestType
#define CLASS_REQUEST 0x21
// bRequest
#define GET_REPORT 0x01
#define GET_IDLE 0x02
#define GET_PROTOCOL 0x03
#define SET_REPORT 0x09
#define SET_IDLE 0x0A
#define SET_PROTOCOL 0x0B

// Descriptor types
#define TYPE_DEVICE 0x01
#define TYPE_CONFIGURATION 0x02
#define TYPE_STRING 0x03
#define TYPE_INTERFACE 0x04
#define TYPE_ENDPOINT 0x05
#define TYPE_DEVICE_QUALIFIER 0x06
#define TYPE_OTHER_SPEED_CONFIGURATION 0x07
#define TYPE_INTERFACE_POWER 0x08
#define TYPE_HID 0x21
#define TYPE_REPORT 0x22
#define TYPE_PHYSICAL 0x23

// we list descriptors in a separate file as they take up a lot of space
#include "descriptors.h"

// we need to define some variables to keep track of USB state
static uint8_t configuration_ok = 1; // if equal to 0 CFGOK failed
static uint8_t usb_configuration = 0; // used by SET_CONFIGURATION and GET_CONFIGURATION
static uint8_t sent_reports = 0; // before sending data check we sent all report descriptors
static uint8_t delay_startup = 0;
static uint8_t device_protocol = 1; // used by SET_PROTOCOL and GET_PROTOCOL
static uint8_t endpoints_ready = 0; // is set to 1 before any endpoint usage (except EP0)
static uint16_t wTotalLength = 0; // used by the configuration descriptor

// Now some variables must be defined to give persistent storage for keys
// and buttons being currently pressed. Is not set static as it must be 
// externally accessed.
uint8_t keyboard_modifier_keys = 0;
struct keypress keyboard_keys[KEY_ROLLOVER];

// struct for general usb state (such as keyboard LEDs)
static uint8_t led_state = 0;
struct usb_state usb = {0, 0, 0};

//struct consumer_state {
//	uint16_t current;
//	uint16_t previous;
//} consumer_key = {0, 0};
struct consumer_state consumer_key = {0, 0};

//struct mouse_state {
//	uint8_t buttons; // only uses first 3 LSB
//	uint8_t x_axis;
//	uint8_t y_axis;
//} mouse = {0, 0, 0};
struct mouse_state mouse = {0, 0, 0};

// buffer for vendor endpoint
//struct vendor_state {
//	// `state` is a bitmap
//	// 0b1 == ready to send
//	// 0b10 == ready to receive
//	// 0b100 == data is stale (already sent or received)
//	uint8_t state;
//	uint8_t data[8];
//} vendor_buffer = {0, {0, 0, 0, 0, 0, 0, 0, 0}};
//struct vendor_state vendor_buffer = {0, {0, 0, 0, 0, 0, 0, 0, 0}};

const struct hid_descriptor keyboard_hid[KBD_ENDPOINTS] = {
	{ USB_ED_KBD, USB_HID_KBD, USB_HIDR_KBD, 0x2 }
};

const struct hid_descriptor application_hid[APP_ENDPOINTS] = {
	{ USB_ED_APP, USB_HID_APP, USB_HIDR_APP, 0 }
};

const struct hid_descriptor mouse_hid[MSE_ENDPOINTS] = {
	{ USB_ED_MSE, USB_HID_MSE, USB_HIDR_MSE, 0 }
};

// this may have to be 2 endpoints, not sure yet 
//const struct hid_descriptor vendor_hid[VEN_ENDPOINTS] = {
//	{ USB_ED_VEN_IN, USB_HID_VEN, USB_HIDR_VEN_IN, 0 },
//	{ USB_ED_VEN_OUT, USB_HID_VEN, USB_HIDR_VEN_OUT, 0 }
//};

const struct reply_mapping reply_endpoints[REPLY_ENDPOINTS] = {
	{ USB_ED_VEN_OUT, USB_ED_VEN_IN }
};

const struct hid_descriptor vendor_hid[VEN_ENDPOINTS] = {
	{ USB_ED_VEN_IN, 0, 0, 0 },
	{ USB_ED_VEN_OUT, 0, 0, 0 }
};

const struct usb_interface usb_interfaces[INTERFACE_COUNT] = {
	{ keyboard_hid, KBD_ENDPOINTS },
	{ application_hid, APP_ENDPOINTS},
	{ mouse_hid, MSE_ENDPOINTS },
	{ vendor_hid, VEN_ENDPOINTS }
};

//const char LANGID[2] = { 0x09, 0x04 };
#define LANGID 0x0409
const int16_t string1[] PROGMEM = L"rendware";
const int16_t string2[] PROGMEM = L"v1";
struct string_descriptor vendor_strings[STRING_COUNT] = {
	{ string1, sizeof(string1) },
	{ string2, sizeof(string2) }
};

// the length of the descriptor array is (bLength - 2) in size, which 
// is since I ommit the bLength and bDescriptorType values from the
// array as they can be determined through other means. It saves 2
// bytes per descriptor, except for report descriptors.
struct usb_description descriptors = {
	USB_DD,
	USB_DC,
	usb_interfaces,
	INTERFACE_COUNT
};

// for additional functionality on device
struct action_state action_ep_state = {0, 0, 0, 0, 0, 0};

// function definitions
void usb_init(void);
const struct hid_descriptor *get_hid_descriptor(uint8_t endpoint);
void get_descriptor(struct setup_packet *data0);
void send_descriptor(struct setup_packet *data0, struct descriptor_stream *stream);
void usb_wait_frames(uint16_t frames);
void send_report(uint8_t interface);
void get_status(void);
void usb_setup(struct setup_packet *data0);
uint8_t usb_configured(void);
uint8_t activate_endpoints(void);
const uint8_t *out_ep_handler(uint8_t *data, uint16_t length);
void out_ep_reply(const uint8_t *endpoint);

// keyboard specific
void kbd_press_release(uint8_t press, struct keypress key);
void kbd_consumer_press(uint16_t keycode);
void kbd_single_press(uint8_t keycode, uint8_t modifier);
void reset_sending(void);

extern uint8_t device_reset;
extern struct debugging usb_debug;
extern void usb_debug_write(uint8_t data);
extern void usb_debug_flush(void);

uint8_t usb_keyboard_send(void);
uint8_t prepare_send(const uint8_t endpoint);
uint8_t intr_state = 0;
