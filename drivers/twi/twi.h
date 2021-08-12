#ifndef __twi
#define __twi
#include <kbd/globals.h>
#include "globals.h"

uint8_t twi_status = twi_unset;
uint8_t twi_failed = 0;

#define twi_buffer_limit 10 // arbitrary limit
uint8_t twi_buffer_index = 0;
uint8_t twi_buffer[twi_buffer_limit];

void twi_init(void); // initialize the registers / pins
void twi_set_slave(uint8_t address); // select target device
uint8_t twi_check_status(uint8_t expected); // checks for errors, false on failure
uint8_t twi_append_data(uint8_t data); // append data to send (includes register address if reading)
uint8_t twi_commit(uint8_t write); // start the transaction
void twi_send(uint8_t message); // send data
uint8_t twi_read(void); // read operation
void twi_start_condition(void);
void twi_stop_condition(void);
#endif
