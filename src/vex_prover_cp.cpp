#include "vex_prover_cp.h"
#include <iostream>

void
generate_audit_proofs(prover_input &in, prover_output *out, std::vector<checkpoints> &cp_vault, int cp)
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

    } else if (b_i == sp && !constructed_eq) {

      pi.label = EQUAL;
      pi.proof = cp_vault.at(i).at(0);
      constructed_eq = 1;

    } else { // b_i <= sp
    
      uint8_t closest_cp[HASH_SIZE];
      pi.proof.resize(HASH_SIZE);

      // Note: sp - bi  = (MAX_BID - b_i) - (MAX_BID - sp) = x - q

      int closest = find_closest_cp(sp - b_i, cp);
      uint32_t remaining = find_remaining(sp - b_i, closest, cp);

      vex::gen_s(cp_vault.at(i).at(closest).data(), cp_vault.at(i).at(closest).size(), closest_cp);
  
      pi.label = LESS_THAN;
      // remaining = x - q (but starting from the closest cp) not from 0. 
      vex::gen_proof(0, remaining, closest_cp, HASH_SIZE, pi.proof.data()); 

    }

    proofs.push_back(pi);
  }

  if (out)
    out->proofs = proofs;
}
