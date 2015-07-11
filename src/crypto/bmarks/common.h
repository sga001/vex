#ifndef VEX_COMMON_UTILS_H
#define VEX_COMMON_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <openssl/evp.h>

//#define HASH SHA256
#define HASH SHA
#define HASH_SIZE SHA_DIGEST_LENGTH
//#define HASH_SIZE SHA256_DIGEST_LENGTH
#define SEED_SIZE HASH_SIZE
#define KEY_SIZE 128 // in bytes
#define MAX_BID 10000 // $100.00

#define kSecToNanosec 1e9
#define kSecToMicrosec 1e6
#define kSecToMillisec 1e3
#define kMillisecToNanosec 1e6
#define kMillisecToMicrosec 1e3
#define kMillisecToSec 1e-3
#define kMicrosecToNanosec 1e3
#define kMicrosecToMillisec 1e-3
#define kMicrosecToSec 1e-6


#ifdef __cplusplus
}
#endif


#endif 
