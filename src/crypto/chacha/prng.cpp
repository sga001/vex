#include "chacha/prng.h"
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <string.h>


void* aligned_malloc(size_t size)
{
  void* ptr = malloc(size + PAGESIZE);

  if (ptr)
  {
    void* aligned = (void*)(((long)ptr + PAGESIZE) & ~(PAGESIZE - 1));
    ((void**)aligned)[-1] = ptr;
    return aligned;
  }
  else
    return NULL;
}


Prng::Prng(int type)
{
  init_chacha_png();
}

Prng::Prng(int type, uint8_t *key, uint8_t *iv)
{
  init_chacha_png(key, iv);
}

int Prng::get_type()
{
  return type;
}

void Prng::get_randomb(char *buf, int nbits) {
  chacha_urandomb(buf, nbits);
}

void Prng::init_chacha_png() {
  chacha = (ECRYPT_ctx*)aligned_malloc(sizeof(ECRYPT_ctx));
  random_state = new u8[RANDOM_STATE_SIZE];

  u8 key[256];
  u8 iv[64];

  std::ifstream rand;

  rand.open("/dev/urandom", std::ifstream::in);
  rand.read((char*)&key, (size_t)(256/8));
  rand.read((char*)&iv, (size_t)(64/8));
  rand.close();

  ECRYPT_keysetup(chacha, key, 256, 64);
  ECRYPT_ivsetup(chacha, iv);

  chacha_refill_randomness();

  return;
}

void Prng::init_chacha_png(uint8_t *key, uint8_t *iv) {
  chacha = (ECRYPT_ctx*)aligned_malloc(sizeof(ECRYPT_ctx));
  random_state = new u8[RANDOM_STATE_SIZE];

  ECRYPT_keysetup(chacha, key, 256, 64);
  ECRYPT_ivsetup(chacha, iv);

  chacha_refill_randomness();

  return;
}


// generates random bits using one big call to chacha and keeping state
void Prng::chacha_urandomb(char *buf, int nbits) {
  if (nbits == 0)
    return;

  // determine number of bytes
  int nbytes = ceil(double(nbits)/8.0);
  int diff = (nbytes <<3) - nbits;

  // check that we have enough randomness
  if ((RANDOM_STATE_SIZE-random_index) < nbytes)
    chacha_refill_randomness();

  if (diff == 0) {
    memcpy(buf, &random_state[random_index], nbytes);
  } else {
    memcpy(buf, &random_state[random_index], nbytes - 1);
    char byte = random_state[random_index + nbytes - 1];
    byte &= 0xFF >> (8 - diff);
    byte |= buf[nbytes - 1] & (0xFF << diff);
    buf[nbytes - 1] = byte;
  }
 
   // update index
  random_index += nbytes;
}

// generate new random state
void Prng::chacha_refill_randomness() {
  ECRYPT_keystream_bytes(chacha, random_state, RANDOM_STATE_SIZE);
  random_index = 0;
}
