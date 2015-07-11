#include "vex_bidder_cp_server.h"
#include "getopt_pp.h"

int
main(int argc, char* argv[])
{

  GetOpt::GetOpt_pp ops(argc, argv);

  if (ops >> GetOpt::OptionPresent('h', "help"))
    fatal << "usage: bidder_main [-r random seed] [-p port] \n";

  int port = 10000;
  int rand_seed = 12345;

  ops >> GetOpt::Option('p', "port", port)
      >> GetOpt::Option('s', "seed", rand_seed);

  srand(rand_seed);

  ref<vex_bidder_cp_server> bidder = New refcounted<vex_bidder_cp_server>();
  bidder->start_server(port);
  amain();
}
