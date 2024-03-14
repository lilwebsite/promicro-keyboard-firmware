#include <kbd/globals.h>
//#include <drivers/twi/twi.h>
#include <drivers/twi/globals.h>
#include "settings.h"
#include "pinconfig.h"

extern void twi_init(void);
//extern void twi_set_slave(uint8_t address);
//extern uint8_t twi_commit(uint8_t write);
//extern void twi_append_data(uint8_t data);
extern uint8_t twi_action(struct twi_target *action);

struct twi_target ABANK_DIR = SET_TARGET(IODIRA, WRITE);
struct twi_target BBANK_DIR = SET_TARGET(IODIRB, WRITE);

struct twi_target ABANK_GPPU = SET_TARGET(GPPUA, WRITE);
struct twi_target BBANK_GPPU = SET_TARGET(GPPUB, WRITE);
