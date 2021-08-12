// status codes
#define twi_unset 0xFF
#define master_start 0x08
#define master_repeat_start 0x10
#define master_arbitration_lost 0x38
#define transmitter_slave_address_acknowledged 0x18
#define transmitter_slave_address_not_acknowledged 0x20
#define transmitter_byte_acknowledged 0x28
#define transmitter_byte_not_acknowledged 0x30
#define receiver_slave_read_acknowledged 0x40
#define receiver_slave_read_not_acknowledged 0x48
#define receiver_byte_acknowledged 0x50
#define receiver_byte_not_acknowledged 0x58
#define twi_no_relevant_state_info 0xF8
#define twi_illegal_condition 0x00

// controller addresses
#define MCP23018_ADDR 0b01000000
