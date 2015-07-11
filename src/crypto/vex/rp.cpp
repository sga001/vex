#include "rp.h"

#include <algorithm>
#include <string.h>

namespace vex {

static Prng prng(PNG_CHACHA);

int
random_bits(int length, uint8_t* buf)
{ 
  prng.get_randomb((char*) buf, length); // bits  
  return 0;
}

size_t 
comp_chain(int length, const uint8_t* s, size_t s_length, uint8_t* c)
{
  if (length < 0) {
    return -1;
  } else if (length == 0) {
    memcpy(c, s, s_length);
    return s_length;
  }

  HASH(s, s_length, c);

  for (int i = 0; i < length-1; ++i)
    HASH(c, HASH_SIZE, c);

  return HASH_SIZE; 
}

int
setup(int n, const uint8_t* prefix, uint8_t* sprime, uint8_t* s)
{
  memcpy(sprime, prefix, PREFIX_SIZE);
  prng.get_randomb((char*) sprime+PREFIX_SIZE, n*8); // bits  
  
  HASH(sprime, n+PREFIX_SIZE, s);

  return n+PREFIX_SIZE;
}

int
gen_commit(uint32_t x, const uint8_t* s, size_t s_length, uint8_t* c)
{
  return comp_chain((int) x, s, s_length, c);
}

int
gen_s(const uint8_t* sprime, size_t sp_length, uint8_t* s)
{
  return comp_chain(1, sprime, sp_length, s);
}

int
gen_salted_commit(uint32_t x, const uint8_t *s, size_t s_length, 
  const uint8_t *salt, size_t salt_length, uint8_t *c)
{
  uint8_t c_temp[HASH_SIZE + salt_length];
  comp_chain((int) x, s, s_length, c_temp);  
  memcpy(c_temp + HASH_SIZE, salt, salt_length);
  return comp_chain(1, c_temp, HASH_SIZE + salt_length, c);
}

int
gen_proof(uint32_t q, uint32_t x, const uint8_t* s, size_t s_length,
  uint8_t* p)
{
  return comp_chain((int) x - q, s, s_length, p);
}

int
verify_proof(uint32_t q, const uint8_t* c, size_t c_length,
  const uint8_t* p, size_t p_length)
{
  uint8_t c_temp[std::max(c_length, p_length)];
  
  if (comp_chain((int) q, p, p_length, c_temp) != c_length)
    return 0;
 
  return (memcmp(c_temp, c, c_length) == 0);
}

int
verify_eq_proof(uint32_t q, const uint8_t* c, size_t c_length,
  const uint8_t* p, size_t p_length, const uint8_t* prefix)
{

  if (verify_proof(q+1, c, c_length, p, p_length)) {
    if (memcmp(p, prefix, PREFIX_SIZE) == 0)
      return 1;
  }
  return 0;
}

int
verify_salted_proof(uint32_t q, const uint8_t *c, size_t c_length,
  const uint8_t *p, size_t p_length, const uint8_t *s, size_t s_length)
{
  uint8_t c_temp[c_length + s_length];
  uint8_t final[HASH_SIZE];
  
  if (comp_chain((int) q, p, p_length, c_temp) != c_length)
    return 0;
  
  // append salt
  memcpy(c_temp + c_length, s, s_length);
  
  // hash it
  HASH(c_temp, c_length + s_length, final);

  // compare
  return (memcmp(final, c, c_length) == 0);
}

int
verify_eq_salted_proof(uint32_t q, const uint8_t *c, size_t c_length,
  const uint8_t *p, size_t p_length, const uint8_t *s, size_t s_length,
  const uint8_t *prefix)
{

  if (verify_salted_proof(q+1, c, c_length, p, p_length, s, s_length)) {
    if (memcmp(p, prefix, PREFIX_SIZE) == 0)
      return 1;
  }

  return 0;

}

} // namespace vex
