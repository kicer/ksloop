## Set Mcu Type
DEVICE=ch552e
CPU=CH552

## RAM/ROM code size/map
XRAM_SIZE = 0x0400
XRAM_LOC = 0x0000
CODE_SIZE = 0x3800

## A directory for common include files
BSP = bsp

## Get program name from enclosing directory name
TARGET = objects/$(lastword $(subst /, ,$(CURDIR)))

SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.rel)

HEADERS=$(wildcard src/inc/*.h $(BSP)/inc/*.h)

CC = /Developer/sdcc/bin/sdcc
OBJCOPY = /Developer/sdcc/bin/sdobjcopy
FLASHER = /Developer/librech551/wchisptool

CFLAGS := -mmcs51 --model-large \
    --xram-size $(XRAM_SIZE) --xram-loc $(XRAM_LOC) \
    --code-size $(CODE_SIZE) \
    -I$(BSP)/inc -Isrc/inc \
    -D$(CPU)
LDFLAGS := $(CFLAGS)


.PHONY: all clean flash factory erase

$(TARGET).bin: $(TARGET).ihx
	$(OBJCOPY) -I ihex -O binary $(TARGET).ihx $(TARGET).bin

$(TARGET).ihx: $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

%.rel : %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

CCOMPILEDFILES=$(SOURCES:.c=.asm) $(SOURCES:.c=.lst) $(SOURCES:.c=.rel) \
               $(SOURCES:.c=.rst) $(SOURCES:.c=.sym)
clean:
	rm -f $(TARGET).bin $(TARGET).ihx $(TARGET).cdb $(TARGET).lk $(TARGET).map $(TARGET).mem $(CCOMPILEDFILES)

flash: $(TARGET).bin
	$(FLASHER) -g -f $(TARGET).bin

erase:
	$(FLASHER) -e

release: erase flash
