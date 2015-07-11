#include "vex_bidder_server.h"

void
vex_bidder_server::start_server(int port)
{
  conn_.tcp_listen(port, wrap(this, &vex_bidder_server::read_cb));
}


void
vex_bidder_server::read_cb(strbuf data)
{
  uint8_t type;
  int ret;
  
  type = get_type(data);
  
  switch (type) {
    case C_REQ:
      PROCESS_REQ(c_req);  
      break;
    
    case D_REQ:
      PROCESS_REQ(d_req);
      break;
    
    case A_OUT:
      PROCESS_REQ(va_out_bidder);
      break;
    
    default:
      fatal << "Invalid request type: " << type << "\n";  
  }
}


// Process commitment request
void
vex_bidder_server::process_c_req(c_req &req)
{
  c_sub commitment_msg; // vex commitment message
  auction_context ctx; // auction context 
 
  commitment_msg.id = req.id;
  ctx.id = req.id;
 
  ctx.sig_id = req.sig_id;
  ctx.vd.bid = compute_bid(req);
  ctx.vd.ad_tag = g_sample_ad;

  // Create vectors of the right size
  ctx.vo.c.resize(HASH_SIZE);
  ctx.vo.htag.resize(HASH_SIZE);
  ctx.vd.sprime.resize(SEED_SIZE);
  uint8_t s[HASH_SIZE];

  // Generate seeds (s' and s)
  vex::setup(HASH_SIZE, g_prefix, ctx.vd.sprime.data(), s);
  
  // Generate commitment to MAX_BID - bid and the ad tag
  vex::gen_commit(MAX_BID - ctx.vd.bid, s, HASH_SIZE, ctx.vo.c.data());
  vex::gen_commit(1, (uint8_t*) ctx.vd.ad_tag.c_str(), ctx.vd.ad_tag.size(), 
    ctx.vo.htag.data());

  // Add commitment and htag to the response
  commitment_msg.vo = ctx.vo;

  // Add new context to the list 
  auctions_ctx_.insert(std::pair<std::vector<uint8_t>, auction_context>(
                    ctx.id, ctx));

  // Send the commitment
  a_write(C_SUB, commitment_msg);
}


// Process decommitment request
void
vex_bidder_server::process_d_req(d_req &req)
{
  d_sub decommitment_msg;
  decommitment_msg.id = req.id;
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx); 
 
  ctx->sig_vo = req.sig_vo;
  ctx->bundle = req.bundle;  

  // Check to see if our own request is in the bundle
  bool check_own = false;
  for (auto it = ctx->bundle.begin(); it != ctx->bundle.end(); ++it) {
    if (ctx->vo == *it) {
      check_own = true;
      break;
    }
  }

#ifndef TPUT  
  if (!check_own)
    fatal << "Vex object not included...\n";
#endif

  // Finish constructing decommitment message, and send it
  decommitment_msg.vd = ctx->vd;
  a_write(D_SUB, decommitment_msg);
}

// Process auction's outcome
void
vex_bidder_server::process_va_out_bidder(va_out_bidder &req)
{
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx);

  ctx->outcome = req.sale_price;
}

uint32_t
vex_bidder_server::compute_bid(c_req &req)
{
  // can be based on req

  uint32_t bid = rand() % MAX_BID;
  return bid;
}
