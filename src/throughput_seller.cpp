#include "seller_server.h"
#include "getopt_pp.h"
#include "throughput_util.h"
#include <iostream>

static std::map<int, auction_context> ctx_map;

void
generate_auction(int i)
{
  
  a_req req = generate_a_req(i);
  
  a_t_write(A_REQ, req);

  auction_context ctx;
  ctx.id = i;
  ctx.start_time = req.timestamp;
  ctx_map.insert(std::pair<int, auction_context>(i, ctx));
}


void
process_outcome(int i, strbuf data)
{
  uint8_t type;
  int ret;
 
  type = get_type(data); 

  a_out_seller req;
  PROCESS_T_REQ(req);
  
  auction_context *ctx = &(ctx_map[i]);
  ctx->ad_tag = req.ad_tag;
  ctx->price = req.price;
  ctx->end_time = PAPI_get_real_usec();  
}

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

  ops >> GetOpt::Option('i', "iterations", iterations)
      >> GetOpt::Option('a', "auctions", num_auctions)
      >> GetOpt::Option('r', "random seed", rand_seed)
      >> GetOpt::Option('b', "bidders", num_bidders);

  srand(rand_seed);
  
  // Setup 

  for (int i = 0; i < iterations; ++i) {

    std::vector<strbuf> requests;
    char buf1[300];
    char buf2[100];      
    
    for (int j = 0; j < num_auctions; ++j) {
      a_out_seller req;
      req.id = (uint32_t) j;
      req.price = rand();
      gen_random_string(buf1, 299);
      gen_random_string(buf2, 99);
      req.ad_tag = std::string(buf1);
      req.result_url = std::string(buf2);
      requests.push_back(a_t_write(A_OUT, req));
    }


    // test
    long start_time = PAPI_get_virt_usec();
    
    for (int j = 0; j < num_auctions; j++) {
      generate_auction(j); 
      process_outcome(j, requests[j]);
    } 

    long stop_time = PAPI_get_virt_usec();

    std::cout << "Time: " 
              << ((double) stop_time - start_time) / (double) num_auctions 
              << " microseconds.\n";

  }

  return 0;
}
