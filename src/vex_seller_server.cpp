#include "vex_seller_server.h"
#include <iostream>

void
vex_seller_server::start_server(int port)
{
  conn_.tcp_listen(port, wrap(this, &vex_seller_server::read_cb));
}

void
vex_seller_server::read_cb(strbuf data)
{
  uint8_t type;
  int ret;
  
  type = get_type(data);
  
  switch (type) {
    case START:
      schedule_auctions();
      break;
    
    case SIGN_REQ:
      PROCESS_REQ(sign_req);
      break;
    
    case A_OUT:
      PROCESS_REQ(va_out_seller);
      break;

    default:
      fatal << "Invalid request type: " << type << "\n";
  }
}


void
vex_seller_server::process_sign_req(const sign_req &req)
{
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx);

  sign_sub sub;
  
  ctx->bundle = req.bundle;
  sub.id = req.id;

  // Get data of all vex objects
  msgpack::sbuffer buf;
  msgpack::pack(&buf, req.id);
  msgpack::pack(&buf, req.bundle);

  str data = str(buf.data(), buf.size());

  // Sign the id || bundle
  bigint sig_vo_int = priv_key_.sign(data);
  get_raw_bigint(sig_vo_int, &sub.sig_vo);
  
  a_write(SIGN_SUB, sub);
}

void
vex_seller_server::process_va_out_seller(const va_out_seller &req)
{

  auction_context *ctx = &(auctions_ctx_.at(req.id));
  assert(ctx);
 
  // Get the end time
  ctx->end_time = PAPI_get_real_usec(); 
 
  r_sub sub;
  
  ctx->winner_idx = req.winner_idx;
  ctx->sale_price = req.sale_price;
  ctx->ad_tag = req.ad_tag;

  sub.id = req.id;
  
  // Sign id || winner_idx || sale_price || htag of winner 
  
  msgpack::sbuffer buf;
  msgpack::pack(&buf, req.id);
  msgpack::pack(&buf, req.winner_idx);
  msgpack::pack(&buf, req.sale_price);
  msgpack::pack(&buf, ctx->bundle.at(req.winner_idx-1).htag);

  str data = str(buf.data(), buf.size());
  
  // Do the actual signing
  bigint sig_r_int = priv_key_.sign(data);
  get_raw_bigint(sig_r_int, &sub.sig_r);
  
  a_write(R_SUB, sub);

  if (sent_auctions_ >= total_auctions_) {
    if (req.id == last_id_) {
      for (auto it = auctions_ctx_.begin(); it != auctions_ctx_.end(); ++it) {
        long total_time = it->second.end_time - it->second.start_time;
        warn << "Auction " << hexdump(it->first.data(), it->first.size()) 
             << " time: " << total_time << " microseconds.\n";
      }
     conn_.close_net();
     exit(0);
    } 
  }

}

double
vex_seller_server::poisson_next_time()
{
  double u = drand48(); // [0, 1]
  return log(1-u)/(-lambda_);
}

void
vex_seller_server::schedule_auctions()
{
   if (++sent_auctions_ > total_auctions_) {
    return;
  }

  generate_auctions();

 // Schedule next
  int next_time = ceil(poisson_next_time() * kSecToNanosec);
  delaycb(0, next_time, wrap(this, &vex_seller_server::schedule_auctions));
}


void
vex_seller_server::generate_auctions()
{
  va_req req;
  req.id.resize(HASH_SIZE);
  get_page_data(&req);
  get_user_data(&req);
  
  req.timestamp = PAPI_get_real_usec(); 
  
  msgpack::sbuffer buf;
  msgpack::pack(&buf, req.url);
  msgpack::pack(&buf, req.timestamp);
 
  // Get id = H(url || timestamp) (for now)
  HASH(buf.data(), buf.size(), req.id.data());

  // Sign the id
  bigint sig_id_int = priv_key_.sign(str((char*) req.id.data(), req.id.size()));
  get_raw_bigint(sig_id_int, &req.sig_id); 
  
  a_write(A_REQ, req);
  
  // Add context to list
  auction_context ctx;
  ctx.id = req.id;
  ctx.start_time = req.timestamp;
  auctions_ctx_.insert(std::pair<std::vector<uint8_t>, auction_context>(ctx.id, ctx));
  
  if (sent_auctions_ == total_auctions_)
    last_id_ = req.id;

  //warn << "Auction num: " << sent_auctions_ << "\n";
 }
