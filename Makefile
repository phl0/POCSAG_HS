#  Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
#  Copyright (C) 2016 by Jim McLaughlin KI6ZUM

#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# POCSAG source files
POCSAG_HS_PATH=.

# STM32 library paths
F1_LIB_PATH=./STM32F10X_Lib

# MCU external clock frequency (Hz)
CLK_PI_F4=12000000
CLK_DEF=8000000

# Directory Structure
BINDIR=bin
OBJDIR_F1=obj_f1

# Output files
BINELF_F1=pocsag_f1.elf
BINHEX_F1=pocsag_f1.hex
BINBIN_F1=pocsag_f1.bin
BINELF_F1BL=pocsag_f1bl.elf
BINHEX_F1BL=pocsag_f1bl.hex
BINBIN_F1BL=pocsag_f1bl.bin

# Header directories
INC_F1= . $(F1_LIB_PATH)/CMSIS/ $(F1_LIB_PATH)/Device/ $(F1_LIB_PATH)/STM32F10x_StdPeriph_Driver/inc/ $(F1_LIB_PATH)/usb/inc/
INCLUDES_F1=$(INC_F1:%=-I%)

# CMSIS libraries
INCLUDES_LIBS_F1=

# STM32F1 Standard Peripheral Libraries source path
STD_LIB_F1=$(F1_LIB_PATH)/STM32F10x_StdPeriph_Driver/src

# STM32F1 USB support source path
USB_F1=$(F1_LIB_PATH)/usb

# STM32F1 system source path
SYS_DIR_F1=$(F1_LIB_PATH)/Device
STARTUP_DIR_F1=$(F1_LIB_PATH)/Device/startup

# GNU ARM Embedded Toolchain
CC=arm-none-eabi-gcc
CXX=arm-none-eabi-g++
LD=arm-none-eabi-ld
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump
NM=arm-none-eabi-nm
SIZE=arm-none-eabi-size
A2L=arm-none-eabi-addr2line

# Configure vars depending on OS
ifeq ($(OS),Windows_NT)
	CLEANCMD=del /S *.o *.hex *.bin *.elf
	MDDIRS=md $@
	DFU_UTIL=./$(F1_LIB_PATH)/utils/win/dfu-util.exe
	STM32FLASH=./$(F1_LIB_PATH)/utils/win/stm32flash.exe
