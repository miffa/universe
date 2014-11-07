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

LFLAGS       = -lboost_unit_test_framework -lxmlcpp $(LIBDIR)
CFLAGS       = -Wall -DVERSION=$(VERSION) $(VERBOSE) $(DEBUG) $(INCLUDEDIR)

OBJS = \
TestAll.o TestCharDecoder.o TestPullParser.o \
TestCodeConv.o TestSerializer.o

.PHONY: all clean

.cpp.o:
	$(CPP) $(CFLAGS) -o $@ -c $<

all: test

test: ../libxmlcpp.a $(OBJS)
	$(CPP) -o $@ $(OBJS) $(LFLAGS)
	./test

clean:
	@rm -f $(OBJS) test
	@rm -f __test*.xml


