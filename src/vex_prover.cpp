#include "vex_prover.h"
#include <iostream>

void
generate_audit_proofs(prover_input &in, prover_output *out)
{
  // sp = pi.sale_price
  // B = pi.vds[i].bid
  // w = pi.winner_idx
  // S' = pi.vds[i].sprime 
  
  std::vector<vex_proof> proofs;
  bool constructed_eq = 0;
  uint32_t sp = in.sale_price;
  
  for (int i = 0; i < (int) in.vds.size(); ++i) {

    uint32_t b_i = in.vds[i].bid;

    vex_proof pi;

    if (b_i >= sp && i == in.winner_idx) {

      pi.label = GREATER_THAN;

#ifdef NOCC
      uint8_t c[HASH_SIZE];
      uint8_t hs[HASH_SIZE];
      vex::gen_s(in.vds[i].sprime.data(), SEED_SIZE, hs);
      vex::gen_commit(MAX_BID - b_i, hs, HASH_SIZE, c);
      if (memcmp(c, in.vos[i].c.data(), HASH_SIZE) != 0)
        fatal << "Consistency check failed for GT bid\n";
#endif

    } else if (b_i == sp && !constructed_eq) {

      pi.label = EQUAL;
      pi.proof = in.vds[i].sprime;
      constructed_eq = 1;

#ifdef NOCC
      if (!vex::verify_eq_proof(MAX_BID - sp, in.vos[i].c.data(), HASH_SIZE, 
         in.vds[i].sprime.data(), SEED_SIZE, g_prefix))
        fatal << "Consistency check failed for EQUAL bid\n";
#endif


    } else { // b_i <= sp
    
      uint8_t hs[HASH_SIZE];
      pi.proof.resize(HASH_SIZE);
      vex::gen_s(in.vds[i].sprime.data(), SEED_SIZE, hs);    
  
      pi.label = LESS_THAN;
      vex::gen_proof(MAX_BID - sp, MAX_BID - b_i, hs, HASH_SIZE, pi.proof.data()); 

#ifdef NOCC
      if (!vex::verify_proof(MAX_BID - sp, in.vos[i].c.data(), HASH_SIZE, pi.proof.data(),
        HASH_SIZE))
        fatal << "Consistency check failed for LESS THAN bid\n";
#endif

    }

    proofs.push_back(pi);
  }

  if (out)
    out->proofs = proofs;
}
