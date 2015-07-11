#include "vex_adx_server.h"
#include "file_parser.h"

int
main (int argc, char *argv[])
{
  if (argc != 2)
    fatal << "usage: vex_adx_main <filename>\n";
  
  ref<vex_adx_server> adx = New refcounted<vex_adx_server>();

  std::vector<std::pair<std::string, int> > pairs =  parse_file(argv[1]);

  str hosts[pairs.size()-1];
  int ports[pairs.size()-1];  

  for (int i = 1; i < (int) pairs.size(); ++i) {
    hosts[i-1] = str(pairs.at(i).first.c_str());
    ports[i-1] = pairs.at(i).second; 
  }

  adx->init_conn(pairs.size()-1, hosts, ports);
  adx->init_seller_conn(str(pairs.at(0).first.c_str()), pairs.at(0).second);
  amain();
}
