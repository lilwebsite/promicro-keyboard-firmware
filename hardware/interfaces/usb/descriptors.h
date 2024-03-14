// USB Device Descriptor
// This describes the device with some basic settings. The endpoints
// are not defined just yet but this helps give the host an idea of
// what our device will capable of.
const uint8_t USB_DD[DD_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x12, // (18) length of this array, afaik it must be 18
//	// bDescriptorType
//	0x01, // (1) designates this descriptor as type `device`
	// bcdUSB
	0x00, // (2) refers to the USB version number, in this case, 2.0
	0x02, // since we are casting uint8_t to uint16_t, MSB comes last
	// bDeviceClass
	0x00, // (2) refers to the device class, in this case, HID (Human Interface Device)
	// bDeviceSubClass
	0x00, // subclass omitted
	// bDeviceProtocol
	0x00, // protocol omitted
	// bMaxPacketSize0
	0x08, // (8) sets the maximum packet size for endpoint zero.
	// idVendor
	0xFE, // sets the device "vendor". Pay USB-IF $6000 or choose undefined. Guess which I did
	0xFE,
	// idProduct
	0xFE,
	0xFE,
	// bcdDevice
	0x00, // sets the device version. Arbitrary number, I set 1 for version 1.0
	0x01,
	// iManufacturer
	0x01, // manufacturer string index
	// iProduct
	0x02, // product string index
	// iSerialNumber
	0x02, // serial number string index
	// bNumConfigurations
	0x01 // number of available device configurations, we use 1
};

// USB Device Configuration
// This configures some additional settings, mainly what interfaces our
// device provides.
const uint8_t USB_DC[DC_LENGTH - 5] PROGMEM = {
//	// bLength
//	0x09,
//	// bDescriptorType
//	0x02, // of type `configuration`
//	// wTotalLength
//	0x00, // total length of all descriptors returned by this config. That includes all endpoint/HID descriptors as well.
//	0x00,
//	// bNumInterfaces
//	0x01, // number of interfaces this configuration provides
	// bConfigurationValue
	0x01, // the value provided to SetConfiguration() request which selects this config
	// iConfiguration
	0x01, // the index of the string which describes this config
	// bmAttributes
	0x80, // bit 7 (0x40) allows us to specify how much power to draw from the bus. I am not sure if this has to be set.
	// bMaxPower
	0xFA  // this is related to the previous option. I set 500mA here. 0xFA is 250 in decimal
};

// USB Interface Descriptor
// This describes our interface, which will contain all of our endpoints
// It is not actually defined but resolved in realtime based on the structs
// that represent it. Saves some program memory that way.
//const uint8_t USB_ID[ID_LENGTH - 4] = {
//	// bLength
//	0x09,
//	// bDescriptorType
//	0x04, // of type `interface`
//	// bInterfaceNumber
//	0x00, // number of this interface, this being the first (0) in the index
//	// bAlternateSetting
//	0x00, // we don't implement GetInterface(), so this will remain 0
//	// bNumEndpoints
//	0x01, // number of endpoints, excluding endpoint 0
//	// bInterfaceClass
//	0x03, // USB-IF defined class code. 0x03 is HID
//	// bInterfaceSubClass
//	0x00, // not used
//	// bInterfaceProtocol
//	0x00, // not used
//	// iInterface
//	0x01  // the string describing this interface
//};

// USB Endpoint Descriptor
// The endpoint determines bandwidth requirements for data being sent
// (or received). A HID descriptor will be defined later which will operate
// on these constraints as well.
const uint8_t USB_ED_KBD[ED_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x07,
//	// bDescriptorType
//	0x05, // of type `endpoint`
	// bEndpointAddress
	// `IN` in this context means keyboard is sending data IN to the computer
	0x81, // direction IN (0x80), endpoint 1 (0x01)
	// bmAttributes
	0x03, // interrupt mode. More or less: only send data on keystrokes
	// wMaxPacketSize
	//0x20, // maximum amount of bytes (keystrokes) to send (1 byte = 1 packet)
	KBD_EP_SIZE,
	0x00,
	// bInterval
	0x01 // this is a 1ms interval, since this a full-speed device
};

// USB HID Descriptor
// This is a special descriptor that is loaded as part of the HID specification
// we only define what our subsequent related descriptors will be.
const uint8_t USB_HID_KBD[HID_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x09,
//	// bDescriptorType
//	0x21, // of type `HID`
	// bcdHID
	0x11, // HID specification version to follow, equates 1.11
	0x01,
	// bCountryCode
	0x00, // this is not really relevant so just set 0 (unknown)
	// bNumDescriptors
	0x01, // this is the number of class descriptors we are specifying
	// bDescriptorClass
	0x22, // descriptor type `Report`
	// wDescriptorLength
	0x40, // length of the report descriptor (64 bytes)
	0x00
};

