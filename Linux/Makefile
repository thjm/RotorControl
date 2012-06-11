#
# Makefile for AVR/Projekte/RotorControl/Linux sub-directory
#
# $Id: Makefile,v 1.2 2012/06/11 14:38:03 mathes Exp $
#
#

# define if we went to crate some helpful histograms
DoHistograms 	= 1

CC 		= gcc
CFLAGS  	= -g -Wall -std=c99

CXX             = g++
CXXFLAGS        = -g -Wall

LD              = g++
LDFLAGS 	= -g

AR 		= ar -r
RANLIB  	= ranlib

REMOVE  	= rm -f

INCLUDES 	= -I.

ifeq ($(DoHistograms),1)
DEFINES += -DDO_HISTOGRAMS
CXXFLAGS += $(shell root-config --cflags)
LDFLAGS += $(shell root-config --libs)
endif

#
# libserial (0.5.2 with patches for gcc)
LIBSERIAL_INCDIR = /usr/local/include
LIBSERIAL_LIBDIR = /usr/local/lib
LIBSERIAL = -L$(LIBSERIAL_LIBDIR) -lserial

# ---

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# ---

HDRS =
SRCS =

all:: analyzedat compass1

# --- program to analyze recorded (minicom) files from compass device

ANALYZEDAT_OBJS = analyzedat.o

analyzedat: $(ANALYZEDAT_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(ANALYZEDAT_OBJS)

clean::
	$(REMOVE) analyzedat

SRCS += analyzedat.cc

# --- program which reads (& analyzes) data received via serial port

COMPASS1_OBJS = compass1.o

compass1: $(COMPASS1_OBJS)
	$(LD) $(LDFLAGS) -o $@ $(COMPASS1_OBJS) $(LIBSERIAL)

clean::
	$(REMOVE) compass1

SRCS += compass1.cc

# --- general clean target ---

clean::
	$(REMOVE) *.o

# ----- create the dependencies -----

depend: .depend

.depend: $(SRCS) $(HDRS)
	@rm -f .depend
	@touch .depend
	$(CC) -MM  $(CXXFLAGS) $(INCLUDES) $(HDRS) $(SRCS) > .depend

clean::
	@rm -f .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

# ----- end of Makefile -----
