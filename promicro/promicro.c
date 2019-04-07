#include "promicro.h"

uint8_t init_inputs(void)
{
/* 	//input pins from keyboard matrix
	set_PINX_input(1, D, 1);//pin 2
	set_PINX_input(0, D, 1);//pin 3
	set_PINX_input(4, D, 1);//pin 4
	set_PINX_input(6, C, 1);//pin 5
	set_PINX_input(7, D, 1);//pin 6
	set_PINX_input(6, E, 1);//pin 7
	set_PINX_input(4, B, 1);//pin 8
	set_PINX_input(5, B, 1);//pin 9 */
	
	uint8_t pullup = 1;
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		set_PINX_output(input_pins[x].position, input_pins[x].port);
		set_PINX_input(input_pins[x].position, input_pins[x].port, pullup);
	}

	return 1;
}
/* 
void read_pins(void)
{
	for(uint8_t x = 0; x < COLUMNS; x++)
	{
		ispressed[x] = read_PINX_bit(input_pins[x].position, input_pins[x].port);
	}
 	ispressed[0] = PINB & (1<<5);
	ispressed[1] = PINB & (1<<4);
	ispressed[2] = PINE & (1<<6);
	ispressed[3] = PIND & (1<<7);
	ispressed[4] = PINC & (1<<6);
	ispressed[5] = PIND & (1<<4);
	ispressed[6] = PIND & (1<<0);
	ispressed[7] = PIND & (1<<1);
	return;
} */

uint8_t init_pins(void)
{
	//pins are based on a pro micro in order
	//output pins for the decoder
	set_PINX_output(4, F);//pin A0
	set_PINX_output(5, F);//pin A1
	set_PINX_output(6, F);//pin A2
	set_PINX_output(7, F);//pin A3

	//if(!init_inputs()){return 0;}
	init_inputs();
	
	//USB
	set_PINX_input(3, D, 0);
	set_PINX_input(2, D, 0);
	
	//not used
	set_PINX_input(6, B, 1);//pin 10
	set_PINX_input(2, B, 1);//pin 16
	set_PINX_input(3, B, 1);//pin 14
	set_PINX_input(1, B, 1);//pin 15
	/* set_PINX_input(4, F, 0);//pin A0
	set_PINX_input(5, F, 0);//pin A1
	set_PINX_input(6, F, 0);//pin A2
	set_PINX_input(7, F, 0);//pin A3 */
	set_PINX_input(0, F, 0);
	set_PINX_input(1, F, 0);
	set_PINX_input(2, E, 0);
	set_PINX_input(5, D, 0);
	set_PINX_input(6, D, 0);
	set_PINX_input(7, C, 0);
	set_PINX_input(0, B, 0);
	set_PINX_input(7, B, 0);
	
	return 1;
}

uint8_t init_promicro(void)
{
	//boot_program_page();//bootloader
	
	CPU_PRESCALE(CPU_16MHz);
	
	//if(!init_pins()){return 0;}
	
	init_pins();
	//DIEOEB &= ~(1<<5);
	//PVOVB &= ~(1<<5);
	
	//TCCR1A  = 0b10101001;//set and configure fast PWM
	//TCCR1B  = 0b00001001;
	
	return 1;
}