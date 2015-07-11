# Where to find it
SFSDIR		:= ../deps/lib/sfslite
SFSLIB  	:= $(SFSDIR)/libasync.a $(SFSDIR)/libsfscrypt.a
PAPILIB 	:= /usr/lib/x86_64-linux-gnu/libpapi.a
PCRELIB		:= /usr/lib/x86_64-linux-gnu/libpcre.a
PFMLIB		:= /usr/lib/x86_64-linux-gnu/libpfm.a
DEPLIBS		:= -L../deps/lib/
CHACHAINC	:= crypto/chacha
SHAINC		:= crypto/sha256
VEXINC		:= crypto/vex
VCRYPTO		:= -Icrypto -I$(CHACHAINC) -I$(SHAINC) -I$(VEXINC)
EXTRA			:= #-I/usr/include/x86_64-linux-gnu/c++/4.7/

# How to compile
CC	:= gcc
CXX := g++
YASM	:= yasm -f x64 -f elf64 -X gnu dwarf2 -D LINUX
CFLAGS  := -O2 $(EXTRA) $(VCRYPTO) -I.. -I../deps/include/ -I/usr/local -I. -I/usr/include -Wall -Wno-unused -Wno-deprecated
CXXFLAGS := $(CFLAGS) -std=c++11
LDFLAGS := $(DEPLIBS) -Wl,-Bstatic $(SFSLIB) $(PCRELIB) -lgmp -lresolv -lmsgpack  -Wl,-Bdynamic -lpapi -lpfm
GEN_OPT := -O2 -msse4.2 -mavx
VEC_FLAGS := -ftree-vectorize
SHA_FLAGS := $(GEN_OPT) $(VEC_FLAGS) $(CFLAGS) -std=gnu99

# OS-dependent flags
OS			:= $(shell uname -s | awk '{print tolower($$0)}')

ifneq ($(OS),darwin)
	LDFLAGS	:= -Wl,--no-as-needed ${LDFLAGS}
endif


# What to compile

BIN	 := seller_main adx_main bidder_main vex_adx_main vex_bidder_main \
				vex_seller_main vex_adx_nocc_main vex_adx_cp_main vex_bidder_cp_main
TPUT := throughput_seller throughput_bidder throughput_adx \
				throughput_vex_bidder throughput_vex_seller throughput_vex_adx \
				throughput_vex_adx_nocc throughput_vex_bidder_cp throughput_vex_adx_cp\
				throughput_vex_prover throughput_vex_prover_nocc throughput_vex_verifier\
				throughput_vex_prover_cp

SRCS	:= $(wildcard *.cpp) $(wildcard *.c)

all: ${BIN} ${TPUT}


.PHONY: clean


# Crypto Targets