// USB HID Report Descriptor, Generic Keyboard
// This part is probably the most important part of the entire driver.
// It alows us to specify key features that can completely unlock the
// capabilities of the keyboard to do much more than your average
// HID keyboard device. The following report will specify generic
// keyboard functionality with an extended codepage. I generated these
// values by using the USBIF HID Descriptor Tool by modifying the
// keyboard report descriptor template. If you need a more detailed
// explanation on specific values, check HID 1.11 spec. There is a
// discrepancy with usage page `Keyboard` for USAGE_MAXIMUM. The HID
// Descriptor Tool does not recognize keycodes above `Keyboard ExSel`
// which I believe to be a limitation to the USB version the tool was
// made for.
const uint8_t USB_HIDR_KBD[0x40] PROGMEM = {
	0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
	0x09, 0x06,        // USAGE (Keyboard)
	0xA1, 0x01,        // COLLECTION (Application)
	0x05, 0x07,           // USAGE_PAGE (Keyboard)
	0x19, 0xE0,           // USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xE7,           // USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,           // LOGICAL_MINIMUM (0)
	0x25, 0x01,           // LOGICAL_MAXIMUM (1)
	0x95, 0x08,           // REPORT_COUNT (8)
	0x75, 0x01,           // REPORT_SIZE (1)
	0x81, 0x02,           // INPUT (Data,Var,Abs)
	0x95, 0x01,           // REPORT_COUNT (1)
	0x75, 0x08,           // REPORT_SIZE (8)
	0x81, 0x03,           // INPUT (Cnst,Var,Abs)
	0x95, 0x05,           // REPORT_COUNT (5)
	0x75, 0x01,           // REPORT_SIZE (1)
	0x05, 0x08,           // USAGE_PAGE (LEDs)
	0x19, 0x01,           // USAGE_MINIMUM (Num Lock)
	0x29, 0x05,           // USAGE_MAXIMUM (Kana)
	0x91, 0x02,           // OUTPUT (Data,Var,Abs)
	0x95, 0x01,           // REPORT_COUNT (1)
	0x75, 0x03,           // REPORT_SIZE (3)
	0x91, 0x03,           // OUTPUT (Cnst,Var,Abs)
	0x95, 0x1E,           // REPORT_COUNT (30)
	0x75, 0x08,           // REPORT_SIZE (8)
	0x15, 0x00,           // LOGICAL_MINIMUM (0)
	0x26, 0xFF, 0x00,     // LOGICAL_MAXIMUM (255)
	0x05, 0x07,           // USAGE_PAGE (Keyboard)
	0x19, 0x00,           // USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0xDD,           // USAGE_MAXIMUM (Keypad Hexadecimal)
	0x81, 0x00,           // INPUT (Data,Ary,Abs)
	0xC0               // END_COLLECTION
};

const uint8_t USB_ED_APP[ED_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x07,
//	// bDescriptorType
//	0x05,
	// bEndpointAddress
	0x82,
	// bmAttributes
	0x03,
	// wMaxPacketSize
	0x02,
	0x00,
	// bInterval
	0x01
};

const uint8_t USB_HID_APP[HID_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x09,
//	// bDescriptorType
//	0x21,
	// bcdHID
	0x11,
	0x01,
	// bCountryCode
	0x00,
	// bNumDescriptors
	0x01,
	// bDescriptorClass
	0x22,
	// wDescriptorLength
	0x19,
	0x00
};

// USB HID Report Descriptor, Consumer Control
// This functions much like the above descriptor, however adds
// consumer functions such as volume up/down, play/pause, etc.
// which requires it's own Usage Page as it is not part of 
// Generic Desktop. As we see in the above report descriptor,
// INPUT of type Array does not need an associated Constant.
const uint8_t USB_HIDR_APP[0x19] PROGMEM = {
	0x05, 0x0C,                // USAGE_PAGE (Consumer Devices)
	0x09, 0x01,                // USAGE (Consumer Control)
	0xA1, 0x01,                // COLLECTION (Application)
	0x19, 0x00,                   // USAGE_MINIMUM (Unassigned)
	0x2A, 0x14, 0x05,             // USAGE_MAXIMUM (Unassigned)
	0x15, 0x00,                   // LOGICAL_MINIMUM (0)
	0x27, 0xFF, 0xFF, 0x00, 0x00, // LOGICAL_MAXIMUM (65535)
	0x75, 0x10,                   // REPORT_SIZE (16)
	0x95, 0x01,                   // REPORT_COUNT (1)
	0x81, 0x00,                   // INPUT (Data,Ary,Abs)
	0xC0                       // END_COLLECTION
};

const uint8_t USB_ED_MSE[ED_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x07,
//	// bDescriptorType
//	0x05,
	// bEndpointAddress
	0x83,
	// bmAttributes
	0x03,
	// wMaxPacketSize
	0x04,
	0x00,
	// bInterval
	0x01
};

