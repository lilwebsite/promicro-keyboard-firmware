#include <kbd/globals.h>

//position: what port # to set high/low
//value: set position high(1) or low(0)
//port: letter for which port to set (B, C, D, E, F)
uint8_t set_PORTX_bit(uint8_t position, uint8_t value, const char port)
{
	//if (value == 0)
	//{
	//	PORTB &= ~(1 << position);//set port bit LOW
	//}
	//else
	//{
	//	PORTB |= (1 << position);//set port bit HIGH
	//}
	switch (port)
	{
	case B:
		(value) ? (PORTB |= (1 << position)) : (PORTB &= ~(1 << position));
		break;
	case C:
		(value) ? (PORTC |= (1 << position)) : (PORTC &= ~(1 << position));
		break;
	case D:
		(value) ? (PORTD |= (1 << position)) : (PORTD &= ~(1 << position));
		break;
	case E:
		(value) ? (PORTE |= (1 << position)) : (PORTE &= ~(1 << position));
		break;
	case F:
		(value) ? (PORTF |= (1 << position)) : (PORTF &= ~(1 << position));
		break;
	default:
		return 0;
	}
	return 1;
}

uint8_t set_DDRX_bit(uint8_t position, uint8_t value, const char port)
{
	switch (port)
	{
	case B:
		(value) ? (DDRB |= (1 << position)) : (DDRB &= ~(1 << position));
		break;
	case C:
		(value) ? (DDRC |= (1 << position)) : (DDRC &= ~(1 << position));
		break;
	case D:
		(value) ? (DDRD |= (1 << position)) : (DDRD &= ~(1 << position));
		break;
	case E:
		(value) ? (DDRE |= (1 << position)) : (DDRE &= ~(1 << position));
		break;
	case F:
		(value) ? (DDRF |= (1 << position)) : (DDRF &= ~(1 << position));
		break;
	default:
		return 0;
	}
	return 1;
}

uint8_t read_PINX_bit(uint8_t position, const char port)
{
	switch (port)
	{
	case B:
		return PINB & (1<<position);
		break;
	case C:
		return PINC & (1<<position);
		break;
	case D:
		return PIND & (1<<position);
		break;
	case E:
		return PINE & (1<<position);
		break;
	case F:
		return PINF & (1<<position);
		break;
	default:
		return 0;
	}
	return 0;
}

void set_PINX_variable_output(uint8_t position, const char port, uint8_t output)
{
	set_PORTX_bit(position, output, port);
	set_DDRX_bit(position, 1, port);//sets direction to output
	return;
}

void set_PINX_output(uint8_t position, const char port)
{
	set_PINX_variable_output(position, port, 0);//low output
	return;
}

void set_PINX_input(uint8_t position, const char port, uint8_t pullup)//pullup -> 1 == pullup resistance , 0 == tri-state(no pullup)
{
	set_PORTX_bit(position, pullup, port);
	set_DDRX_bit(position, 0, port);//set direction to input
	return;
}

uint8_t u4_reverse(uint8_t num)//MSB <--> LSB
{
	num = ((num & 0xA) >> 1) | ((num & 0x5) << 1);
	num = ((num & 0xC) >> 2) | ((num & 0x3) << 2);
	return num;
}

uint8_t u8_reverse(uint8_t num)//MSB <--> LSB
{
	num = ((num & 0xAA) >> 1) | ((num & 0x55) << 1);
	num = ((num & 0xCC) >> 2) | ((num & 0x33) << 2);
	return num;
}