CHACHA_ASM = $(CHACHAINC)/chacha.s
CHACHA_OBJS = $(CHACHAINC)/chacha.o
CHACHA_HDRS = $(wildcard $(CHACHAINC)/*.h)
CHACHA_C		= $(CHACHAINC)/prng.cpp
CHACHA_C_OBJS = $(CHACHA_C:.cpp=.o)

SHA256_SRCS = $(SHAINC)/sha256.c
SHA256_HDRS = $(SHAINC)/sha256.h
SHA256_OBJS = $(SHA256_SRCS:.c=.o)
SHA256ASM_SRCS = $(SHAINC)/intel/sha256_avx.asm $(SHAINC)/intel/sha256_sse4.asm 
#							   $(SHAINC)/intel/sha256_avx2_rorx2.asm $(SHAINC)/intel/sha256_avx2_rorx8.asm
SHA256ASM_OBJS = $(SHA256ASM_SRCS:.asm=.o)

VEX_C			= $(VEXINC)/rp.cpp
VEX_OBJS  = $(VEX_C:.cpp=.o)
VEX_HDRS 	= $(VEXINC)/rp.h

CRYPTO   = $(CHACHA_OBJS) $(CHACHA_C_OBJS) $(VEX_OBJS) $(SHA256_OBJS) $(SHA256ASM_OBJS)
COMMON = net.h util.h message_types.h getopt_pp.h
VEX_COMMON = $(COMMON) vex_util.h vex_message_types.h

# Baseline
SELLERDEP		:= seller_main.o seller_server.o getopt_pp.o net.o
ADXDEP			:= adx_main.o adx_server.o net.o
BIDDERDEP		:= bidder_main.o bidder_server.o getopt_pp.o net.o

# Vex
VEXSELLERDEP		:= vex_seller_main.o vex_seller_server.o getopt_pp.o net.o $(CRYPTO) 
VEXADXDEP			 	:= vex_adx_main.o vex_adx_server.o net.o $(CRYPTO)
VEXBIDDERDEP	 	:= vex_bidder_main.o vex_bidder_server.o getopt_pp.o net.o $(CRYPTO)
VEXBIDDERCPDEP 	:= vex_bidder_cp_main.o vex_bidder_cp_server.o vex_bidder_server.o getopt_pp.o net.o $(CRYPTO)
VEXADXNOCCDEP 	:= vex_adx_main.o vex_adx_nocc_server.o net.o $(CRYPTO)
VEXADXCPDEP			:= vex_adx_cp_main.o vex_adx_cp_server.o vex_adx_server.o net.o getopt_pp.o $(CRYPTO)

# TPUT
VEXTPUTBID		:= throughput_vex_bidder_main.o throughput_vex_bidder.o \
								 vex_bidder_server_tput.o net.o getopt_pp.o $(CRYPTO)
VEXTPUTBIDCP	:= throughput_vex_bidder_cp_main.o throughput_vex_bidder_cp.o \
								 vex_bidder_cp_server.o vex_bidder_server_tput.o net.o getopt_pp.o $(CRYPTO)
VEXTPUTSEL		:= throughput_vex_seller_main.o throughput_vex_seller.o \
								 vex_seller_server.o net.o getopt_pp.o $(CRYPTO)
VEXTPUTADX		:= throughput_vex_adx_main.o throughput_vex_adx.o \
								 vex_adx_server.o net.o getopt_pp.o $(CRYPTO)
VEXTPUTADXN		:= throughput_vex_adx_main.o throughput_vex_adx.o \
								 vex_adx_nocc_server.o net.o getopt_pp.o $(CRYPTO)
VEXTPUTADXCP	:= throughput_vex_adx_cp_main.o throughput_vex_adx_cp.o \
								 vex_adx_cp_server.o vex_adx_server.o net.o getopt_pp.o $(CRYPTO)
VEXPROVER			:= throughput_vex_prover.o vex_prover.o getopt_pp.o $(CRYPTO)
VEXPROVERNOCC	:= throughput_vex_prover.o vex_prover_nocc.o getopt_pp.o $(CRYPTO)
VEXPROVERCP		:= throughput_vex_prover_cp.o vex_prover_cp.o getopt_pp.o $(CRYPTO)
VEXVERIFIER		:= throughput_vex_verifier.o vex_prover.o vex_verifier.o getopt_pp.o $(CRYPTO)

# Binaries

seller_main:	$(SELLERDEP) seller_server.h $(COMMON)
	$(CXX) -o $@ $(SELLERDEP) $(LDFLAGS)

adx_main:	$(ADXDEP) adx_server.h $(COMMON)
	$(CXX) -o $@ $(ADXDEP) $(LDFLAGS)

bidder_main: $(BIDDERDEP) bidder_server.h $(COMMON)
	$(CXX) -o $@ $(BIDDERDEP) $(LDFLAGS)

vex_seller_main: $(VEXSELLERDEP) vex_seller_server.h $(VEX_COMMON)
	$(CXX) -o $@ $(VEXSELLERDEP) $(LDFLAGS)

vex_bidder_main: $(VEXBIDDERDEP) vex_bidder_server.h $(VEX_COMMON)
	$(CXX) -o $@ $(VEXBIDDERDEP) $(LDFLAGS)

vex_bidder_cp_main: $(VEXBIDDERCPDEP) vex_bidder_cp_server.h $(VEX_COMMON)
	$(CXX) -o $@ $(VEXBIDDERCPDEP) $(LDFLAGS)

vex_adx_main: $(VEXADXDEP) vex_adx_server.h $(VEX_COMMON)
	$(CXX) -o $@ $(VEXADXDEP) $(LDFLAGS)

vex_adx_nocc_main: $(VEXADXNOCCDEP) vex_adx_server.h $(VEX_COMMON)
	$(CXX) -o $@ $(VEXADXNOCCDEP) $(LDFLAGS)

vex_adx_cp_main: $(VEXADXCPDEP) vex_adx_cp_server.h $(VEX_COMMON)
	$(CXX) -o $@ $(VEXADXCPDEP) $(LDFLAGS)

vex_adx_nocc_server.o: vex_adx_server.cpp vex_adx_server.h
	$(CXX) $(CXXFLAGS) -D NOCC -c vex_adx_server.cpp -o $@

vex_bidder_server_tput.o: vex_bidder_server.cpp vex_bidder_server.h
	$(CXX) $(CXXFLAGS) -D TPUT -c vex_bidder_server.cpp -o $@

vex_prover_nocc.o: vex_prover.cpp vex_prover.h
	$(CXX) $(CXXFLAGS) -D NOCC -c vex_prover.cpp -o $@

# TPUT Binaries
throughput_seller: throughput_seller.cpp getopt_pp.o seller_server.h throughput_util.h
	$(CXX) $(CXXFLAGS) -o $@ throughput_seller.cpp getopt_pp.o $(LDFLAGS) 

throughput_bidder: throughput_bidder.cpp getopt_pp.o bidder_server.h throughput_util.h
	$(CXX) $(CXXFLAGS) -o $@ throughput_bidder.cpp getopt_pp.o $(LDFLAGS)

throughput_adx: throughput_adx.cpp getopt_pp.o adx_server.h throughput_util.h
	$(CXX) $(CXXFLAGS) -o $@ throughput_adx.cpp getopt_pp.o $(LDFLAGS)

throughput_vex_bidder: $(VEXTPUTBID) throughput_util.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXTPUTBID) $(LDFLAGS)

throughput_vex_bidder_cp: $(VEXTPUTBIDCP) throughput_util.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXTPUTBIDCP) $(LDFLAGS)

throughput_vex_seller: $(VEXTPUTSEL) throughput_util.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXTPUTSEL) $(LDFLAGS)

throughput_vex_adx: $(VEXTPUTADX) throughput_util.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXTPUTADX) $(LDFLAGS)

throughput_vex_adx_cp: $(VEXTPUTADXCP) throughput_util.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXTPUTADXCP) $(LDFLAGS)

throughput_vex_adx_nocc: $(VEXTPUTADXN) throughput_util.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXTPUTADXN) $(LDFLAGS)

throughput_vex_prover: $(VEXPROVER) throughput_util.h vex_prover.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXPROVER) $(LDFLAGS)

throughput_vex_verifier: $(VEXVERIFIER) throughput_util.h vex_prover.h vex_verifier.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXVERIFIER) $(LDFLAGS)

throughput_vex_prover_nocc: $(VEXPROVERNOCC) throughput_util.h vex_prover.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXPROVERNOCC) $(LDFLAGS)

throughput_vex_prover_cp: $(VEXPROVERCP) throughput_util.h vex_prover_cp.h throughput_vex_generator.h
	$(CXX) $(CXXFLAGS) -o $@ $(VEXPROVERCP) $(LDFLAGS)



# Crypto rules
$(CHACHA_OBJS): $(CHACHA_ASM)
	$(CXX) $(GEN_OPT) $(VEC_FLAGS) $(CXXFLAGS) -c $(CHACHA_ASM) -o $@

$(CHACHA_C_OBJS): $(CHACHA_C)
	$(CXX) $(GEN_OPT) $(VEC_FLAGS) $(CXXFLAGS) -c $(CHACHA_C) -o $@

$(SHA256_OBJS): $(SHA256_SRCS) $(SHA256_HDRS)
	$(CC) $(SHA_FLAGS) -c $(@:.o=.c) -o $@

$(SHA256ASM_OBJS): $(SHA256ASM_SRCS)
	$(YASM) -o $@ $(@:.o=.asm)

$(VEX_OBJS): $(VEX_C)
	$(CXX) $(GEN_OPT) $(CXXFLAGS) -c $(VEX_C) -o $@


install:
	mkdir -p ../bin/
	cp ${BIN} ${TPUT} ../bin/ 

clean:
	rm -rf ${BIN}
	rm -rf ${TPUT}
	rm -rf ${ADXDEP}
	rm -rf ${BIDDERDEP}
	rm -rf ${SELLERDEP}
	rm -rf ${VEXADXDEP}
	rm -rf ${VEXSELLERDEP}
	rm -rf ${VEXBIDDERDEP}
	rm -rf ${VEXBIDDERCPDEP}
	rm -rf ${VEXADXNOCCDEP}
	rm -rf ${VEXADXCPDEP}
	rm -rf ${VEXTPUTSEL}
	rm -rf ${VEXTPUTBID}
	rm -rf ${VEXTPUTBIDCP}
	rm -rf ${VEXTPUTADX}
	rm -rf ${VEXTPUTADXCP}
	rm -rf ${VEXPROVER}
	rm -rf ${VEXPROVERCP}
	rm -rf ${VEXPROVERNOCC}
	rm -rf ${VEXVERIFIER}
	rm -rf ../bin/


copy:
	cp ${BIN} ${TPUT} ../../bin/

.depend: ${SRCS}
	rm -rf ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ >> ./.depend

ifneq ($(MAKECMDGOALS),clean)
	include .depend
endif
