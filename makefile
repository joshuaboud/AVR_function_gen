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
MCU_TARGET  = atmega328p
OPTIMIZE	   = -Os
DEFS		   =
LIBS		   =
PROGRAMMER = usbtiny
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
