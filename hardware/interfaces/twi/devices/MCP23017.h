#ifndef __MCP23018
#define __MCP23018

#define IOCON 0
#define TWI_BASE_ADDR 0b01000000
#define TARGET_SLAVE TWI_BASE_ADDR

#if IOCON == 0
	#define   IODIRA 0x0
	#define   IODIRB 0x1
	#define    IPOLA 0x2
	#define    IPOLB 0x3
	#define GPINTENA 0x4
	#define GPINTENB 0x5
	#define  DEFVALA 0x6
	#define  DEFVALB 0x7
	#define  INTCONA 0x8
	#define  INTCONB 0x9
	// IOCON A & B point to same address
	#define   IOCONA 0xA
	#define   IOCONB 0xB
	//
	#define    GPPUA 0xC
	#define    GPPUB 0xD
	#define    INTFA 0xE
	#define    INTFB 0xF
	#define  INTCAPA 0x10
	#define  INTCAPB 0x11
	#define    GPIOA 0x12
	#define    GPIOB 0x13
	#define    OLATA 0x14
	#define    OLATB 0x15
#elif IOCON == 1
	#define   IODIRA 0x00
	#define   IODIRB 0x10
	#define    IPOLA 0x01
	#define    IPOLB 0x11
	#define GPINTENA 0x02
	#define GPINTENB 0x12
	#define  DEFVALA 0x03
	#define  DEFVALB 0x13
	#define  INTCONA 0x04
	#define  INTCONB 0x14
	#define   IOCONA 0x05
	#define   IOCONB 0x15
	#define    GPPUA 0x06
	#define    GPPUB 0x16
	#define    INTFA 0x07
	#define    INTFB 0x17
	#define  INTCAPA 0x08
	#define  INTCAPB 0x18
	#define    GPIOA 0x09
	#define    GPIOB 0x19
	#define    OLATA 0x0A
	#define    OLATB 0x1A
#endif

extern void twi_init(void);

uint8_t MCP23017_READ_BANK(uint8_t slave, uint8_t bank)
{
	uint8_t byte = 0;

	if (bank != GPIOA && bank != GPIOB)
	{return 1;}

	// re-initialize to clear SCL and state
	//twi_init();

	// start condition
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if (TWSR & 0xF8 != master_start)
	{return 2;}

	// send slave address, write mode
	TWDR = TWI_BASE_ADDR | slave << 1;
	TWCR = 1 << TWINT | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if (TWSR & 0xF8 != transmitter_slave_address_acknowledged)
	{
		// stop condition
		TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;
		return 3;
	}

	// ask to read from register
	TWDR = bank;
	TWCR = 1 << TWINT | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if (TWSR & 0xF8 != transmitter_byte_acknowledged)
	{
		// stop condition
		TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;
		return 4;
	}

	// send repeat start
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if (TWSR & 0xF8 != master_repeat_start)
	{
		// stop condition
		TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;
		return 5;
	}

	// send slave address, read mode
	TWDR = TWI_BASE_ADDR | slave << 1 | 1;
	TWCR = 1 << TWINT | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));

	if (TWSR & 0xF8 != receiver_slave_read_acknowledged)
	{
		// stop condition
		TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;
		return 6;
	}
	// send repeat start
	//TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	//while(! (TWCR & (1 << TWINT)));

	//if (TWSR & 0xF8 != master_repeat_start)
	//{
	//	// stop condition
	//	TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;
	//	return 5;
	//}

	// send slave address, read mode
	//TWDR = TWI_BASE_ADDR | slave << 1 | 1;
	//TWCR = 1 << TWINT | 1 << TWEN;
	//while(! (TWCR & (1 << TWINT)));

	//if (TWSR & 0xF8 != receiver_slave_read_acknowledged)
	//{
	//	// stop condition
	//	TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;
	//	return 6;
	//}

	// read data from slave
	TWCR = 1 << TWINT | 0 << TWEA | 1 << TWEN;
	while(! (TWCR & (1 << TWINT)));
	byte = TWDR;

	if (TWSR & 0xF8 != receiver_byte_not_acknowledged)
	{
		// stop condition
		TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;
		return 7;
	}

	// stop condition
	TWCR = 1 << TWSTO | 0 << TWSTA | 1 << TWINT | 1 << TWEN;

	return byte;
}

#endif
