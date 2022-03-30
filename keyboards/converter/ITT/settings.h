// set ITT_NUMPAD to 1 if numpad is being converted. May possibly automatically detect this in the future
#define ITT_NUMPAD 0

// this is the switch to disable solenoid, references the position in the layout array 
#define SOLENOID_DISABLE 51

// set NO_SOLENOID to 1 if it should be disabled completely
#undef NO_SOLENOID
#define NO_SOLENOID 1

#define COLUMNS 130
#define KEYS 114

#if ITT_NUMPAD==1
#undef NO_SOLENOID
#define NO_SOLENOID 1
#endif

#define UNDO_BUTTON
#define REDO_BUTTON
#define COPY_BUTTON
#define CUT_BUTTON
#define PASTE_BUTTON
#define ENABLE_TRACK_KEYS
#define ENABLE_PLAY_PAUSE
#define STOP_BUTTON
