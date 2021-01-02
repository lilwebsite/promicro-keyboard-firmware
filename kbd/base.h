#ifndef __BASE

#ifdef IBMPingmaster
//pingmaster defines
#define KBD_IBMPingmaster
#define DEMUX_OUT 16
#define DEMUX_IN 4
#define COLUMNS 8
#define ROWS 16
#define KEYS 102
#endif

#ifdef PC8801
//pc8801 defines
#define KBD_PC8801
#define DEMUX_OUT 14
#define DEMUX_IN 4
#define COLUMNS 8
#define ROWS 14
#define KEYS 
#endif

//includes
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/setbaud.h>
#include <util/delay.h>
//project includes
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
