#include "bidder_server.h"
#include "getopt_pp.h"
#include "throughput_util.h"
#include <iostream>

static std::map<int, auction_context> ctx_map;


void
process_bid_request(int i, strbuf data)
{
  b_req req;
  PROCESS_T_REQ(req);
  
  b_sub res = generate_bid(i);
  a_t_write(B_SUB, res);

  auction_context ctx;
  ctx.id = req.id;
  ctx.bid = res.bid;
  ctx.ad = res.ad_tag;
  
  ctx_map.insert(std::pair<int, auction_context>(i, ctx));
}



void
process_outcome(int i, strbuf data)
{
  uint8_t type;
  int ret;
  type = get_type(data);

  a_out_bidder req;
  PROCESS_T_REQ(req);
  
  auction_context *ctx = &(ctx_map[req.id]);
  ctx->outcome = req.price;
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
    std::vector<strbuf> outcomes;
    
    for (int j = 0; j < num_auctions; ++j) {
      b_req req = generate_b_req(j);
      requests.push_back(a_t_write(B_REQ, req));

      a_out_bidder out;
      out.id = j;
      out.price = rand();
      outcomes.push_back(a_t_write(A_OUT, out));
    }


    // Test
    long start_time = PAPI_get_virt_usec();

    for (int j = 0; j < num_auctions; ++j) {
      process_bid_request(j, requests[j]);
      process_outcome(j, outcomes[j]);
    }
    
    long stop_time = PAPI_get_virt_usec();

    std::cout << "Time: "
              << ((double) stop_time - start_time) / (double) num_auctions
              << " microseconds.\n";
  }    
  
  return 0;
}
