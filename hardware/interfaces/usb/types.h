#ifndef __USB_TYPES
#define __USB_TYPES
// simple struct to store the setup packet
struct setup_packet {
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint8_t wValue[2];
	uint8_t wIndex[2];
	uint8_t wLength[2];
};

struct string_descriptor {
	const int16_t *string;
	uint8_t length;
};

// hid descriptors require an associated report descriptor and endpoint.
// we do not need to include descriptor lengths as they are either static
// or stored in the descriptor themselves when arbitrary. Report length is
// stored in the hid descriptor.
struct hid_descriptor {
	const uint8_t *endpoint;
	const uint8_t *hid;
	const uint8_t *report;
	uint8_t ep_size; // based on UECFG1X EPSIZE2:0 settings
};

// this is for OUT endpoints that need a way to reply with IN packets
struct reply_mapping {
	const uint8_t *out_ep;
	const uint8_t *reply_ep;
};

// define an interface with one more hid endpoints
// hid_count == bNumEndpoints
struct usb_interface {
	const struct hid_descriptor *hid;
	uint8_t hid_count;
};

// This struct contains all descriptors for the device.
// The interface descriptor works as a "template" and the
// bInterfaceNumber and bNumEndpoints are resolved through
// the interface_count and hid_count values.
// interface_count == bInterfaceNumber
struct usb_description {
	const uint8_t *device;
	const uint8_t *configuration;
	const struct usb_interface *interface;
	uint8_t interface_count;
};

struct usb_state {
	uint8_t numlock;
	uint8_t capslock;
	uint8_t scrolllock;
};

struct consumer_state {
	uint16_t current;
	uint16_t previous;
};

struct mouse_state {
	uint8_t buttons; // only uses first 3 LSB
	uint8_t x_axis;
	uint8_t y_axis;
};

// buffer for vendor endpoint
//struct vendor_state {
//	// `state` is a bitmap
//	// 0b1 == ready to send
//	// 0b10 == ready to receive
//	// 0b100 == data is stale (already sent or received)
//	uint8_t state;
//	uint8_t data[8];
//};

// vendor endpoint state
struct action_state {
	uint8_t mode;
	uint8_t action;
	uint16_t length;
	uint16_t buffer_pos;
	uint8_t *buffer;
	const uint8_t *reply_ep;
};

struct debugging {
	uint8_t length;
	uint8_t buffer[8];
};

struct descriptor_stream {
	const uint8_t *descriptor;
	uint8_t type;
	uint16_t length;
	uint16_t index;
	uint8_t offset;
	uint8_t buffer_index;
};
#endif
