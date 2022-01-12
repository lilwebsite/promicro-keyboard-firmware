#micro definitions
MCU=atmega32u4
PROG=usbtiny
BAUD=9600
FREQ=16000000UL
LFUSE=0xBF
HFUSE=0xD8
EFUSE=0xC3

#programmer applications
AVR=avrdude
AVRF=-D
DFU=dfu-programmer $(MCU)

#info files
FUSES=lfuse.hex hfuse.hex efuse.hex
DUMP=dump.hex

#keyboard definitions
#
#as far as im aware, 126(+2 for modifier keys) is max key rollover with single buffer endpoint
#KEY_ROLLOVER takes up N*3 bytes of RAM (104 * 3 = 312 bytes, for example) and cannot be higher than the ENDPOINT_SIZE
#if MAKE_VARS not set the compiler will use defaults predefined in the code
MAKE_VARS=1
KEY_ROLLOVER=64
KEYBOARD=IBMPingmaster
DRIVER=SN74159N
LAYOUT=bigwebsite
# set to yes if you want to be able to print debug info using hid_listen
DEBUG_PRINT=yes
#don't change ENDPOINT_SIZE, PACKET_SIZE or REPORT_COUNT unless you know what you're doing!
ENDPOINT_SIZE=64
PACKET_SIZE=64
REPORT_COUNT=62

DEFINES=-DMAKEVARS -DF_CPU=$(FREQ) -DBAUD=$(BAUD) -DKEY_ROLLOVER=$(KEY_ROLLOVER) -DENDPOINT_SIZE=$(ENDPOINT_SIZE) -DPACKET_SIZE=$(PACKET_SIZE) -DREPORT_COUNT=$(REPORT_COUNT)

#compiler definitions
BIN=kbd
CC=avr-gcc
OBJCOPY=avr-objcopy
LDFLAGS=-Wl,-Map=$(strip $@).map -Wl,--start-group -Wl,-lm -Wl,--end-group -Wl,--gc-sections,--print-gc-sections
ifeq ($(DEBUG_PRINT), yes)
DEFINES+= -DDEBUG_PRINT
endif
ifneq ($(DEBUG), yes)
#CFLAGS=$(DEFINES) -mmcu=$(MCU) -O3 -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -ffunction-sections -mrelax
#CFLAGS=$(DEFINES) -mmcu=$(MCU) -O3 -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections
CFLAGS=$(DEFINES) -mmcu=$(MCU) -O3 -Werror -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections
else
CFLAGS=$(DEFINES) -mmcu=$(MCU) -Og -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields
endif

#source files
SRC := usb/usb.c
SRC += drivers/twi/twi.c
SRC += $(wildcard keyboards/*/$(KEYBOARD)/keyboard.c)
SRC += $(wildcard kbd/*.c)
SRC += $(wildcard promicro/*.c)
SRC += $(wildcard drivers/*/$(DRIVER).c)

#KBD_OBJS := $(KBD:%.c=%.o)
OBJS += $(SRC:%.c=%.o)

#compiler functions
$(BIN).hex: $(BIN).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BIN).eep: $(BIN).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $@

$(BIN).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
#	$(CC) -E $(CFLAGS) $(GENDEPFLAGS) -I ./ -I $(wildcard keyboards/*/$(KEYBOARD)/) -I $(wildcard layouts/$(KEYBOARD)/$(LAYOUT)) $< -o $@.precompile
	$(CC) -c $(CFLAGS) $(GENDEPFLAGS) -I ./ -I $(wildcard keyboards/*/$(KEYBOARD)/) -I $(wildcard layouts/$(KEYBOARD)/$(LAYOUT)) $< -o $@

all: $(BIN).hex $(BIN).eep

#clean all files
fullclean:
	rm -f $(BIN).elf $(BIN).hex $(OBJS) $(DUMP) $(FUSES) $(BIN).elf.map $(BIN).eep

#clean without removing hex file
clean:
	rm -f $(BIN).elf $(OBJS) $(SRC:%.c=%.o.precompile) $(DUMP) $(FUSES) $(BIN).elf.map 

##################
#helper functions#
##################
ifeq ($(FLASH_DFU), no)
flash: $(BIN).hex
	$(AVR) $(AVRF) -c$(PROG) -p$(MCU) -b${BAUD} -Uflash:w:$<:i -Ulock:w:0x0f:m
	
flash-bootloader:
	$(AVR) $(AVRF) -c$(PROG) -p$(MCU) -b${BAUD} -Uflash:w:bootloader/BootloaderDFU.hex:i -Ulock:w:0x0f:m

readflash:
	$(AVR) $(AVRF) -c$(PROG) -p$(MCU) -Uflash:r:$(DUMP):i
	
readfuses:
	$(AVR) -c$(PROG) -p$(MCU) -Ulfuse:r:lfuse.hex:h -Uhfuse:r:hfuse.hex:h -Uefuse:r:efuse.hex:h

erasechip:
	$(AVR) -p$(MCU) -c$(PROG) -e -u -Ulock:w:0x3f:m -Ulfuse:w:$(LFUSE):m -Uhfuse:w:$(HFUSE):m -Uefuse:w:$(EFUSE):m

validate: $(BIN).hex
	$(AVR) -p$(MCU) -c$(PROG) -Uflash:v:$<:i -Ulfuse:v:$(LFUSE):m -Uhfuse:v:$(HFUSE):m -Uefuse:v:$(EFUSE):m

else

flash: $(BIN).hex
	$(DFU) flash $<

readflash:
	$(DFU) read > $(DUMP)

erasechip:
	$(DFU) erase --force

launch:
	$(DFU) launch
endif

finish:
ifneq ($(MAKE_ERR), no)
	@echo
	@echo 'make failed and was unable to generate hex file'
else
ifneq ($(FLASH_ERR), no)
	@echo
	@echo 'flashing failed but hex file exists and is OK to use'
else
ifneq ($(FLASH_DEVICE), no)
ifneq ($(HICCUP), no)
	@echo
	@echo 'flashing had issues but ultimately succeeded... device flashed!'
else
	@echo
	@echo "AVR-GCC completed... successfully flashed device!"
endif
else
	@echo
	@echo "AVR-GCC completed... Successfully generated hex file!"
endif
endif
	@echo "// // // // // // // // // // // // // // //"
	avr-size kbd.hex
	@echo "// // // // // // // // // // // // // // //"
endif
