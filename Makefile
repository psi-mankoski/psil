#
#    File:   Makefile
#    Author: ***PSI***
#    Date:   Mon Jul 19 20:29:04 1999
#
#    Description:
#       This is the GNU makefile for Psil.
#
#    Copyright (c) 1999-2019 Joseph J. Mankoski ***PSI***
#


# Define variables.


SRCDIR = .
OBJDIR = .
BINDIR = .

ifeq ($(PLAT),iPAQ)

CC = agcc
CXX = ag++
AR = aar
RANLIB = aranlib
LD = ald

#CFLAGS = -Wall -g -DDEBUG=1 -fPIC -fsigned-char -mcpu=strongarm -O3 -DiPAQ
CFLAGS = -Wall -DDEBUG=0 -fomit-frame-pointer -fPIC -fsigned-char -mcpu=strongarm -O3 -DiPAQ

else

BASE_CFLAGS = -Wall -g -DDEBUG=0
#BASE_CFLAGS = -Wall -g -DDEBUG=1 -DFLONUM_IS_DOUBLE
CFLAGS = $(BASE_CFLAGS) -O3

endif

CXXFLAGS = $(CFLAGS)

HEADERS = $(SRCDIR)/Psil.h \
	  $(SRCDIR)/PsilTypes.h

SOURCES = $(SRCDIR)/Environment.cpp \
	  $(SRCDIR)/Error.cpp \
	  $(SRCDIR)/Evaluator.cpp \
	  $(SRCDIR)/Primitives.cpp \
	  $(SRCDIR)/Printer.cpp \
	  $(SRCDIR)/Psil.cpp \
	  $(SRCDIR)/Reader.cpp \
	  $(SRCDIR)/Stack.cpp

OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

PROGRAMS = psil


# Define targets.


all:	$(PROGRAMS)

gdb:
	$(MAKE) "CFLAGS=$(BASE_CFLAGS)"

$(PROGRAMS):	$(OBJECTS)
	$(LINK.cc) -o $@ $^

$(OBJECTS):	$(HEADERS)

clean:
	$(RM) $(OBJECTS)

cleanest:	clean
	$(RM) $(PROGRAMS)

# (Only needed for old GNU "make" versions.)
%.o:	%.cpp
	$(COMPILE.cc) -o $@ $<
