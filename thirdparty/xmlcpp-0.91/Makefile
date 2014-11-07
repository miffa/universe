# xmlcpp PullParser library.
#
# Copyright Wongoo Lee 2006. Use, modification and distribution is
# subject to the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

PROD         = xmlcpp
VERSION      = `cat VERSION`

VERBOSE      = #-v
DEBUG        = #-g

export VERBOSE
export DEBUG

MAKE         = make
CPP          = g++
TAR          = tar
ZIP          = gzip
AR           = ar -cr

INCLUDEDIR   = 

CFLAGS       = -Wall -DVERSION=$(VERSION) $(VERBOSE) $(DEBUG) $(INCLUDEDIR)

TESTDIR      = tests
SAMPLEDIR    = samples

OBJS         = \
Serializer.o \
PullParser.o PullParserError.o Token.o \
CharEncoder.o \
CharDecoder.o CharDecoderIterator.o CodeConv.o Exception.o

.PHONY: test sample all clean

.cpp.o:
	$(CPP) $(CFLAGS) -o $@ -c $<

libxmlcpp.a: $(OBJS)
	$(AR) $@ $(OBJS)

test:
	$(MAKE) -C $(TESTDIR) -e

sample:
	$(MAKE) -C $(SAMPLEDIR) -e

all: libxmlcpp.a test sample

dist:
	@rm -rf $(PROD)-$(VERSION)

	@mkdir $(PROD)-$(VERSION)
	@mkdir $(PROD)-$(VERSION)/$(TESTDIR)
	@mkdir $(PROD)-$(VERSION)/$(SAMPLEDIR)

	@cp *.cpp $(PROD)-$(VERSION)
	@cp *.h $(PROD)-$(VERSION)
	@cp README $(PROD)-$(VERSION)
	@cp VERSION $(PROD)-$(VERSION)
	@cp Doxyfile $(PROD)-$(VERSION)
	@cp CHANGES $(PROD)-$(VERSION)
	@cp LICENSE_1_0.txt $(PROD)-$(VERSION)
	@cp Makefile $(PROD)-$(VERSION)
	@cp $(TESTDIR)/*.cpp $(PROD)-$(VERSION)/$(TESTDIR)
	@cp $(TESTDIR)/*.xml $(PROD)-$(VERSION)/$(TESTDIR)
	@cp $(TESTDIR)/Makefile $(PROD)-$(VERSION)/$(TESTDIR)
	@cp $(SAMPLEDIR)/*.cpp $(PROD)-$(VERSION)/$(SAMPLEDIR)
	@cp $(SAMPLEDIR)/Makefile $(PROD)-$(VERSION)/$(SAMPLEDIR)

	@$(TAR) -cf $(PROD)-$(VERSION).tar $(PROD)-$(VERSION)
	@$(ZIP) $(PROD)-$(VERSION).tar
	@rm -rf $(PROD)-$(VERSION)

clean:
	@rm -f $(OBJS)
	@$(MAKE) -C $(TESTDIR) clean
	@$(MAKE) -C $(SAMPLEDIR) clean


