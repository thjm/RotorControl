# Hey Emacs, this is a -*- makefile -*-
#

# AVR-GCC Makefile template, derived from the WinAVR template (which
# is public domain), believed to be neutral to any flavor of "make"
# (GNU make, BSD make, SysV make)

# in case of ATtiny without hardware TWI we need software TWI (in assembler)
UseATtiny	= 0

# debug via UART (used only by program lsm303test.c)
UseUARTDebug	= 0

# use NMEA like data format for the generated messages (lsm303read.c)
UseNMEAFormat	= 1

# do we want to use a boot loader? (for remote firmware updates)
UseBootloader	= 0

ifeq ($(UseATtiny),1)
MCU = attiny2313
else
MCU = atmega8
endif
FORMAT = ihex
TARGET = lsm303read
HDRS = vector.h
SRCS = $(TARGET).c LSM303DLH.c num2uart.c uart.c
ifneq ($(UseATtiny),1)
SRCS += twimaster.c
endif
CXXSRCS = 
ifeq ($(UseATtiny),1)
ASRCS = i2cmaster.S
else
ASRCS = 
endif
OPT = s
# for the ATmega256 the additional option '-mrelax' is recommended
#OPT += -mrelax

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Place -D or -U options here
#CDEFS = -DF_CPU=8000000UL -DLIB_POLOLU
# U.Radig, ATmega8 test board with 12MHz
#CDEFS = -DF_CPU=12000000UL
# ATmega with factory settings on bread board
#CDEFS = -DF_CPU=1000000UL
#CDEFS = -DF_CPU=2000000UL
CDEFS = -DF_CPU=4000000UL

ifeq ($(UseNMEAFormat),1)
CDEFS += -DNMEA_FORMAT
endif
ifeq ($(UseUARTDebug),1)
CDEFS += -DUART_DEBUG
endif

# Place -I options here
CINCS = -I.


CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)
CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CEXTRA)

#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 

# Address where the bootloader area starts (0x0000 if no bootloader)
ifeq ($(UseBootloader),1)
BOOTLOADERSTARTADR=0x0800
else
BOOTLOADERSTARTADR=0x0000
endif

#Additional libraries.

# Minimalistic printf version
PRINTF_LIB_MIN = -Wl,-u,vfprintf -lprintf_min

# Floating point printf version (requires MATH_LIB = -lm below)
PRINTF_LIB_FLOAT = -Wl,-u,vfprintf -lprintf_flt

PRINTF_LIB = 

# Minimalistic scanf version
SCANF_LIB_MIN = -Wl,-u,vfscanf -lscanf_min

# Floating point + %[ scanf version (requires MATH_LIB = -lm below)
SCANF_LIB_FLOAT = -Wl,-u,vfscanf -lscanf_flt

SCANF_LIB = 

MATH_LIB = -lm

# External memory options

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# used for variables (.data/.bss) and heap (malloc()).
#EXTMEMOPTS = -Wl,--section-start,.data=0x801100,--defsym=__heap_end=0x80ffff

# 64 KB of external RAM, starting after internal RAM (ATmega128!),
# only used for heap (malloc()).
#EXTMEMOPTS = -Wl,--defsym=__heap_start=0x801100,--defsym=__heap_end=0x80ffff

EXTMEMOPTS =

#LDMAP = -Wl,-Map=$(TARGET).map,--cref
LDBOOT = -Ttext=$(BOOTLOADERSTARTADR)
LDFLAGS = $(EXTMEMOPTS) $(LDMAP) $(LDBOOT) $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)


# Programming support using avrdude. Settings and variables.

AVRDUDE_PROGRAMMER = usbasp
AVRDUDE_PORT = usb

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
#AVRDUDE_WRITE_EEPROM = -U eeprom:w:$(TARGET).eep


# Uncomment the following if you want avrdude's erase cycle counter.
# Note that this counter needs to be initialized first using -Yn,
# see avrdude manual.
#AVRDUDE_ERASE_COUNTER = -y

# Uncomment the following if you do /not/ wish a verification to be
# performed after programming the device.
#AVRDUDE_NO_VERIFY = -V

