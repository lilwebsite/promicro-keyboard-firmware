#ifndef __BASE
//includes
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/setbaud.h>
#include <util/delay.h>
//user includes
#include "types.h"
//define no-operation
#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)
//convenience macros
#define LOW 0
#define HIGH 1

//#define DEMUX_OUT 16//how many rows the demultiplexer can pull low
//#define DEMUX_IN 4//how many inputs the demultiplexer has
//#define COLUMNS 8//usable columns
//#define ROWS 16//usable rows -- should be the same as DEMUX_OUT
//#define KEYS 102//how many keys on the keyboard
//#define KEY_ROLLOVER 8//how many keys can be pressed at one time

#define __BASE
#endif