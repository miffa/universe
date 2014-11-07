# xmlcpp PullParser library.
#
# Copyright Wongoo Lee 2006. Use, modification and distribution is
# subject to the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

VERSION      = `cat ../VERSION`

MAKE         = make
CPP          = g++

INCLUDEDIR   = -I..
LIBDIR       = -L..

LFLAGS       = -lxmlcpp $(LIBDIR)
CFLAGS       = -Wall -DVERSION=$(VERSION) $(VERBOSE) $(DEBUG) $(INCLUDEDIR)

OBJS         = Tutorial2.o Tutorial1.o XMLPrinter.o

.PHONY: all clean

.cpp.o:
	$(CPP) $(CFLAGS) -o $@ -c $<

all: tutorial2 tutorial1 xmlprinter

tutorial2: ../libxmlcpp.a Tutorial2.o
	$(CPP) -o $@ Tutorial2.o $(LFLAGS)

tutorial1: ../libxmlcpp.a Tutorial1.o
	$(CPP) -o $@ Tutorial1.o $(LFLAGS)

xmlprinter: ../libxmlcpp.a XMLPrinter.o
	$(CPP) -o $@ XMLPrinter.o $(LFLAGS)

clean:
	@rm -f $(OBJS) tutorial2 tutorial1 xmlprinter 
	@rm -f __test*.xml

