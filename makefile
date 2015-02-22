#
# libhd44780
# Copyright (C) 2015 David Jolly
# ----------------------
#
# libhd44780 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libhd44780 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# Change these to match your device
# NOTE: frequencies less than 8MHz might not work!
F_CPU=1000000
DEV=atmega328p
DEV_SRT=m328p

CC=avr-gcc
CC_FLG=-Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(DEV)

BIN=./bin/
BUILD=./build/
EX0=sample
HD=hd44780
LIB_INC=./src/lib/include/
LIB_SRC=./src/lib/src/
SAMPLE=./src/sample/

all: clean init sample

# Init/Uninit tasks

clean:
	rm -rf $(BIN)
	rm -rf $(BUILD)

init:
	mkdir $(BIN)
	mkdir $(BUILD)

# Build/Flash samples

sample:
	@echo ""
	@echo "============================================"
	@echo "BUILDING SAMPLE"
	@echo "============================================"
	$(CC) $(CC_FLG) -c $(LIB_SRC)$(HD).c -o $(BUILD)$(HD).o -Wa,-ahl=$(BUILD)$(HD).s
	$(CC) $(CC_FLG) -c $(SAMPLE)$(EX0).c -o $(BUILD)$(EX0).o -Wa,-ahl=$(BUILD)$(EX0).s
	$(CC) $(CC_FLG) -o $(BUILD)$(EX0).elf $(BUILD)$(EX0).o $(BUILD)$(HD).o
	avr-objcopy -j .text -j .data -O ihex $(BUILD)$(EX0).elf $(BIN)$(EX0).hex
	avr-size --format=avr --mcu=$(DEV) $(BUILD)$(EX0).elf

sample_flash:
	@echo ""
	@echo "============================================"
	@echo "FLASHING SAMPLE"
	@echo "============================================"
	avrdude -p $(DEV_SRT) -P usb -c usbtiny -U flash:w:$(BIN)$(EX0).hex
