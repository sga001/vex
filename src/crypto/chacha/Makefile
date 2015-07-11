YASM 				= yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX
CC					= gcc-4.7
CXX					= g++-4.7
LINK				= g++-4.7

CFLAGS 			= -g -I. -pedantic -Wall -std=c++0x -fno-strict-aliasing \
						-Wno-unused-but-set-variable -Wno-enum-compare -c
LDFLAGS 		= #/usr/local/lib/libpapi.a 

GEN_OPT			= -O3 -msse4.2
VEC_FLAGS		= -ftree-vectorize
LOOP_OPTFLAGS = 

CHACHA_ASM  = chacha.s
CHACHA_OBJS  = chacha.o
CHACHA_HDRS	 = $(wilcard *.h)
CHACHA_C    = prng.cpp
CHACHA_C_OBJS = $(CHACHA_C:.cpp=.o)


MAIN_SRCS		= main.cpp
MAIN_HDRS		=
MAIN_OBJS		= $(MAIN_SRCS:.cpp=.o)

PROG				= main

all:	$(PROG)

OBJS				= $(MAIN_OBJS) $(CHACHA_OBJS) $(CHACHA_C_OBJS)

$(CHACHA_OBJS): $(CHACHA_ASM)
	$(CXX) $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS) $(CHACHA_ASM) -o $@

$(CHACHA_C_OBJS): $(CHACHA_C)
	$(CXX) $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS) $(CHACHA_C) -o $@

$(MAIN_OBJS): $(MAIN_SRCS)
	$(CXX) $(GEN_OPT) $(LOOP_OPTFLAGS) $(CFLAGS) $(@:.o=.cpp) -o $@

$(PROG): $(OBJS)
	$(LINK) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(PROG)
