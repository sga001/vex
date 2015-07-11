#include "seller_server.h"
#include "getopt_pp.h"
int
main (int argc, char* argv[])
{
  GetOpt::GetOpt_pp ops(argc, argv);
  
  if (ops >> GetOpt::OptionPresent('h', "help"))
    fatal << "usage: seller_main  [-p port] [-l offered_load] [-t total auctions]\n";
  
  int port = 9999;
  int total_auctions = 2000;
  double lambda = 500;
  int seed = 12345;

  ops >> GetOpt::Option('p', "port", port)
      >> GetOpt::Option('l', "lambda", lambda)
      >> GetOpt::Option('s', "seed", seed)
      >> GetOpt::Option('t', "total", total_auctions);

  srand(seed);
  
  ref<seller_server> seller = New refcounted<seller_server>(total_auctions, lambda);
  seller->start_server(port);
  amain();
}
