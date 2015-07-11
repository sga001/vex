#include "adx_server.h"
#include "throughput_util.h"
#include "getopt_pp.h"
#include <iostream>

static std::map<int, auction_context> ctx_map;

void rep_outcome(auction_context *ctx);
std::pair<int, uint32_t> comp_auction(auction_context *ctx);

void
process_a_request(int i, int num_bidders, strbuf data)
{
  a_req req;
  PROCESS_T_REQ(req); 
  
  b_req breq;

  auction_context ctx;
  ctx.id = req.id;
  ctx_map.insert(std::pair<int, auction_context>(req.id, ctx));
  
  breq.id = req.id;
  breq.url = req.url;
  breq.width = req.width;
  breq.height = req.height;
  breq.reserve_price = req.reserve_price;
  breq.cookie_version = req.cookie_version;
  breq.cookie_age_seconds = req.cookie_age_seconds;
  breq.cookie_id = req.cookie_id;
  breq.user_agent = req.user_agent;
  breq.geo_criteria_id = req.geo_criteria_id;
  breq.postal_code = req.postal_code;
  breq.timezone_offset = req.timezone_offset;
  breq.timestamp = req.timestamp;

  breq.ip = rand();
  breq.seller_network_id = rand();
  breq.detected_language = rand();

  for (int j = 0; j < num_bidders; ++j) {
    a_t_write(B_REQ, breq);
  }
}

void
process_b_sub(int i, int k, int num_bidders, strbuf data)
{
  b_sub req;
  PROCESS_T_REQ(req);

  auction_context *ctx = &(ctx_map[i]);
  ctx->bids.insert(std::pair<int, uint32_t>(k, req.id));
  ctx->ad_tags.insert(std::pair<int, std::string>(k, req.ad_tag));
  
  if (ctx->bids.size() == (uint32_t) num_bidders) {
    ctx->outcome = comp_auction(ctx);
    rep_outcome(ctx);
  }
}

void
rep_outcome(auction_context *ctx)
{
  a_out_bidder bout;
  a_out_seller sout;

  bout.id = ctx->id;
  bout.price = ctx->outcome.second;

  sout.id = ctx->id;
  sout.price = ctx->outcome.second;
  sout.ad_tag = ctx->ad_tags[ctx->outcome.first];
  
  a_t_write(A_OUT, sout);
  a_t_write(A_OUT, bout);
}


std::pair<int, uint32_t>
comp_auction(auction_context *ctx)
{
  uint32_t max_bid = 0;
  uint32_t sp = 0;
  uint32_t c_bid = 0;
  int max_bidder = 0;

  for (auto it = ctx->bids.begin(); it != ctx->bids.end(); ++it) {
    c_bid = it->second;

    if (max_bid < c_bid) {
      sp = max_bid;
      max_bid = c_bid;
      max_bidder = it->first;
    } else if (sp < c_bid) {
      sp = c_bid;
    }
  }

  return std::pair<int, uint32_t>(max_bidder, sp);
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


  for (int i = 0; i < iterations; ++i) {
    std::vector<strbuf> requests;
    std::vector<strbuf> bids;
    
    // Setup
    for (int j = 0; j < num_auctions; ++j) {
      a_req req = generate_a_req(j);
      requests.push_back(a_t_write(B_REQ, req));

      for (int k = 0; k < num_bidders; ++k) {
        b_sub sub = generate_bid(j);
        bids.push_back(a_t_write(B_SUB, sub));
      }
    }

   // Test
   long start_time = PAPI_get_virt_usec();
 
   for (int j = 0; j < num_auctions; ++j) {
      process_a_request(i, num_bidders, requests[j]);
    
      for (int k = 0; k < num_bidders; ++k)
        process_b_sub(j, k, num_bidders, bids[j * num_bidders + k]);
    }
    
    long stop_time = PAPI_get_virt_usec();
    
    std::cout << "Time: "
              << ((double) stop_time - start_time) / (double) num_auctions
              << " microseconds.\n";
  }

  return 0;
}

