#include "throughput_util.h"
#include "throughput_vex_generator.h"
#include "getopt_pp.h"
#include <iostream>
#include "vex_prover_cp.h"

c_sub
gen_c_d_sub_salted(std::vector<uint8_t> id, checkpoints &cp_list, d_sub *dec, uint32_t max_price, bool set = true){
  c_sub commitment_msg;

  commitment_msg.id = id;
  vex_object vo;
  vex_decommitment vd;

  if (set)
    vd.bid = max_price;
  else
    vd.bid = rand() % max_price;
  vd.ad_tag = g_sample_ad;

  // Create vectors of the right size
  vo.c.resize(HASH_SIZE);
  vo.htag.resize(HASH_SIZE);
  vd.sprime.resize(SALT_SIZE);

  // Generate salt
  vex::random_bits(SALT_SIZE * 8, vd.sprime.data());

  std::vector<uint8_t> the_seed = cp_list[0];


  // Generate commitment to MAX_BID - bid and the ad tag
  vex::gen_salted_commit((MAX_BID - vd.bid) + 1, the_seed.data(), SEED_SIZE,
    vd.sprime.data(), SALT_SIZE, vo.c.data());
  vex::gen_commit(1, (uint8_t*) vd.ad_tag.c_str(), vd.ad_tag.size(),
    vo.htag.data());

  // Add commitment and htag to the response
  commitment_msg.vo = vo;
 
  if (dec) {
    dec->id = id;
    dec->vd = vd;
  }

  return commitment_msg;
}


prover_input
gen_prover_cp_input(std::vector<uint8_t> id, std::vector<checkpoints> &cp_vault, int bidders, uint32_t max_price)
{
 prover_input pi;

  for (int i = 0; i < bidders; ++i) {
    d_sub d;
    if (i < 2)
      pi.vos.push_back(gen_c_d_sub_salted(id, cp_vault[i], &d, max_price, true).vo);
    else
      pi.vos.push_back(gen_c_d_sub_salted(id, cp_vault[i], &d, max_price).vo);
    pi.vds.push_back(d.vd);
  }

  gen_auction_outcome(&pi);

  return pi;
}

void
run_experiment(int iterations, int num_auctions, int num_bidders, int cp, uint32_t max_price)
{
  std::vector<checkpoints> cp_vault;

  init_checkpoint_vault(&cp_vault, cp, num_bidders);

  for (int i = 0; i < iterations; ++i) {
    
    va_req req = gen_vareq();
  
    std::vector<prover_input> pis;
  
    // setup
    for (int j = 0; j < num_auctions; ++j) {
      pis.push_back(gen_prover_cp_input(req.id, cp_vault, num_bidders, max_price));
    }

    // test
    long start_time = PAPI_get_virt_usec();

    for (int j = 0; j < num_auctions; ++j) {
      generate_audit_proofs(pis[j], NULL, cp_vault, cp);
    }

    long stop_time = PAPI_get_virt_usec();

    std::cout << "Time: "
              << ((double) stop_time - start_time) / (double) num_auctions
              << " microseconds.\n";
  }

}


int
main(int argc, char *argv[])
{
  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    return 0;

  GetOpt::GetOpt_pp ops(argc, argv);

  if (ops >> GetOpt::OptionPresent('h', "help"))
    fatal << "usage: " << argv[0] << " [-s rand_seed] [-i iterations] [-a auctions] -[b bidders] \n";

  int iterations = 100;
  int num_auctions = 100;
  int num_bidders = 100;
  int rand_seed = 1124;
  int cp = 16;
  uint32_t max = 10000;

  ops >> GetOpt::Option('i', "iterations", iterations)
      >> GetOpt::Option('a', "auctions", num_auctions)
      >> GetOpt::Option('r', "random seed", rand_seed)
      >> GetOpt::Option('c', "cp num", cp)
      >> GetOpt::Option('m', "max price", max)
      >> GetOpt::Option('b', "bidders", num_bidders);

  srand(rand_seed);

  run_experiment(iterations, num_auctions, num_bidders, cp, max);
  return 0;
}
