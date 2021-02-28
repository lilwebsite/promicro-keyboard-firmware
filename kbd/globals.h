//includes
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include "types.h"
//define no-operation
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)
//convenience macros
#define LOW 0
#define HIGH 1