# Increase verbosity level.  Please use this when submitting bug
# reports about avrdude. See <http://savannah.nongnu.org/projects/avrdude> 
# to submit bug reports.
#AVRDUDE_VERBOSE = -v -v

AVRDUDE_BASIC = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER)
AVRDUDE_FLAGS = $(AVRDUDE_BASIC) $(AVRDUDE_NO_VERIFY) $(AVRDUDE_VERBOSE) $(AVRDUDE_ERASE_COUNTER)


CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
#SIZE = avr-size --mcu=$(MCU) --format=avr
SIZE = avr-size --format=berkeley
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
#OBJS = $(SRCS:.c=.o) $(CXXSRCS:.cpp=.o) $(ASRCS:.S=.o)
OBJS = $(TARGET).o LSM303DLH.o num2uart.o uart.o

ifeq ($(UseATtiny),1)
OBJS += i2cmaster.o
else
OBJS += twimaster.o
endif

# Define all listing files.
LST = $(ASRCS:.S=.lst) $(SRCS:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: build

build: elf hex eep

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lss: $(TARGET).lss 
sym: $(TARGET).sym


# UART library of P.Fleury
UARTLIBDIR = $(FLEURYHOME)/uartlibrary
CINCS += -I$(UARTLIBDIR)
UARTLIB = uart.o

uart.c: $(UARTLIBDIR)/uart.c
	ln -s $< $@
clean::
	rm -f uart.c

# I2CMASTER library of P.Fleury
I2CLIBDIR = $(FLEURYHOME)/i2cmaster
CINCS += -I$(I2CLIBDIR)
I2CLIB = twimaster.o

#ifeq ($(UseATtiny),0)
twimaster.c: $(I2CLIBDIR)/twimaster.c
	ln -s $< $@

clean::
	rm -f twimaster.c
#endif

# Program the device.  
program: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH) $(AVRDUDE_WRITE_EEPROM)



# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 


.SUFFIXES: .elf .hex .eep .lss .sym .cpp

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@



# Link: create ELF output file from object files.
$(TARGET).elf: $(OBJS)
	$(CC) $(ALL_CFLAGS) $(OBJS) --output $@ $(LDFLAGS)
	$(SIZE) $@


# Target: avrledtest.elf
AVRLEDTESTOBJS = avrledtest.o

avrledtest.elf: $(AVRLEDTESTOBJS)
	$(CC) $(ALL_CFLAGS) $(AVRLEDTESTOBJS) $(LDFLAGS) --output $@ \
		-Wl,-Map=avrledtest.map,--cref
	$(SIZE) $@

clean::
	$(RM) avrledtest.elf avrledtest.hex avrledtest.map avrledtest.lss
	$(RM) $(AVRLEDTESTOBJS)

SRCS += avrledtest.c

# Target: lsm303test.elf
LSM303TESTOBJS = lsm303test.o $(UARTLIB) $(I2CLIB) LSM303DLH.o

ifeq ($(UseUARTDebug),1)
LSM303TESTOBJS = += num2uart.o
endif

lsm303test.elf: $(LSM303TESTOBJS)
	$(CC) $(ALL_CFLAGS) $(LSM303TESTOBJS) $(LDFLAGS) --output $@ \
		-Wl,-Map=lsm303test.map,--cref
	$(SIZE) $@

clean::
	$(RM) lsm303test.elf lsm303test.hex lsm303test.map lsm303test.lss
	$(RM) $(LSM303TESTOBJS)

HDRS += global.h LSM303DLH.h num2uart.h
SRCS += lsm303test.c LSM303DLH.c num2uart.c

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create object files from C++ source files.
.cpp.o:
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



# Target: clean project.
clean::
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lss \
	$(OBJS) $(LST) $(SRCS:.c=.s) $(SRCS:.c=.d)

.PHONY:	all build elf hex eep lss sym program coff extcoff clean depend

# ----- create the dependencies -----

depend: .depend

.depend: $(SRCS) $(ASRCS) $(HDRS)
	@rm -f .depend
	@touch .depend
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(CINCS) $(SRCS) $(ASRCS) > .depend

clean::
	@rm -f .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

# --- end of Makefile ---
