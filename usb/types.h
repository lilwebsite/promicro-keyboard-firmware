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
