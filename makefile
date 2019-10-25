# ----------------------------------------------------------------------------
# "THE BEER-WARE LICENSE" (Revision 42):
# <joerg@FreeBSD.ORG> and <jboudreau@45drives.com> wrote this file.
# As long as you retain this notice you can do whatever you want with this
# stuff. If we meet some day, and you think this stuff is worth it, 
# you can buy me a beer in return.        Joerg Wunsch
# ----------------------------------------------------------------------------
#
# This make file was taken from www.nongnu.org/avr-libc/user-manual/group__demo__project.html
# during a desperate attempt to get avr-gcc to work for me. I (Josh Boudreau) have added the
# flash and size targets.

PRG			= main
OBJ			= main.o
#MCU_TARGET	 = at90s2313
#MCU_TARGET	 = at90s2333
#MCU_TARGET	 = at90s4414
#MCU_TARGET	 = at90s4433
#MCU_TARGET	 = at90s4434
#MCU_TARGET	 = at90s8515
#MCU_TARGET	 = at90s8535
#MCU_TARGET	 = atmega128
#MCU_TARGET	 = atmega1280
#MCU_TARGET	 = atmega1281
#MCU_TARGET	 = atmega1284p
#MCU_TARGET	 = atmega16
#MCU_TARGET	 = atmega163
#MCU_TARGET	 = atmega164p
#MCU_TARGET	 = atmega165
#MCU_TARGET	 = atmega165p
#MCU_TARGET	 = atmega168
#MCU_TARGET	 = atmega169
#MCU_TARGET	 = atmega169p
#MCU_TARGET	 = atmega2560
#MCU_TARGET	 = atmega2561
#MCU_TARGET	 = atmega32
#MCU_TARGET	 = atmega324p
#MCU_TARGET	 = atmega325
#MCU_TARGET	 = atmega3250
#MCU_TARGET	 = atmega329
#MCU_TARGET	 = atmega3290
#MCU_TARGET	 = atmega32u4
#MCU_TARGET	 = atmega48
#MCU_TARGET	 = atmega64
#MCU_TARGET	 = atmega640
#MCU_TARGET	 = atmega644
#MCU_TARGET	 = atmega644p
#MCU_TARGET	 = atmega645
#MCU_TARGET	 = atmega6450
#MCU_TARGET	 = atmega649
#MCU_TARGET	 = atmega6490
#MCU_TARGET	 = atmega8
#MCU_TARGET	 = atmega8515
#MCU_TARGET	 = atmega8535
#MCU_TARGET	 = atmega88
#MCU_TARGET	 = attiny2313
#MCU_TARGET	 = attiny24
#MCU_TARGET	 = attiny25
#MCU_TARGET	 = attiny26
#MCU_TARGET	 = attiny261
#MCU_TARGET	 = attiny44
#MCU_TARGET	 = attiny45
#MCU_TARGET	 = attiny461
#MCU_TARGET	 = attiny84
#MCU_TARGET	 = attiny85
#MCU_TARGET	 = attiny861
MCU_TARGET  = atmega328p
OPTIMIZE	   = -Os
DEFS		   =
LIBS		   =
PROGRAMMER = avrispmkII
PROG_TARGET = m328p
PROG_PORT = usb
# You should not have to change anything below here.
CC			 = avr-gcc
# Override is only needed by avr-lib build system.
override CFLAGS		= -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS	   = -Wl,-Map,$(PRG).map
OBJCOPY		= avr-objcopy
OBJDUMP		= avr-objdump
all: $(PRG).elf lst text eeprom
$(PRG).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
# dependency:
demo.o: demo.c iocompat.h
clean:
	rm -rf *.o $(PRG).elf *.eps *.png *.pdf *.bak 
	rm -rf *.lst *.map $(EXTRA_CLEAN_FILES)
lst:  $(PRG).lst
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@
# Rules for building the .text rom images
text: hex bin srec
hex:  $(PRG).hex
bin:  $(PRG).bin
srec: $(PRG).srec
%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
# Rules for building the .eeprom rom images
eeprom: ehex ebin esrec
ehex:  $(PRG)_eeprom.hex
ebin:  $(PRG)_eeprom.bin
esrec: $(PRG)_eeprom.srec
%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ || { echo empty $@ not generated; exit 0; }
%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@ || { echo empty $@ not generated; exit 0; }
%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@ || { echo empty $@ not generated; exit 0; }
# Every thing below here is used by avr-libc's build system and can be ignored
# by the casual user.
FIG2DEV				 = fig2dev
EXTRA_CLEAN_FILES	   = *.hex *.bin *.srec
dox: eps png pdf
eps: $(PRG).eps
png: $(PRG).png
pdf: $(PRG).pdf
%.eps: %.fig
	$(FIG2DEV) -L eps $< $@
%.pdf: %.fig
	$(FIG2DEV) -L pdf $< $@
%.png: %.fig
	$(FIG2DEV) -L png $< $@
# avrdude flashing
flash:
	avrdude -p $(PROG_TARGET) -c $(PROGRAMMER) -P $(PROG_PORT) -U flash:w:$(PRG).elf:e -v
size:
	avr-size -C --mcu=$(MCU_TARGET) $(PRG).elf