const uint8_t USB_HID_MSE[HID_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x09,
//	// bDescriptorType
//	0x21,
	// bcdHID
	0x11,
	0x01,
	// bCountryCode
	0x00,
	// bNumDescriptors
	0x01,
	// bDescriptorClass
	0x22,
	// wDescriptorLength
	0x32,
	0x00
};

// USB HID Report Descriptor, Generic Mouse
// More or less functions like the keyboard descriptor, except
// the controls are for mice. Some keyboards have trackpads,
// trackballs, or even trackpoints built-in. This fulfills that
// functionality without using a separate micro with a separate
// USB driver. This is straight copied from the HID Descriptor
// Tool.
const uint8_t USB_HIDR_MSE[0x32] PROGMEM = {
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x02, // USAGE (Mouse)
	0xA1, 0x01, // COLLECTION (Application)
	0x09, 0x01,    // USAGE (Pointer)
	0xA1, 0x00,    // COLLECTION (Physical)
	0x05, 0x09,       // USAGE_PAGE (Button)
	0x19, 0x01,       // USAGE_MINIMUM (Button 1)
	0x29, 0x03,       // USAGE_MAXIMUM (Button 3)
	0x15, 0x00,       // LOGICAL_MINIMUM (0)
	0x25, 0x01,       // LOGICAL_MAXIMUM (1)
	0x95, 0x03,       // REPORT_COUNT (3)
	0x75, 0x01,       // REPORT_SIZE (1)
	0x81, 0x02,       // INPUT (Data,Var,Abs)
	0x95, 0x01,       // REPORT_COUNT (1)
	0x75, 0x05,       // REPORT_SIZE (5)
	0x81, 0x03,       // INPUT (Cnst,Var,Abs)
	0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
	0x09, 0x30,       // USAGE (X)
	0x09, 0x31,       // USAGE (Y)
	0x15, 0x81,       // LOGICAL_MINIMUM (-127)
	0x25, 0x7F,       // LOGICAL_MAXIMUM (127)
	0x75, 0x08,       // REPORT_SIZE (8)
	0x95, 0x02,       // REPORT_COUNT (2)
	0x81, 0x06,       // INPUT (Data,Var,Rel)
	0xC0,          // END_COLLECTION
	0xC0        // END_COLLECTION
};

const uint8_t USB_ED_VEN_IN[ED_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x07,
//	// bDescriptorType
//	0x05,
	// bEndpointAddress
	0x84,
	// bmAttributes
	0x03,
	// wMaxPacketSize
	0x08,
	0x00,
	// bInterval
	0x01
};

const uint8_t USB_ED_VEN_OUT[ED_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x07,
//	// bDescriptorType
//	0x05,
	// bEndpointAddress
	0x05,
	// bmAttributes
	0x03,
	// wMaxPacketSize
	0x08,
	0x00,
	// bInterval
	0x01
};

const uint8_t USB_HID_VEN[HID_LENGTH - 2] PROGMEM = {
//	// bLength
//	0x09,
//	// bDescriptorType
//	0x21,
	// bcdHID
	0x11,
	0x01,
	// bCountryCode
	0x00,
	// bNumDescriptors
	0x01,
	// bDescriptorClass
	0x22,
	// wDescriptorLength
	0x13,
	0x00
};

// USB HID Report Descriptor, Vendor Defined
// This one is a bit more unique but quite simple really. It is
// a bi-directional data pipeline which the HID driver on the
// host system can communicate with. I was thinking of the
// possibility this can be achieved by other means (without HID
// report descriptors) but this seems simple and I don't have to
// write a separate driver for it. INPUT to the host is read only
// (AKA Constant) and OUTPUT from the host to the device is writable
// by the host.
const uint8_t USB_HIDR_VEN_IN[0x13] PROGMEM = {
	0x06, 0x00, 0xFF, // USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,       // USAGE (Vendor Usage 1)
	0xA1, 0x01,          // COLLECTION (Application)
	0x15, 0x00,          // LOGICAL_MINIMUM (0)
	0x26, 0xFF, 0x00,    // LOGICAL_MAXIMUM (255)
	0x75, 0x08,          // REPORT_SIZE (8)
	0x95, 0x08,          // REPORT_COUNT (8)
	0x81, 0x01,          // INPUT (Cnst,Ary,Abs)
	0xC0              // END_COLLECTION
};

const uint8_t USB_HIDR_VEN_OUT[0x13] PROGMEM = {
	0x06, 0x00, 0xFF, // USAGE_PAGE (Vendor Defined Page 1)
	0x09, 0x01,       // USAGE (Vendor Usage 1)
	0xA1, 0x01,          // COLLECTION (Application)
	0x15, 0x00,          // LOGICAL_MINIMUM (0)
	0x26, 0xFF, 0x00,    // LOGICAL_MAXIMUM (255)
	0x75, 0x08,          // REPORT_SIZE (8)
	0x95, 0x08,          // REPORT_COUNT (8)
	0x91, 0x00,          // OUTPUT (Data,Ary,Abs)
	0xC0              // END_COLLECTION
};
