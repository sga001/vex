#include "vex_adx_server.h"

// Initialize the connection to bidders

void
vex_adx_server::init_conn(int num, str hosts[], int ports[])
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
    conn_[i]->a_read(wrap(this, &vex_adx_server::read_cb, i));
  }

  warn << "Connected to all bidders\n";
}

// Initialize the connection to the seller (this blocks)
void
vex_adx_server::init_seller_conn(str host, int port)
{
  strbuf start_message;

  conn_[SELLER_IDX]->tcp_connect(host, port);

  // Here you could have a different callback from the one used to handle
  // bidders, but I'm using the same function.
  conn_[SELLER_IDX]->a_read(wrap(this, &vex_adx_server::read_cb, SELLER_IDX));

  warn << "Connected to the seller. Sending START message.\n";
  // Tell the seller to go ahead and start.
  uint8_t type = START;
  start_message << str((char*) &type, 1);
  conn_[SELLER_IDX]->a_write(start_message);
}

// Parse incoming message
void
vex_adx_server::read_cb(int conn_idx, strbuf data)
{
  uint8_t type;
  int ret;

  type = get_type(data);
  
  switch (type) {
    case A_REQ:
      PROCESS_REQ(va_req);
      break;

    case C_SUB:
      PROCESS_REQ(c_sub);
      break;

    case D_SUB:
      PROCESS_REQ(d_sub);
      break;

    case SIGN_SUB:
      PROCESS_REQ(sign_sub);
      break;

    case R_SUB:
      PROCESS_REQ(r_sub);
      break;
    default:
      fatal << "Invalid request type: " << type << "\n";
  }
}


// Handle requests

void
vex_adx_server::process_va_req(int conn_idx, const va_req &req)
{
  c_req creq;

  // Create auction context
  auction_context ctx;
  ctx.id = req.id;
  ctx.sig_id = req.sig_id;
  auctions_ctx_.insert(std::pair<std::vector<uint8_t>, auction_context>(req.id, ctx));

  creq.id = req.id;
  creq.sig_id = req.sig_id;
  creq.url = req.url;
  creq.width = req.width;
  creq.height = req.height;
  creq.reserve_price = req.reserve_price;
  creq.cookie_version = req.cookie_version;
  creq.cookie_age_seconds = req.cookie_age_seconds;
  creq.cookie_id = req.cookie_id;
  creq.user_agent = req.user_agent;
  creq.geo_criteria_id = req.geo_criteria_id;
  creq.postal_code = req.postal_code;
  creq.timezone_offset = req.timezone_offset;
  creq.timestamp = req.timestamp;

  creq.ip = rand();
  creq.seller_network_id = rand();
  creq.detected_language = rand();

  a_write(ALL_BIDDERS, C_REQ, creq);
}

void
vex_adx_server::process_c_sub(int conn_idx, const c_sub &req)
{
  //Update auction context
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx);

  ctx->vos.insert(std::pair<int, vex_object>(conn_idx, req.vo));
  
  if (ctx->vos.size() == conn_.size() - 1) {
    //Send SIGN_REQ
    sign_req sreq;
    sreq.id = req.id;
    map_to_vector(ctx->vos, sreq.bundle);   
    a_write(SELLER_IDX, SIGN_REQ, sreq);
  }
}


void
vex_adx_server::process_sign_sub(int conn_idx, const sign_sub &req)
{
  //Update auction context
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx);
  
  ctx->sig_vo = req.sig_vo;
  
  // Send D_REQ
  d_req dreq;
  dreq.id = ctx->id;
  dreq.sig_vo = ctx->sig_vo;
  map_to_vector(ctx->vos, dreq.bundle);  
  a_write(ALL_BIDDERS, D_REQ, dreq);
}


void
vex_adx_server::process_d_sub(int conn_idx, const d_sub &req)
{
  // Update auction context
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx);
  
  ctx->vds.insert(std::pair<int, vex_decommitment>(conn_idx, req.vd));
  
  // When full, check consistency, compute auction, and report_outcome(ctx)
  if (ctx->vds.size() == conn_.size() - 1) {
    

#ifndef NOCC
    // currently NOT using optimized approach
    // To switch: replaces with consistency_and_proof(ctx);
    // and put compute_auction before.
    if (!consistency_check(ctx)) {
//    ctx->outcome = compute_auction(ctx);
      fatal << "Consistency check failed...\n";
    }
#endif
    ctx->outcome = compute_auction(ctx);
    report_outcome(ctx);
  }
}

void
vex_adx_server::process_r_sub(int conn_idx, const r_sub &req)
{
  // Update auction context
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx);
  ctx->sig_r = req.sig_r;
}


//-------------- Functions to handle auction and outcome -----------//

std::pair<int, uint32_t>
vex_adx_server::compute_auction(auction_context *ctx)
{
  uint32_t max_bid = 0;
  uint32_t sp = 0;
  uint32_t c_bid = 0;
  int max_bidder = 1;
  int price_idx = 1;

  for (auto it = ctx->vds.begin(); it != ctx->vds.end(); ++it) {

    if (ctx->invalid.find(it->first) != ctx->invalid.end())
      continue;

    c_bid = it->second.bid;

    if (max_bid < c_bid) {
      sp = max_bid;
      price_idx = max_bidder;

      max_bid = c_bid;
      max_bidder = it->first;
    } else if (sp < c_bid) {
      sp = c_bid;
      price_idx = it->first;
     }
  }

  
  return std::pair<int, int>(max_bidder, price_idx);
}


