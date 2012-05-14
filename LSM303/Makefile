# Hey Emacs, this is a -*- makefile -*-
#
# $Id: Makefile,v 1.9 2012/05/14 08:00:31 mathes Exp $
#

# AVR-GCC Makefile template, derived from the WinAVR template (which
# is public domain), believed to be neutral to any flavor of "make"
# (GNU make, BSD make, SysV make)


#MCU = atmega2561
#MCU = atmega168
#MCU = atmega88
MCU = atmega8
FORMAT = ihex
TARGET = lsm303read
HDRS = vector.h
SRCS = $(TARGET).c uart.c i2cmaster.c LSM303DLH.c num2uart.c
CXXSRCS = 
ASRCS = 
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
CDEFS = -DF_CPU=12000000UL -DUART_DEBUG -DLED_DEBUG

# Place -I options here
CINCS = -I.


CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)
CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CEXTRA)

#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 


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

#LDMAP = $(LDFLAGS) -Wl,-Map=$(TARGET).map,--cref
LDFLAGS = $(EXTMEMOPTS) $(LDMAP) $(PRINTF_LIB) $(SCANF_LIB) $(MATH_LIB)


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
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
#OBJS = $(SRCS:.c=.o) $(CXXSRCS:.cpp=.o) $(ASRCS:.S=.o)
OBJS = $(TARGET).o uart.o i2cmaster.o LSM303DLH.o num2uart.o

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

SRCS += uart.c

uart.c: $(UARTLIBDIR)/uart.c
	ln -s $< $@
clean::
	rm -f uart.c

# I2CMASTER library of P.Fleury
I2CLIBDIR = $(FLEURYHOME)/i2cmaster
CINCS += -I$(I2CLIBDIR)
I2CLIB = i2cmaster.o

SRCS += i2cmaster.c

i2cmaster.c: $(I2CLIBDIR)/twimaster.c
	ln -s $< $@
clean::
	rm -f i2cmaster.c

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


# Target: lsm303test.elf
LSM303TESTOBJS = lsm303test.o $(UARTLIB) $(I2CLIB) LSM303DLH.o num2uart.o

lsm303test.elf: $(LSM303TESTOBJS)
	$(CC) $(ALL_CFLAGS) $(LSM303TESTOBJS) --output $@ \
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


