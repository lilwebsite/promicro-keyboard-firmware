#define B 'B'
#define C 'C'
#define D 'D'
#define E 'E'
#define F 'F'

// port B
#define PB0 B, 0
#define PB1 B, 1
#define PB2 B, 2
#define PB3 B, 3
#define PB4 B, 4
#define PB5 B, 5
#define PB6 B, 6
#define PB7 B, 7

// port C
#define PC6 C, 6
#define PC7 C, 7

// port D
#define PD0 D, 0
#define PD1 D, 1
#define PD2 D, 2
#define PD3 D, 3
#define PD4 D, 4
#define PD5 D, 5
#define PD6 D, 6
#define PD7 D, 7

// port E 
#define PE2 E, 2
#define PE3 E, 3
#define PE4 E, 4
#define PE5 E, 5
#define PE6 E, 6
#define PE7 E, 7

// port F
#define PF0 F, 0
#define PF1 F, 1
#define PF4 F, 4
#define PF5 F, 5
#define PF6 F, 6
#define PF7 F, 7

// pin # mapping
#define PIN1  PB0
#define PIN2  PB1
#define PIN3  PB2
#define PIN4  PB3
#define PIN5  PB4
#define PIN6  PB5
#define PIN7  PB6
#define PIN8  PB7

#define PIN9  PC6
#define PIN10 PC7

#define PIN11 PD0
#define PIN12 PD1
#define PIN13 PD2
#define PIN14 PD3
#define PIN15 PD4
#define PIN16 PD5
#define PIN17 PD6
#define PIN18 PD7

#define PIN19 PE2
#define PIN20 PE3
#define PIN21 PE4
#define PIN22 PE5
#define PIN23 PE6
#define PIN24 PE7

#define PIN25 PF0
#define PIN26 PF1
#define PIN27 PF4
#define PIN28 PF5
#define PIN29 PF6
#define PIN30 PF7

#define CONTROLLER_INIT if (CLOCK != 0x00) { CLKPR = 0x80; CLKPR = CLOCK; }
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)
#define DEVICE_RESET() do {wdt_enable(WDTO_15MS); for(;;) {}} while(0)
