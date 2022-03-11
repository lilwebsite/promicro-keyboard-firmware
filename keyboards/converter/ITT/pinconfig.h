// old converter pin out, here for historical purpose
// const struct pin ITT_C1 = {F, 7};
// const struct pin ITT_DATA = {F, 4};
// const struct pin ITT_EN = {F, 5};
// const struct pin ITT_SOLENOID = {F, 6};
// const struct pin ITT_C2 = {B, 1};

const struct pin ITT_C1 = {F, 4};
const struct pin ITT_EN = {F, 5};
const struct pin ITT_SOLENOID = {F, 6};
const struct pin ITT_DATA = {F, 7};
const struct pin ITT_C2 = {B, 1};

uint8_t init_pins(void)
{
	#if ITT_NUMPAD==1
	set_PINX_variable_output(ITT_EN.position, ITT_EN.port, 0); // this pin needs to be LOW for the numpad to work, not sure what it's other functions are
	set_PINX_variable_output(ITT_SOLENOID.position, ITT_SOLENOID.port, 1); // solenoid trigger, unused for numpad
	#else
	set_PINX_variable_output(ITT_EN.position, ITT_EN.port, 1); // this pin needs to be HIGH for the keyboard to work, not sure what it's other functions are
	set_PINX_variable_output(ITT_SOLENOID.position, ITT_SOLENOID.port, 0); // solenoid trigger
	#endif

	set_PINX_input(ITT_DATA.position, ITT_DATA.port, 0); // data signal
	set_PINX_variable_output(ITT_C2.position, ITT_C2.port, 0); // secondary clock, keep low

	return 1;
}
