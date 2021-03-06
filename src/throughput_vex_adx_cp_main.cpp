#include "throughput_vex_adx_cp.h"
#include "getopt_pp.h"
#include <papi.h>

int
main(int argc, char* argv[])
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
  int cp_num = 2500;
  int cp_rate = 16;

  ops >> GetOpt::Option('i', "iterations", iterations)
      >> GetOpt::Option('a', "auctions", num_auctions)
      >> GetOpt::Option('r', "random seed", rand_seed)
      >> GetOpt::Option('v', "vault size", cp_num)
      >> GetOpt::Option('c', "checkpoint rate", cp_rate)
      >> GetOpt::Option('b', "bidders", num_bidders);

  srand(rand_seed);

  ref<throughput_vex_adx_cp> adx = New refcounted<throughput_vex_adx_cp>(cp_num, cp_rate);
  adx->init_checkpoint_vault();
  adx->fake_init_conn(num_bidders);
  adx->run_experiment(iterations, num_auctions, num_bidders);
}
