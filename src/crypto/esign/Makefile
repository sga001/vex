# Where to find it
TOPDIR := ../../..
SFSDIR := $(TOPDIR)/sfslite

SFSINC  := -I$(TOPDIR) -I$(SFSDIR) -I$(SFSDIR)/async -I$(SFSDIR)/arpc -I$(SFSDIR)/crypt
					  
SFSLIB  := $(SFSDIR)/async/.libs/libasync.a $(SFSDIR)/crypt/.libs/libsfscrypt.a
PAPILIB := /usr/local/lib/libpapi.a

# How to compile
CC	:= gcc-4.7
CXX := g++-4.7
CFLAGS  := $(SFSINC) -g -I.. -I/usr/local -I. -I/usr/include -Wall -Werror -Wno-unused
CXXFLAGS := $(CFLAGS) -std=c++0x
LDFLAGS := $(SFSLIB) $(PAPILIB) -lgmp -lpcre -lresolv -lmsgpack

# OS-dependent flags
OS			:= $(shell uname -s | awk '{print tolower($$0)}')

ifneq ($(OS),darwin)
	LDFLAGS	:= -Wl,--no-as-needed ${LDFLAGS}
endif

# What to compile

BIN		:= esign_test
SRCS	:= $(wildcard *.cpp) $(wildcard *.c)

all: ${BIN}

.PHONY: clean

# Targets
ESIGNDEPS := esign_test.o


esign_test:	$(ESIGNDEPS)
	$(CXX) -o $@ $^ $(LDFLAGS)

esign_test.o: esign_test.cpp vex_esign.h

clean:
	rm -rf ${BIN}
	rm -rf ${ESIGNDEPS}

.depend: ${SRCS}
	rm -rf ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ >> ./.depend

ifneq ($(MAKECMDGOALS),clean)
	include .depend
endif

