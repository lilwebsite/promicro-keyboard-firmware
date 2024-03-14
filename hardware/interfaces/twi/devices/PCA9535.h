#ifndef __PCA9535
#define __PCA9535

//#define TWI_BASE_ADDR 0b01000000
#define TWI_BASE_ADDR 0x40

#define IN_PORT_0  0x00
#define IN_PORT_1  0x01
#define OUT_PORT_0 0x02
#define OUT_PORT_1 0x03
#define POL_INV_0  0x04
#define POL_INV_1  0x05
#define CFG_PORT_0 0x06
#define CFG_PORT_1 0x07

extern void twi_start_condition(void);
extern uint8_t twi_begin_transaction(void);
extern uint8_t twi_send_slave(uint8_t slave);
extern uint8_t twi_send_command(uint8_t command);
extern uint8_t twi_check_status(uint8_t expected);
extern uint8_t twi_read(uint8_t acknowledge);
extern void twi_stop_condition(void); 

extern void usb_debug_write(uint8_t data);
extern void usb_debug_flush(void);

uint16_t PCA9535_READ_PINS(uint8_t slave)
{
	uint16_t bytes = 0;

	// start condition
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != master_start)
	{return 0;}

	// send slave address, write mode
	TWDR = (TWI_BASE_ADDR) | ((slave << 1) & 0xE);
	TWCR = 1 << TWINT | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != transmitter_slave_address_acknowledged)
	{
		TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 0 << TWEN;
		return 0;
	}

	// send command byte
	TWDR = 0;
	TWCR = 1 << TWINT | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != transmitter_byte_acknowledged)
	{return 0;}

	// send repeat start
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != master_repeat_start)
	{return 0;}

	// send slave address, read mode
	TWDR = TWI_BASE_ADDR | slave << 1 | 1;
	TWCR = 1 << TWINT | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != receiver_slave_read_acknowledged)
	{return 0;}

	// read data from slave, first bank
	TWCR = 1 << TWINT | 1 << TWEA | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));
	bytes = TWDR;

	if ((TWSR & 0xF8) != receiver_byte_acknowledged)
	{return 0;}

	// read data from slave, second bank
	TWCR = 1 << TWINT | 0 << TWEA | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));
	bytes |= TWDR << 8;

	if ((TWSR & 0xF8) != receiver_byte_not_acknowledged)
	{return 0;}

	// send stop condition
	TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 0 << TWEN;

	return bytes;
}

#endif
