YASM 				= yasm -f x64 -f elf64 -X gnu -g dwarf2 -D LINUX
CC					= gcc-4.7
LINK				= g++-4.7

CFLAGS 			= -g -I. -I.. -I../sha512 -I../sha256 -I../skein -pedantic -Wall -std=gnu99 \
-fno-strict-aliasing -Wno-unused-but-set-variable -Wno-enum-compare -c
LDFLAGS 		= /usr/local/lib/libpapi.a 

GEN_OPT			= -O3 -msse4.2
VEC_FLAGS		= -ftree-vectorize
LOOP_OPTFLAGS = 
SHA_FLAGS 	= $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS)

CRYPTO_HDRS  = ../sha512/sha512_utils.h
CRYPTO_SRCS  = ../sha512/sha512_utils.c
CRYPTO_OBJS	 = $(CRYPTO_SRCS:.c=.o)

SHA256_SRCS    = ../sha256/sha256.c
SHA256_HDRS		 = ../sha256/sha256.h
SHA256_OBJS		 = $(SHA256_SRCS:.c=.o)
SHA256ASM_SRCS = ../sha256/intel/sha256_avx.asm ../sha256/intel/sha256_sse4.asm
SHA256ASM_OBJS = $(SHA256ASM_SRCS:.asm=.o) 

SHA512_SRCS    = ../sha512/sha512.c
SHA512_HDRS		 = ../sha512/sha512.h
SHA512_OBJS		 = $(SHA512_SRCS:.c=.o)
SHA512ASM_SRCS = ../sha512/intel/sha512_avx.asm ../sha512/intel/sha512_sse4.asm
SHA512ASM_OBJS = $(SHA512ASM_SRCS:.asm=.o) 

SKEIN_B_ASM  = ../skein/skein_block_x64.s
SKEIN_B_C		 = ../skein/skein_block.c
SKEIN_B_OBJS  = ../skein/skein_block.o
SKEIN_SRCS	 = ../skein/SHA3api_ref.c ../skein/skein.c
SKEIN_HDRS	 = ../skein/brg_endian.h ../skein/SHA3api_ref.h ../skein/skein.h \
							 ../skein/skein_port.h ../skein/brg_types.h ../skein/skein_iv.h
SKEIN_OBJS	= $(SKEIN_SRCS:.c=.o)

MAIN_SRCS		= bmark.c
MAIN_HDRS		=
MAIN_OBJS		= $(MAIN_SRCS:.c=.o)

PROG				= bmark

all:	$(PROG)


OBJS				= $(MAIN_OBJS) $(SHA256_OBJS) $(SHA256ASM_OBJS) $(SHA512_OBJS) \
							$(SHA512ASM_OBJS) $(CRYPTO_OBJS) $(SKEIN_B_OBJS) $(SKEIN_OBJS)

$(SHA512_OBJS): $(SHA512_SRCS) $(SHA512_HDRS)
	$(CC) $(SHA_FLAGS) $(@:.o=.c) -o $@

$(SHA512ASM_OBJS): $(SHA512ASM_SRCS)
	$(YASM) -o $@ $(@:.o=.asm)

$(SHA256_OBJS): $(SHA256_SRCS) $(SHA256_HDRS)
	$(CC) $(SHA_FLAGS) $(@:.o=.c) -o $@

$(SHA256ASM_OBJS): $(SHA256ASM_SRCS)
	$(YASM) -o $@ $(@:.o=.asm)

$(CRYPTO_OBJS): $(CRYPTO_SRCS) $(CRYPTO_HDRS)
	$(CC) $(GEN_OPT) $(CFLAGS) $(@:.o=.c) -o $@

$(SKEIN_B_OBJS): $(SKEIN_B_ASM)
	$(CC) $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS) $(SKEIN_B_ASM) -o $@

$(SKEIN_OBJS): $(SKEIN_SRCS) $(SKEIN_HDRS)
	$(CC) $(GEN_OPT) $(CFLAGS) $(@:.o=.c) -o $@

$(MAIN_OBJS): $(MAIN_SRCS)
	$(CC) $(GEN_OPT) $(LOOP_OPTFLAGS) $(CFLAGS) $(@:.o=.c) -o $@

$(PROG): $(OBJS)
	$(LINK) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(PROG)
