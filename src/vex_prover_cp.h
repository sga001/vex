#ifndef _VEX_PROVER_CP_H_
#define _VEX_PROVER_CP_H_

#include "vex_util.h"
#include "vex_message_types.h"

typedef std::vector<std::vector<uint8_t> > checkpoints;


static int
find_closest_cp(uint32_t num, int cp)
{
  int closest = 0;

  for (int i = 0; i < cp; ++i) {
    if ((uint32_t) (MAX_BID/cp) * i <= num)
      closest = i;
    else
      break;
  }
  return closest;
}


static uint32_t
find_remaining(uint32_t num, int closest, int cp)
{
  uint32_t remaining = num - (closest * (MAX_BID/cp));

  if (closest == 0)
    remaining++;

  return remaining;
}


static void
init_checkpoint_vault(std::vector<checkpoints> *cps, int cp, int cp_num)
{
  for (int i = 0; i < cp_num; ++i) {
    std::vector<uint8_t> seed;
    std::vector<uint8_t> c; // 1 checkpoint
    checkpoints cp_l;        // list of checkpoints

    seed.resize(SEED_SIZE);
    c.resize(HASH_SIZE);
    cp_l.reserve(cp);

    vex::setup(HASH_SIZE, g_prefix, seed.data(), c.data());
    cp_l.push_back(seed); // idx 0 = seed

    for (int j = 0; j < cp-1; ++j) {
      vex::gen_commit((MAX_BID / cp), c.data(), HASH_SIZE, c.data());
      cp_l.push_back(c); // add checkpoint to list
    }
    // add list to vault
    cps->push_back(cp_l);
  }
}


void generate_audit_proofs(prover_input &in, prover_output *out, std::vector<checkpoints> &cp_vault, int cp);

#endif
