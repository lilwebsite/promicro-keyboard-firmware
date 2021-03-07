#define B 'B'
#define C 'C'
#define D 'D'
#define E 'E'
#define F 'F'

uint8_t set_PORTX_bit(uint8_t position, uint8_t value, const char port);
uint8_t set_DDRX_bit(uint8_t position, uint8_t value, const char port);
uint8_t read_PINX_bit(uint8_t position, const char port);
uint8_t u4_reverse(uint8_t num);
uint8_t u8_reverse(uint8_t num);
void set_PINX_output(uint8_t position, const char port);
void set_PINX_variable_output(uint8_t position, const char port, uint8_t output);
void set_PINX_input(uint8_t position, const char port, uint8_t pullup);
