#include "vex_verifier.h"

bool 
verify_audit_proofs(verifier_input &in, prover_output &p_in)
{
  int num_gt = 0;
  int num_eq = 0;
  uint32_t sp = in.sale_price;

  for (int i = 0; i < (int) p_in.proofs.size(); ++i) {
    
    if (p_in.proofs[i].label == GREATER_THAN) {

      if (in.winner_idx != i || in.htag != in.vos[i].htag) 
        fatal << "Wrong GT proof\n";

      num_gt++;

    } else if (p_in.proofs[i].label == EQUAL) {

      if (!vex::verify_eq_proof(MAX_BID - sp, in.vos[i].c.data(), HASH_SIZE,
         p_in.proofs[i].proof.data(), SEED_SIZE, g_prefix))
        fatal << "Wrong EQUAL proof\n";
      
      num_eq++;

    } else {
      
      if (!vex::verify_proof(MAX_BID-sp, in.vos[i].c.data(), HASH_SIZE, 
          p_in.proofs[i].proof.data(), HASH_SIZE))
        fatal << "Wrong LESS_THAN proof\n";
    }
  }


  if (num_gt != 1 || num_eq != 1)
    fatal << "Wrong labeling\n";

  return 1;
}

