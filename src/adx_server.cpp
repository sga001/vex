#include "adx_server.h"
//#include <iostream>

// Initialize the connection to bidders

void 
adx_server::init_conn(int num, str hosts[], int ports[])
{
  // Reserve enough space for n+1 connections
  conn_.reserve(num+1);

  // Add the seller connection, but do not initialize it
  conn_.push_back(New refcounted<net>());

  // Add and initialize all bidder connections
  for (int i = 1; i <= num; ++i) {
    conn_.push_back(New refcounted<net>());   
    conn_[i]->tcp_connect(hosts[i-1], ports[i-1]);
    
    // Set which function to call when receiving messages from
    // bidders.
    conn_[i]->a_read(wrap(this, &adx_server::read_cb, i));
  }

}

// Initialize the connection to the seller (this blocks)
void 
adx_server::init_seller_conn(str host, int port)
{
  strbuf start_message;
  
  conn_[SELLER_IDX]->tcp_connect(host, port);

  // Here you could have a different callback from the one used to handle
  // bidders, but I'm using the same function.
  conn_[SELLER_IDX]->a_read(wrap(this, &adx_server::read_cb, SELLER_IDX));  

  // Tell the seller to go ahead and start.
  uint8_t type = START;
  start_message << str((char*) &type, 1);
  conn_[SELLER_IDX]->a_write(start_message);
}


//-------------- Functions to handle events ----------------//

// Parse incoming message
void
adx_server::read_cb(int conn_idx, strbuf data)
{
  uint8_t type;
  int ret;

  type = get_type(data);
  
  switch (type) {
    case A_REQ:
      PROCESS_REQ(a_req);
      break;
   
    case B_SUB:
      PROCESS_REQ(b_sub);
      break;

    default:
      fatal << "Invalid request type: " << type << "\n";
  }
}

// Handle an auction request
void
adx_server::process_a_req(int conn_idx, const a_req &req)
{
  b_req breq;

  // Create an auction context
  auction_context ctx;
  ctx.id = req.id;
  auctions_ctx_.insert(std::pair<uint32_t, auction_context>(req.id, ctx));

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

  a_write(ALL_BIDDERS, B_REQ, breq); 
}


// Handle a bid submission
void
adx_server::process_b_sub(int conn_idx, const b_sub &sub)
{
  // Update auction context
  auction_context *ctx = &(auctions_ctx_.at(sub.id));
  assert(ctx);

  ctx->bids.insert(std::pair<int, uint32_t>(conn_idx, sub.bid));
  ctx->ad_tags.insert(std::pair<int, std::string>(conn_idx, sub.ad_tag));
  
//  std::cout << "New bid for " << sub.id << ": " << sub.bid << "\n";

  if (ctx->bids.size() == conn_.size() - 1) {
    ctx->outcome = compute_auction(ctx);
    report_outcome(ctx);
  }
}



//---------------- Functions to handle auction and outcome -------------//

std::pair<int, uint32_t> 
adx_server::compute_auction(auction_context *ctx)
{
  uint32_t max_bid = 0;
  uint32_t sp = 0;
  uint32_t c_bid = 0;
  int max_bidder = 0;
  
  for (auto it = ctx->bids.begin(); it != ctx->bids.end(); ++it) {
    c_bid = it->second;

    if (max_bid <= c_bid) {
      sp = max_bid;
      max_bid = c_bid;
      max_bidder = it->first;
    } else if (sp < c_bid) {
      sp = c_bid;
    }
  }

  return std::pair<int, uint32_t>(max_bidder, sp);
}


void
adx_server::report_outcome(auction_context *ctx)
{
  a_out_bidder bout;
  a_out_seller sout;

  bout.id = ctx->id;
  bout.price = ctx->outcome.second;

  sout.id = ctx->id;
  sout.price = ctx->outcome.second;
  sout.ad_tag = ctx->ad_tags.at(ctx->outcome.first);
  
  a_write(SELLER_IDX, A_OUT, sout);
  a_write(ctx->outcome.first, A_OUT, bout);
}

