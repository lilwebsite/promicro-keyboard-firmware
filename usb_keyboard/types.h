struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	int16_t wString[];
};

struct descriptor_list_struct {
	uint16_t wValue;
	uint16_t wIndex;
	const uint8_t *addr;
	uint8_t length;
};

struct keypress {
	uint8_t keycode;
	uint8_t row;
	uint8_t column;
};

const inline uint8_t keypress_compare(struct keypress kp1, struct keypress kp2)
{return (kp1.row == kp2.row) && (kp1.column == kp2.column);}