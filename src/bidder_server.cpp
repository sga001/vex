#include "bidder_server.h"
#include <iostream>

void
bidder_server::start_server(int port)
{
  conn_.tcp_listen(port, wrap(this, &bidder_server::read_cb));
}


void
bidder_server::read_cb(strbuf data)
{
  uint8_t type;
  int ret;
  
  type = get_type(data);
 
  switch (type) {
    case B_REQ:
      PROCESS_REQ(b_req);
      break;

    case A_OUT:
      PROCESS_REQ(a_out_bidder);
      break;
    
    default:
      fatal << "Invalid request type: " << type << "\n";
  }
}

// Process bid request
void
bidder_server::process_b_req(const b_req &req)
{
  b_sub res;
  int len = rand() % 200;
  char buf[len];

  res.id = req.id;
  
  // Bid
  res.bid = compute_bid(req);  

  // Ad properties
  res.ad_tag = g_sample_ad; // sample ad found in util.h
  
  res.width = rand() % 2000;
  res.height = rand() % 2000;
  gen_random_string(buf, len);
  res.result_url = std::string(buf);

  a_write(B_SUB, res);

  // Write context

  auction_context ctx;
  ctx.id = req.id;
  ctx.bid = res.bid;
  ctx.ad = res.ad_tag;
  
  auctions_ctx_.insert(std::pair<uint32_t, auction_context>(ctx.id, ctx));
}

// Process auction's outcome
void
bidder_server::process_a_out_bidder(const a_out_bidder &req)
{
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  ctx->outcome = req.price;
}


uint32_t
bidder_server::compute_bid(const b_req &req)
{
  // Compute Bid  
  return (rand() + req.id) % MAX_BID;
}
