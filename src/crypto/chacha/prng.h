#ifndef _CRYPTO_PRNG_H_  
#define _CRYPTO_PRNG_H_  
#include <sys/types.h>
#include <stdint.h>

// chacha is a C library
extern "C" {
#include "ecrypt-sync.h"
}

#define PNG_CHACHA 3
#define PAGESIZE 0x2000

// must be factor of mp_bits_per_limb for split()
#define RANDOM_STATE_SIZE (3000*2/8)

class Prng {
  private:
    int type;
    ECRYPT_ctx *chacha; 
    uint8_t *random_state;
    int random_index;

  private:
    void init_mt_png();
    void init_chacha_png();
    void init_chacha_png(uint8_t *key, uint8_t *iv);
    void chacha_urandomb(char *buf, int nbits);

  public:
    Prng(int type);
    Prng(int type, uint8_t *key, uint8_t *iv);
    int get_type();
    void get_randomb(char *buf, int nbits);
    void chacha_refill_randomness();  
};
#endif  // _CRYPTO_PRNG_H_


