#ifndef _VEX_CRYPTO_H
#define _VEX_CRYPTO_H

#include "chacha/prng.h"
#include "sha256/sha256.h"

#define PREFIX_SIZE 32 // bytes
#define HASH_SIZE SHA256_DIGEST_SIZE
#define SEED_SIZE PREFIX_SIZE + HASH_SIZE
#define SALT_SIZE HASH_SIZE
#define HASH SHA256_hash      // HASH(x, size, h(x))

namespace vex {

/**
* Generates length random bits
*/

int random_bits(int length, uint8_t *buf);

/**
* setup takes in a num of bytes n, a well-known prefix, 
* and two buffers sprime and s.
* It fill sprime with n+PREFIX_SIZE bytes of random data,
* and s with the hash of sprime.
* Setup returns -1 on failure. Otherwise, it returns the length
* of the seed.
*/
int setup(int n, const uint8_t *prefix, uint8_t *sprime, uint8_t *s);

int gen_s(const uint8_t *sprime, size_t sp_length, uint8_t *s);

/**
* gen_commit generates a commitment to x, using the provided seed.
* It returns -1 on failure. Otherwise it returns the size of c.
*/
int gen_commit(uint32_t x, const uint8_t *s, size_t s_length, uint8_t *c);

/**
* gen_salted_commit generates a commitment to x, using the provided seed and salt.
* It returns -1 on failure. Otherwise it returns the size of c.
*/
int gen_salted_commit(uint32_t x, const uint8_t *s, size_t s_length, 
  const uint8_t *salt, size_t salt_length, uint8_t *c);

/**
* gen_proof generates a proof stating that a committed value x is greater
* than or equal to a query q.
* It returns -1 on failure. Otherwise it returns the size of p.
*/
int gen_proof(uint32_t q, uint32_t x, const uint8_t *s, size_t s_length, 
  uint8_t *p);

/**
* verify_proof takes in a commitment, a proof, and a query and verifies
* the correctness of the proof. It returns 1 if the proof is correct
* and 0 otherwise.
*/
int verify_proof(uint32_t q, const uint8_t *c, size_t c_length, 
  const uint8_t *p, size_t p_length);

/**
* verify_eq_proof takes in a commitment, a proof and a query and
*  verifies the correctness of the proof.
*/
int verify_eq_proof(uint32_t q, const uint8_t *c, size_t c_length,
  const uint8_t *p, size_t p_length, const uint8_t *prefix);


/**
* verify_salted_proof takes in a commitment, a proof, a salt and a query
*  and verifies the correctness of the proof.
*/

int verify_salted_proof(uint32_t q, const uint8_t *c, size_t c_length,
  const uint8_t *p, size_t p_length, const uint8_t *s, size_t s_length);

/**
* verify_eq_salted_proof takes in a commitment, a proof, a salt, a prefix and a query
*  and verifies the correctness of the proof.
*/

int verify_eq_salted_proof(uint32_t q, const uint8_t *c, size_t c_length,
  const uint8_t *p, size_t p_length, const uint8_t *s, size_t s_length, 
  const uint8_t *prefix);


} // namespace vex


#endif
