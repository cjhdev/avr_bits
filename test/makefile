DIR_ROOT := ../
DIR_BUILD := build
DIR_BIN := bin

CC := avr-gcc

VPATH += $(DIR_ROOT)/src
VPATH += .

INCLUDES += -I$(DIR_ROOT)/include
INCLUDES += -I.

SRC := $(notdir $(wildcard $(DIR_ROOT)/src/*.c))
OBJ := $(SRC:.c=.o)

MCU := atmega328p

CFLAGS += -mmcu=$(MCU)
CFLAGS += -Os -Wall -std=gnu99 -g

CFLAGS += -funsigned-char
CFLAGS += -funsigned-bitfields
CFLAGS += -fpack-struct
CFLAGS += -fshort-enums 
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -ffreestanding
CFLAGS += -fno-split-wide-types
CFLAGS += -mcall-prologues
CFLAGS += -Wl,--gc-sections
CFLAGS += -fno-inline-small-functions

CFLAGS += $(INCLUDES)
CFLAGS += -DF_CPU=8000000UL

all: $(DIR_BUILD)/avr_bits.a

$(DIR_BUILD)/avr_bits.a: $(addprefix $(DIR_BUILD)/, $(OBJ))
	@ avr-gcc-ar cr $@ $^

$(DIR_BUILD)/%.o: %.c
	@ echo building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ echo cleaning up objects
	@ rm -f $(DIR_BUILD)/*

squeaky_clean: clean
	@ echo cleaning up images
	@ rm -f $(DIR_BIN)/*

.PHONY: size clean squeaky_clean all



