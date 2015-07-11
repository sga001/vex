YASM 				= yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX
CC					= gcc #icc
CXX					= g++ #icpc
LINK				= g++ #icpc
EXTRA				= #-I/usr/include/x86_64-linux-gnu/c++/4.7/

CFLAGS 			=$(EXTRA) -I. -I.. -I../chacha -I../sha256 -pedantic -Wall -fno-strict-aliasing \
						-Wno-unused-but-set-variable -Wno-enum-compare -c
LDFLAGS 		=  #/usr/local/lib/libpapi.a  


GEN_OPT			= -O3 -msse4.2
VEC_FLAGS		= -ftree-vectorize
LOOP_OPTFLAGS = 
SHA_FLAGS = $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS) -std=gnu99

CHACHA_ASM  = ../chacha/chacha.s
CHACHA_OBJS  = ../chacha/chacha.o
CHACHA_HDRS	 = $(wilcard ../chacha/*.h)
CHACHA_C    = ../chacha/prng.cpp
CHACHA_C_OBJS = $(CHACHA_C:.cpp=.o)

SHA256_SRCS    = ../sha256/sha256.c                                           
SHA256_HDRS    = ../sha256/sha256.h                                           
SHA256_OBJS    = $(SHA256_SRCS:.c=.o)                                      
SHA256ASM_SRCS = ../sha256/intel/sha256_avx.asm ../sha256/intel/sha256_sse4.asm  
SHA256ASM_OBJS = $(SHA256ASM_SRCS:.asm=.o)

VEX_C				= rp.cpp
VEX_OBJS 		= $(VEX_C:.cpp=.o)
VEX_HDRS		= rp.h

MAIN_SRCS		= rp_test.cpp
MAIN_HDRS		=
MAIN_OBJS		= $(MAIN_SRCS:.cpp=.o)




PROG				= rp_test

all:	$(PROG)

OBJS				= $(MAIN_OBJS) $(CHACHA_OBJS) $(CHACHA_C_OBJS) $(VEX_OBJS)\
							$(SHA256_OBJS) $(SHA256ASM_OBJS)

$(CHACHA_OBJS): $(CHACHA_ASM)
	$(CXX) $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS) -std=c++0x $(CHACHA_ASM) -o $@

$(CHACHA_C_OBJS): $(CHACHA_C)
	$(CXX) $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS) -std=c++0x $(CHACHA_C) -o $@

$(SHA256_OBJS): $(SHA256_SRCS) $(SHA256_HDRS)                              
	$(CC) $(SHA_FLAGS) $(@:.o=.c) -o $@                                      
                                                                           
$(SHA256ASM_OBJS): $(SHA256ASM_SRCS)                                       
	$(YASM) -o $@ $(@:.o=.asm)     

$(VEX_OBJS): $(VEX_C)
	$(CXX) $(GEN_OPT) $(VEX_FLAGS) $(CFLAGS) -std=c++0x $(VEX_C) -o $@

$(MAIN_OBJS): $(MAIN_SRCS)
	$(CXX) $(GEN_OPT) $(LOOP_OPTFLAGS) $(CFLAGS) -std=c++0x $(@:.o=.cpp) -o $@

$(PROG): $(OBJS)
	$(LINK) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(PROG)
