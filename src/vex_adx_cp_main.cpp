#include "vex_adx_cp_server.h"
#include "file_parser.h"
#include "getopt_pp.h"

int
main (int argc, char *argv[])
{
  
  GetOpt::GetOpt_pp ops(argc, argv);

  if (ops >> GetOpt::OptionPresent('h', "help"))
    fatal << "usage: " << argv[0] << " [-s rand_seed] [-i iterations] [-a auctions] -[b bidders] \n";

  if (argc < 2)
    fatal << "usage: vex_adx_main <filename>\n";
  
  int cpoints = 2500;
  int cp = 16;

  ops >> GetOpt::Option('c', "checkpoint rate", cp);
 
  ref<vex_adx_cp_server> adx = New refcounted<vex_adx_cp_server>(cpoints, cp);

  std::vector<std::pair<std::string, int> > pairs =  parse_file(argv[1]);


  str hosts[pairs.size()-1];
  int ports[pairs.size()-1];  

  for (int i = 1; i < (int) pairs.size(); ++i) {
    hosts[i-1] = str(pairs.at(i).first.c_str());
    ports[i-1] = pairs.at(i).second; 
  }
  
  adx->init_checkpoint_vault();
  adx->init_conn(pairs.size()-1, hosts, ports);
  adx->init_seller_conn(str(pairs.at(0).first.c_str()), pairs.at(0).second);
  amain();
}
