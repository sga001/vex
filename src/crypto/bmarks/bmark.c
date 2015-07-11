#include "sha256/sha256.h"
#include "sha512/sha512_utils.h"
#include "stdio.h"
#include "common.h"
#include "skein/SHA3api_ref.h"
#include <papi.h>

#define ITERATIONS 1000
#define MAX_LENGTH 5000
#define GET_TIME(cycles, usec) cycles = PAPI_get_virt_cyc(); \
                                 usec = PAPI_get_virt_usec();

void print_hex(uint8_t* in, int len) {
  int i;
  for (i = 0; i < len; ++i)
    printf("%02X", in[i]);
  printf("\n");
}

void
test0()
{
  int i, j;
	uchar_t digest[32];
	char* buffer = "1234567890123456789012345678901";
	uint64_t bytes = 31;

  SHA256_hash((uint8_t*) buffer, bytes, digest);
	
  for (i = 0; i < ITERATIONS; i++) {    
    SHA256_hash((uint8_t*) buffer, bytes, digest);
    
    for (j=0; j < MAX_LENGTH -1; j++) {
      SHA256_hash(digest, 32, digest);
    }
  }
}


void
test1()
{
  int i, j;
	uchar_t digest[64];
//	uchar_t digest2[64];
	char* buffer = "1234567890123456789012345678901";
	uint64_t bytes = 32;

	for (i = 0; i < ITERATIONS; i++) {    
    opt_SHA512(digest, (uchar_t*) buffer, bytes);
    
    for (j=0; j < MAX_LENGTH -1; j++) {
      opt_SHA512(digest, digest, 64);
//      memcpy(digest, digest2, 64);
    }
  }
  
 // print_hex(digest, 64);
}


void
test2()
{
  int i, j;
  int bitlen = 256;
  int dlen = 32;
  uint8_t digest[32];
  char* data = "1234567890123456789012345678901";  
  
  for (i = 0; i < ITERATIONS; i++) {  
    Hash(bitlen, (uint8_t*) data, dlen, digest);  
    
    for (j = 0; j < MAX_LENGTH -1; j++)
      Hash(bitlen, digest, 32, digest);
  }

//  print_hex(digest, 32);
}

int main(int argc, char* argv[])
{
  long_long start_cycles, end_cycles, start_usec, end_usec;

  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    exit(1);

  printf("\nSHA256:\n");
  GET_TIME(start_cycles, start_usec);
  
  test0();
 
  GET_TIME(end_cycles, end_usec);

  printf("Virtual clock cycles: %lld (%lld cycles per hash)\n", 
    (end_cycles - start_cycles)/ITERATIONS, (end_cycles - start_cycles) / (ITERATIONS * MAX_LENGTH));
  printf("Virtual clock time: %f (ms)\n", kMicrosecToMillisec * (end_usec - start_usec) / ITERATIONS);

  printf("\nSHA512:\n");
  GET_TIME(start_cycles, start_usec);

  test1();

  GET_TIME(end_cycles, end_usec);

  printf("Virtual clock cycles: %lld (%lld cycles per hash)\n", 
    (end_cycles - start_cycles)/ITERATIONS, (end_cycles - start_cycles) / (ITERATIONS * MAX_LENGTH));
  printf("Virtual clock time: %f (ms)\n", kMicrosecToMillisec * (end_usec - start_usec) / ITERATIONS);

  printf("\nSKEIN:\n");
  GET_TIME(start_cycles, start_usec);
  
  test2();
  
  GET_TIME(end_cycles, end_usec);

  printf("Virtual clock cycles: %lld (%lld cycles per hash)\n", 
    (end_cycles - start_cycles)/ITERATIONS, (end_cycles - start_cycles) / (ITERATIONS * MAX_LENGTH));
  printf("Virtual clock time: %f (ms)\n", kMicrosecToMillisec * (end_usec - start_usec) / ITERATIONS);
  printf("\n");
 
  return 0;
}