else
	CLEANCMD=rm -f $(OBJ_F1BL) $(BINDIR)/*.hex $(BINDIR)/pocsag_f1.bin $(BINDIR)/*.elf
	MDDIRS=mkdir $@
	
    ifeq ($(shell uname -s),Linux)
    	ifeq ($(shell uname -m),x86_64)
			DFU_RST=./$(F1_LIB_PATH)/utils/linux64/upload-reset
			DFU_UTIL=./$(F1_LIB_PATH)/utils/linux64/dfu-util
			ST_FLASH=./$(F1_LIB_PATH)/utils/linux64/st-flash
			STM32FLASH=./$(F1_LIB_PATH)/utils/linux64/stm32flash
    	else ifeq ($(shell uname -m),armv7l)
			DFU_RST=./$(F1_LIB_PATH)/utils/rpi32/upload-reset
			DFU_UTIL=./$(F1_LIB_PATH)/utils/rpi32/dfu-util
			ST_FLASH=./$(F1_LIB_PATH)/utils/rpi32/st-flash
			STM32FLASH=./$(F1_LIB_PATH)/utils/rpi32/stm32flash
    	else ifeq ($(shell uname -m),armv6l)
			DFU_RST=./$(F1_LIB_PATH)/utils/rpi32/upload-reset
			DFU_UTIL=./$(F1_LIB_PATH)/utils/rpi32/dfu-util
			ST_FLASH=./$(F1_LIB_PATH)/utils/rpi32/st-flash
			STM32FLASH=./$(F1_LIB_PATH)/utils/rpi32/stm32flash
		else
			DFU_RST=./$(F1_LIB_PATH)/utils/linux/upload-reset
			DFU_UTIL=./$(F1_LIB_PATH)/utils/linux/dfu-util
			ST_FLASH=./$(F1_LIB_PATH)/utils/linux/st-flash
			STM32FLASH=./$(F1_LIB_PATH)/utils/linux/stm32flash
    	endif
    endif

    ifeq ($(shell uname -s),Darwin)
		DFU_RST=./$(F1_LIB_PATH)/utils/macosx/upload-reset
		DFU_UTIL=./$(F1_LIB_PATH)/utils/macosx/dfu-util
		ST_FLASH=./$(F1_LIB_PATH)/utils/macosx/st-flash
		STM32FLASH=./$(F1_LIB_PATH)/utils/macosx/stm32flash
    endif
endif

# Default reference oscillator frequencies
ifndef $(OSC)
	ifeq ($(MAKECMDGOALS),pi-f4)
		OSC=$(CLK_PI_F4)
	else
		OSC=$(CLK_DEF)
	endif
endif

# Build object lists
CXXSRC=$(wildcard $(POCSAG_HS_PATH)/*.cpp)
CSRC_STD_F1=$(wildcard $(STD_LIB_F1)/*.c)
SYS_F1=$(wildcard $(SYS_DIR_F1)/*.c)
STARTUP_F1=$(wildcard $(STARTUP_DIR_F1)/*.c)
CXX_USB_F1=$(wildcard $(USB_F1)/*.cpp)
C_USB_F1=$(wildcard $(USB_F1)/*.c)
OBJ_F1=$(CXXSRC:$(POCSAG_HS_PATH)/%.cpp=$(OBJDIR_F1)/%.o) $(CSRC_STD_F1:$(STD_LIB_F1)/%.c=$(OBJDIR_F1)/%.o) $(SYS_F1:$(SYS_DIR_F1)/%.c=$(OBJDIR_F1)/%.o) $(STARTUP_F1:$(STARTUP_DIR_F1)/%.c=$(OBJDIR_F1)/%.o) 
OBJ_F1BL=$(CXXSRC:$(POCSAG_HS_PATH)/%.cpp=$(OBJDIR_F1)/%.o) $(CSRC_STD_F1:$(STD_LIB_F1)/%.c=$(OBJDIR_F1)/%.o) $(SYS_F1:$(SYS_DIR_F1)/%.c=$(OBJDIR_F1)/%.o) $(STARTUP_F1:$(STARTUP_DIR_F1)/%.c=$(OBJDIR_F1)/%.o) $(CXX_USB_F1:$(USB_F1)/%.cpp=$(OBJDIR_F1)/%.o) $(C_USB_F1:$(USB_F1)/%.c=$(OBJDIR_F1)/%.o)

# MCU flags
MCFLAGS_F1=-mcpu=cortex-m3 -march=armv7-m -mthumb -Wall -Wextra

# Compile flags
DEFS_F1_HS=-DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -DHSE_VALUE=$(OSC) -DVECT_TAB_OFFSET=0x0 -DMADEBYMAKEFILE
DEFS_F1_HS_BL=-DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD -DHSE_VALUE=$(OSC) -DVECT_TAB_OFFSET=0x2000 -DMADEBYMAKEFILE

# Build compiler flags
CFLAGS_F1=-c $(MCFLAGS_F1) $(INCLUDES_F1)
CXXFLAGS_F1=-c $(MCFLAGS_F1) $(INCLUDES_F1)

# Linker flags
LDFLAGS_F1_N =-T normal.ld $(MCFLAGS_F1) $(INCLUDES_LIBS_F1)
LDFLAGS_F1_BL =-T bootloader.ld $(MCFLAGS_F1) $(INCLUDES_LIBS_F1)

# Common flags
CFLAGS=-Os -ffunction-sections -fdata-sections -nostdlib -DCUSTOM_NEW -DNO_EXCEPTIONS -Wno-unused-parameter -nostdlib
CXXFLAGS=-Os -fno-exceptions -ffunction-sections -fdata-sections -nostdlib -fno-rtti -DCUSTOM_NEW -DNO_EXCEPTIONS -Wno-unused-parameter
LDFLAGS=-Os --specs=nano.specs --specs=nosys.specs

# Build Rules
.PHONY: all release_f1 hs bl pi-f4 clean

all: hs

hs: CFLAGS+=$(CFLAGS_F1) $(DEFS_F1_HS)
hs: CXXFLAGS+=$(CXXFLAGS_F1) $(DEFS_F1_HS)
hs: LDFLAGS+=$(LDFLAGS_F1_N)
hs: release_f1

bl: CFLAGS+=$(CFLAGS_F1) $(DEFS_F1_HS_BL)
bl: CXXFLAGS+=$(CXXFLAGS_F1) $(DEFS_F1_HS_BL)
bl: LDFLAGS+=$(LDFLAGS_F1_BL)
bl: release_f1bl

release_f1: GitVersion.h
release_f1: $(BINDIR)
release_f1: $(OBJDIR_F1)
release_f1: $(BINDIR)/$(BINHEX_F1)
release_f1: $(BINDIR)/$(BINBIN_F1)

release_f1bl: GitVersion.h
release_f1bl: $(BINDIR)
release_f1bl: $(OBJDIR_F1)
release_f1bl: $(BINDIR)/$(BINHEX_F1BL)
release_f1bl: $(BINDIR)/$(BINBIN_F1BL)

$(BINDIR):
	$(MDDIRS)

$(OBJDIR_F1):
	$(MDDIRS)

$(BINDIR)/$(BINHEX_F1BL): $(BINDIR)/$(BINELF_F1BL)
	$(CP) -O ihex $< $@
	@echo "Objcopy from ELF to IHEX complete!\n"

$(BINDIR)/$(BINBIN_F1BL): $(BINDIR)/$(BINELF_F1BL)
	$(CP) -O binary $< $@
	@echo "Objcopy from ELF to BINARY complete!\n"

$(BINDIR)/$(BINELF_F1BL): $(OBJ_F1BL)
	$(CXX) $(OBJ_F1BL) $(LDFLAGS) -o $@
	@echo "Linking complete!\n"
	$(SIZE) $(BINDIR)/$(BINELF_F1BL)

$(BINDIR)/$(BINHEX_F1): $(BINDIR)/$(BINELF_F1)
	$(CP) -O ihex $< $@
	@echo "Objcopy from ELF to IHEX complete!\n"

$(BINDIR)/$(BINBIN_F1): $(BINDIR)/$(BINELF_F1)
	$(CP) -O binary $< $@
	@echo "Objcopy from ELF to BINARY complete!\n"

$(BINDIR)/$(BINELF_F1): $(OBJ_F1)
	$(CXX) $(OBJ_F1) $(LDFLAGS) -o $@
	@echo "Linking complete!\n"
	$(SIZE) $(BINDIR)/$(BINELF_F1)

$(OBJDIR_F1)/%.o: $(POCSAG_HS_PATH)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F1)/%.o: $(STD_LIB_F1)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F1)/%.o: $(SYS_DIR_F1)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F1)/%.o: $(STARTUP_DIR_F1)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F1)/%.o: $(USB_F1)/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

$(OBJDIR_F1)/%.o: $(USB_F1)/%.c
	$(CC) $(CFLAGS) $< -o $@
	@echo "Compiled "$<"!\n"

clean:
	$(CLEANCMD)
	$(RM) GitVersion.h
	
stlink:
	$(ST_FLASH) write bin/$(BINBIN_F1) 0x8000000

stlink-bl:
	$(ST_FLASH) write $(F1_LIB_PATH)/utils/bootloader/generic_boot20_pc13.bin 0x8000000
	$(ST_FLASH) write bin/$(BINBIN_F1BL) 0x8002000
	
serial:
	$(STM32FLASH) -v -w bin/$(BINBIN_F1) -g 0x0 $(devser)

serial-bl:
	$(STM32FLASH) -v -w $(F1_LIB_PATH)/utils/bootloader/generic_boot20_pc13.bin -g 0x0 $(devser)
	$(STM32FLASH) -v -w bin/$(BINBIN_F1BL) -g 0x0 -S 0x08002000 $(devser)

nano-hotspot:
ifneq ($(wildcard /usr/local/bin/stm32flash),)
	/usr/local/bin/stm32flash -v -w bin/$(BINBIN_F1) -g 0x0 -R -i 200,-3,3:-200,3 /dev/ttyAMA0
endif

ifneq ($(wildcard /usr/bin/stm32flash),)
	/usr/bin/stm32flash -v -w bin/$(BINBIN_F1) -g 0x0 -R -i 200,-3,3:-200,3 /dev/ttyAMA0
endif

nano-dv:
ifneq ($(wildcard /usr/local/bin/stm32flash),)
	/usr/local/bin/stm32flash -v -w bin/$(BINBIN_F1) -g 0x0 -R -i 67,-66,66:-67,66 /dev/ttyAMA0
endif

ifneq ($(wildcard /usr/bin/stm32flash),)
	/usr/bin/stm32flash -v -w bin/$(BINBIN_F1) -g 0x0 -R -i 67,-66,66:-67,66 /dev/ttyAMA0
endif

zumspot-pi:
ifneq ($(wildcard /usr/local/bin/stm32flash),)
	/usr/local/bin/stm32flash -v -w bin/$(BINBIN_F1) -g 0x0 -R -i 20,-21,21:-20,21 /dev/ttyAMA0
endif

ifneq ($(wildcard /usr/bin/stm32flash),)
	/usr/bin/stm32flash -v -w bin/$(BINBIN_F1) -g 0x0 -R -i 20,-21,21:-20,21 /dev/ttyAMA0
endif

mmdvm_hs_hat: zumspot-pi

mmdvm_hs_dual_hat: zumspot-pi

dfu:
ifdef devser
	$(DFU_RST) $(devser) 750
endif
	$(DFU_UTIL) -D bin/$(BINBIN_F1BL) -d 1eaf:0003 -a 2 -R -R
	
ocd:
ifneq ($(wildcard /usr/bin/openocd),)
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "program bin/$(BINELF_F1) verify reset exit"
endif

ifneq ($(wildcard /usr/local/bin/openocd),)
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/local/share/openocd/scripts/target/stm32f1x.cfg -c "program bin/$(BINELF_F1) verify reset exit"
endif

ifneq ($(wildcard /opt/openocd/bin/openocd),)
	/opt/openocd/bin/openocd -f /opt/openocd/scripts/interface/stlink-v2-1.cfg -f /opt/openocd/share/openocd/scripts/target/stm32f1x.cfg -c "program bin/$(BINELF_F1) verify reset exit"
endif
  
ocd-bl:
ifneq ($(wildcard /usr/bin/openocd),)
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "program $(F1_LIB_PATH)/utils/bootloader/generic_boot20_pc13.bin verify reset exit 0x08000000"
	/usr/bin/openocd -f /usr/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "program bin/$(BINBIN_F1BL) verify reset exit 0x08002000"
endif

ifneq ($(wildcard /usr/local/bin/openocd),)
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/local/share/openocd/scripts/target/stm32f1x.cfg -c "program $(F1_LIB_PATH)/utils/bootloader/generic_boot20_pc13.bin verify reset exit 0x08000000"
	/usr/local/bin/openocd -f /usr/local/share/openocd/scripts/interface/stlink-v2-1.cfg -f /usr/local/share/openocd/scripts/target/stm32f1x.cfg -c "program bin/$(BINBIN_F1BL) verify reset exit 0x08002000"
endif

ifneq ($(wildcard /opt/openocd/bin/openocd),)
	/opt/openocd/bin/openocd -f /opt/openocd/scripts/interface/stlink-v2-1.cfg -f /opt/openocd/share/openocd/scripts/target/stm32f1x.cfg -c "program $(F1_LIB_PATH)/utils/bootloader/generic_boot20_pc13.bin verify reset exit 0x08000000"
	/opt/openocd/bin/openocd -f /opt/openocd/scripts/interface/stlink-v2-1.cfg -f /opt/openocd/share/openocd/scripts/target/stm32f1x.cfg -c "program bin/$(BINBIN_F1BL) verify reset exit 0x08002000"
endif

# Export the current git version if the index file exists, else 000...
GitVersion.h: .FORCE
ifdef SYSTEMROOT
	echo #define GITVERSION "0000000" > $@
else ifdef SystemRoot
	echo #define GITVERSION "0000000" > $@
else
ifneq ("$(wildcard .git/index)","")
	echo "#define GITVERSION \"$(shell git rev-parse --short HEAD)\"" > $@
else
	echo "#define GITVERSION \"0000000\"" > $@
endif
endif

.FORCE:

