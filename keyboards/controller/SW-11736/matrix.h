// This is a macro which generates an array with the correct logical order from a human readable order.

#define NC 0
#define KEYMAP_POS(\
	k001, k002, k003, k004, k005, k006, k007, k008, k009, k010, k011, k012, k013, \
	k014, k015, k016, k017, k018, k019, k020, k021, k022, k023, k024, k025, k026, \
	k027, k028, k029, k030, k031, k032, k033, k034, k035, k036, k037, k038, k039, k040, \
	k041, k042, k043, k044, k045, k046, k047, k048, k049, k050, k051, k052, \
	k053 \
)\
{\
	k015, k016, k017, k001, k002, k003, k004, k005, /* first chip, first register */ \
	k007, k008, k006, k009, k010, k011, k012, k013, /* first chip, second register */ \
	k029, k028, k031, k018, k019, k020, k021, k022, /* second chip, first register */ \
	k024, k025, k023, k026, k030, k032, k033, k034, /* second chip, second register */ \
	k044, k045, k046, k047, k048, k049, k050, k051, /* third chip, first register */ \
	k035, k036, k037, k038, k039, k040, k042, k043, /* third chip, second register */ \
	k014, k027, k041, k053, k052                    /* direct connections */ \
} 
