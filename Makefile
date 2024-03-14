# this makefile will updated at some point...
MCU=atmega32u4
BAUD=9600
F_CPU=16000000UL
PROG_VER=1.0

KEYBOARD=39-LI-0101
LAYOUT=lilwebsite

BIN=hex/$(KEYBOARD)-$(LAYOUT)
CC=avr-gcc
OBJCOPY=avr-objcopy

DEFINES=-DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -DKEYBOARD=$(KEYBOARD) -DPROG_VER=$(PROG_VER)

LDFLAGS=-Wl,--start-group -Wl,-lm -Wl,--end-group -Wl,--gc-sections,--print-gc-sections -Wl,--relax -Wl,--warn-common

CFLAGS=-std=c99 -mmcu=$(MCU) -O3 -Werror -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fdata-sections -ffunction-sections

INCLUDES := -I ./
INCLUDES += -I $(wildcard keyboards/*/$(KEYBOARD)/)
INCLUDES += -I $(wildcard keyboards/*/$(KEYBOARD)/layouts/$(LAYOUT))
INCLUDES += -I $(wildcard hardware/)
INCLUDES += -include $(wildcard hardware/platform/*/$(MCU).h)

SRC := kbd/kbd.c
SRC += $(wildcard hardware/functions/*.c)
SRC += $(wildcard hardware/interfaces/*/*.c)
SRC += $(wildcard keyboards/*/$(KEYBOARD)/*.c)

OBJS += $(SRC:%.c=%.o)
PRE += $(SRC:%.c=debugging/%.precompile)
ASM += $(SRC:%.c=debugging/%.asm)

$(BIN).hex: $(BIN).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BIN).eep: $(BIN).elf
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $@

$(BIN).elf: $(OBJS)
	mkdir -p hex
	$(CC) $(CFLAGS) $(DEFINES) $(LDFLAGS) -Wl,-Map=$(strip $@).map -o $@ $^

debugging_targets: $(PRE) $(ASM) 

%.o: %.c
	$(CC) -c -fverbose-asm $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

debugging/%.asm: %.c 
	mkdir -p $(dir $@)
	$(CC) -S -fverbose-asm $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

debugging/%.precompile: %.c
	mkdir -p $(dir $@)
	$(CC) -E $(CFLAGS) $(DEFINES) $(INCLUDES) $< -o $@

all: debugging_targets $(BIN).hex $(BIN).eep

fullclean: clean
	rm -f $(BIN).hex

clean:
	rm -f $(BIN).elf $(BIN).elf.map $(OBJS) $(SRC:%.c=debugging/%.o.precompile) $(SRC:%.c=debugging/%.o.asm)
	rm -rf debugging
