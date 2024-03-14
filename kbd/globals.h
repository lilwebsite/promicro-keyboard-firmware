// includes
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include "types.h"
#include <interfaces/usb/globals.h>
#include <interfaces/twi/globals.h>

// define no-operation
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)

// convenience macros
#define LOW 0
#define HIGH 1

// send keyboard codes after processing
extern void send_report(uint8_t iface_num);
#define kbsend() send_report(KBD_INTERFACE) // send report for keyboard

// pin functions
extern uint8_t read_pin(const char port, uint8_t position);
extern void pin_value(const char port, uint8_t position, uint8_t value);
extern void pin_output(const char port, uint8_t position);
extern void pin_input(const char port, uint8_t position, uint8_t pullup);

void init(void);
void set(uint8_t row);