void
vex_adx_server::report_outcome(auction_context *ctx)
{
  va_out_bidder bout;
  va_out_seller sout;
  
 // Send outcome 
  bout.id = ctx->id;
  bout.sale_price = ctx->vds.at(ctx->outcome.second).bid;
  bout.valid = ctx->invalid.size() ? 1 : 0;
  
  sout.id = ctx->id;
  sout.sale_price = bout.sale_price;
  sout.winner_idx = ctx->outcome.first;
  sout.ad_tag = ctx->vds.at(ctx->outcome.first).ad_tag;
  sout.valid = bout.valid;

  a_write(SELLER_IDX, A_OUT, sout);
  a_write(ctx->outcome.first, A_OUT, bout);
}


bool
vex_adx_server::consistency_check(auction_context *ctx)
{

  uint8_t htag[HASH_SIZE];
  bool ret = 1;

  for (int i = 1; i < (int) ctx->vos.size(); ++i) {
    vex_decommitment *dec = &(ctx->vds.at(i));
    vex_object *com = &(ctx->vos.at(i));

    HASH((uint8_t*) dec->ad_tag.c_str(), dec->ad_tag.size(), htag);

    if(!vex::verify_eq_proof(MAX_BID - dec->bid, com->c.data(), com->c.size(),
       dec->sprime.data(), dec->sprime.size(), g_prefix) ||
      memcmp(com->htag.data(), htag, HASH_SIZE) != 0) {
          
      ctx->invalid.insert(std::pair<int, bool>(i, 1)); // 1 implies wrong
      ret = 0;
      fatal << "consistency check failing\n";
     }
  }

  return ret;
}

// Heuristic for proof generation
bool
vex_adx_server::consistency_and_proof(auction_context *ctx)
{
  uint8_t htag[HASH_SIZE];
  uint8_t p[HASH_SIZE];
  bool ret = 1;
  int winner_idx = ctx->outcome.first;
  int sale_price_idx = ctx->outcome.second;
  vex_decommitment *dec;
  vex_object *com;

 // warn << "Winner idx: " << winner_idx << "\n";
 // warn << "Sale Price idx: " << sale_price_idx << "\n";
  // Check winner idx
  dec = &(ctx->vds.at(winner_idx));
  com = &(ctx->vos.at(winner_idx));
  
  HASH((uint8_t*) dec->ad_tag.c_str(), dec->ad_tag.size(), htag);

 if(!vex::verify_eq_proof(MAX_BID - dec->bid, com->c.data(), com->c.size(),
    dec->sprime.data(), dec->sprime.size(), g_prefix) || 
    memcmp(com->htag.data(), htag, HASH_SIZE) != 0) {
      ctx->invalid.insert(std::pair<int, bool>(winner_idx, 1));
      ret = 0;
  } else {
    vex_proof proof;
    proof.label = GREATER_THAN;
    ctx->proofs.insert(std::pair<int, vex_proof>(winner_idx, proof));
  }

  // Check sale_price idx
  dec = &(ctx->vds.at(sale_price_idx));
  com = &(ctx->vos.at(sale_price_idx));
  
  HASH((uint8_t*) dec->ad_tag.c_str(), dec->ad_tag.size(), htag);

 if(!vex::verify_eq_proof(MAX_BID - dec->bid, com->c.data(), com->c.size(),
    dec->sprime.data(), dec->sprime.size(), g_prefix) || 
    memcmp(com->htag.data(), htag, HASH_SIZE) != 0) {
      ctx->invalid.insert(std::pair<int, bool>(sale_price_idx, 1));
      ret = 0;
  } else {
    vex_proof proof;
    proof.label = EQUAL;
    proof.proof = dec->sprime;
    ctx->proofs.insert(std::pair<int, vex_proof>(sale_price_idx, proof));
  }

  uint32_t sale_price = ctx->vds.at(sale_price_idx).bid;

  // Check the rest
  for (int i = 1; i < (int) ctx->vos.size(); ++i) {
    if (i == sale_price_idx || i == winner_idx)
      continue;
    
    // Less than or equal to case

    dec = &(ctx->vds.at(i));
    com = &(ctx->vos.at(i));
  
    HASH((uint8_t*) dec->ad_tag.c_str(), dec->ad_tag.size(), htag);
   
    // the +1 in (MAX_BID - bid)+1 is because we are starting
    // from sprime and not s.
    vex::gen_proof(MAX_BID - sale_price, (MAX_BID - dec->bid)+1, dec->sprime.data(),
      dec->sprime.size(), p); 

    if(!vex::verify_proof(MAX_BID - sale_price, com->c.data(), com->c.size(),
      p, HASH_SIZE) || memcmp(com->htag.data(), htag, HASH_SIZE) != 0) {

      ctx->invalid.insert(std::pair<int, bool>(i, 1));
    
      if (ret) {
        ctx->proofs.clear(); // get rid of the proofs
        return 1; // no need to redo auction, but it'll be non-auditable
      }

    } else {
      // This commitment is fine. Add proof to list
      vex_proof proof;
      proof.label = LESS_THAN;
      proof.proof = std::vector<uint8_t>(p, p+HASH_SIZE);
      ctx->proofs.insert(std::pair<int, vex_proof>(i, proof));
    }
  }

  return ret;
}
