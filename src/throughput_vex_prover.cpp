#include "throughput_util.h"
#include "throughput_vex_generator.h"
#include "getopt_pp.h"
#include <iostream>
#include "vex_prover.h"

void
run_experiment(int iterations, int num_auctions, int num_bidders, uint32_t max_price)
{
  for (int i = 0; i < iterations; ++i) {
    
    va_req req = gen_vareq();
  
    std::vector<prover_input> pis;

    // setup
    for (int j = 0; j < num_auctions; ++j) {
      pis.push_back(gen_prover_input(req.id, num_bidders, max_price));
    }
    
    // test
    long start_time = PAPI_get_virt_usec();

    for (int j = 0; j < num_auctions; ++j) {
      generate_audit_proofs(pis[j], NULL);
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
  uint32_t max_price = 1;

  ops >> GetOpt::Option('i', "iterations", iterations)
      >> GetOpt::Option('a', "auctions", num_auctions)
      >> GetOpt::Option('s', "random seed", rand_seed)
      >> GetOpt::Option('m', "random seed", max_price)
      >> GetOpt::Option('b', "bidders", num_bidders);

  srand(rand_seed);

  run_experiment(iterations, num_auctions, num_bidders, max_price);
  return 0;
}
