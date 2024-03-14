uint8_t set_port(const char port, uint8_t position, uint8_t value);
uint8_t set_ddr(const char port, uint8_t position, uint8_t value);
uint8_t read_pin(const char port, uint8_t position);
void pin_value(const char port, uint8_t position, uint8_t value);
void pin_output(const char port, uint8_t position);
void pin_input(const char port, uint8_t position, uint8_t pullup);
