#include "twi.h"

uint8_t twi_check_status(uint8_t expected)
{
	twi_failed = 1;
	twi_status = TWSR & 0xF8;
	switch(twi_status)
	{
		case master_start:
		case master_repeat_start:
		case transmitter_slave_address_acknowledged:
		case transmitter_byte_acknowledged:
		case receiver_slave_read_acknowledged:
		case receiver_byte_acknowledged:
			twi_failed = 0;
			break;
	}

	if(expected != twi_unset && twi_status != expected)
	{twi_failed = 1;}

	return twi_failed;
}

void twi_init()
{
	TWBR = 0x0F;
	set_PORTX_bit(1, D, 1);
	set_DDRX_bit(1, D, 1);
	set_PORTX_bit(0, D, 1);
	set_DDRX_bit(0, D, 1);

	twi_buffer_index = 0;
}

void twi_set_slave(uint8_t address)
{
	twi_buffer_index = 0;
	twi_buffer[0] = address;
}

uint8_t twi_append_data(uint8_t data)
{
	if(twi_buffer_index >= twi_buffer_limit)
	{return 0;}

	twi_buffer_index++;
	twi_buffer[twi_buffer_index] = data;

	return 1;
}

uint8_t twi_commit(uint8_t write)
{
	uint8_t result = 0xFF;

	twi_status = twi_unset;

	twi_start_condition();
	if(twi_check_status(master_start))
	{return 0;}

	twi_send(twi_buffer[0] & 0xFE);
	if(twi_check_status(transmitter_slave_address_acknowledged))
	{return 0;}

	twi_send(twi_buffer[1]);
	if(twi_check_status(transmitter_byte_acknowledged))
	{return 0;}

	if(write)
	{
		twi_send(twi_buffer[2]);
		if(twi_check_status(transmitter_byte_acknowledged))
		{return 0;}
	}
	else
	{
		twi_start_condition();
		if(twi_check_status(master_repeat_start))
		{return 0;}

		twi_send(twi_buffer[0] | 1);
		if(twi_check_status(receiver_slave_read_acknowledged))
		{return 0;}

		result = twi_read();
	}

	twi_stop_condition();

	return result;
}

void twi_send(uint8_t message)
{
	TWDR = message;
	TWCR =  (1 << TWINT) |
			(1 << TWEN);
	if(message == twi_buffer[0] | 1)
	{set_PINX_input(1, D, 0);}
	while(!(TWCR & (1 << TWINT)));
}

uint8_t twi_read(void)
{
	TWCR =  (1 << TWINT) |
			(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

void twi_start_condition(void)
{
	TWCR =  (1 << TWINT) |
			(1 << TWSTA) |
			(1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
}

void twi_stop_condition(void)
{
	TWCR =  (1 << TWINT) |
			(1 << TWSTO) |
			(1 << TWEN);
}
