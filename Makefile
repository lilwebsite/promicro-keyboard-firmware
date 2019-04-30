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
AVRF=-F -V -D
DFU=dfu-programmer $(MCU)

#info files
FUSES=lfuse.hex hfuse.hex efuse.hex
DUMP=dump.hex

#keyboard definitions
#
#as far as im aware, 126(+2 for modifier keys) is max key rollover with single buffer endpoint
#KEY_ROLLOVER takes up N*3 bytes of RAM (104 * 3 = 312 bytes, for example) and cannot be higher than the ENDPOINT_SIZE
KEY_ROLLOVER=64
DEMUX_OUT=16
DEMUX_IN=4
COLUMNS=8
ROWS=16
KEYS=102
#don't change ENDPOINT_SIZE, PACKET_SIZE or REPORT_COUNT unless you know what you're doing!
ENDPOINT_SIZE=64
PACKET_SIZE=64
REPORT_COUNT=62

DEFINES=-DF_CPU=$(FREQ) -DBAUD=$(BAUD) -DKEY_ROLLOVER=$(KEY_ROLLOVER) -DENDPOINT_SIZE=$(ENDPOINT_SIZE) -DPACKET_SIZE=$(PACKET_SIZE) -DREPORT_COUNT=$(REPORT_COUNT) -DDEMUX_OUT=$(DEMUX_OUT) -DDEMUX_IN=$(DEMUX_IN) -DCOLUMNS=$(COLUMNS) -DROWS=$(ROWS) -DKEYS=$(KEYS)

#compiler definitions
BIN=kbd
CC=avr-gcc
OBJCOPY=avr-objcopy
LDFLAGS=-Wl,-Map=$(strip $@).map -Wl,--start-group -Wl,-lm -Wl,--end-group -Wl,--gc-sections,--print-gc-sections
ifneq ($(DEBUG), yes)
CFLAGS=$(DEFINES) -mmcu=$(MCU) -Os -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -mrelax
else
CFLAGS=$(DEFINES) -mmcu=$(MCU) -Og -g -ggdb -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections
endif

#source files
SRC := $(wildcard kbd/*.c)
SRC += $(wildcard usb_keyboard/*.c)
SRC += $(wildcard promicro/*.c)

OBJS := $(SRC:%.c=%.o)

#compiler functions
$(BIN).hex: $(BIN).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BIN).eep: $(BIN).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $@

$(BIN).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	
%.o: %.c
	$(CC) -c $(CFLAGS) $(GENDEPFLAGS) $< -o $@

all: $(BIN).hex $(BIN).eep

#clean all files
fullclean:
	rm -f $(BIN).elf $(BIN).hex $(OBJS) $(DUMP) $(FUSES) $(BIN).elf.map $(BIN).eep

#clean without removing hex file
clean:
	rm -f $(BIN).elf $(OBJS) $(DUMP) $(FUSES) $(BIN).elf.map

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
