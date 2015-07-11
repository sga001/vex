#include "seller_server.h"
#include <iostream>

void
seller_server::start_server(int port)
{
  conn_.tcp_listen(port, wrap(this, &seller_server::read_cb));
}

void
seller_server::read_cb(strbuf data)
{
  uint8_t type;
  int ret;

  type = get_type(data);

  switch (type) {
    case START:
      generate_auctions();
      break;
    case A_OUT:
      PROCESS_REQ(a_out_seller);
      break;

    default:
      fatal << "Invalid request type: " << type << "\n";
  }

}

void
seller_server::process_a_out_seller(const a_out_seller &req)
{
  
  auction_context *ctx = &(auctions_ctx_.at(req.id));
  ctx->ad_tag = req.ad_tag;
  ctx->price = req.price;
  ctx->end_time = PAPI_get_real_usec();

  if ((int) req.id >= total_auctions_) {
    for (int i = 1; i <= total_auctions_; ++i) {
      long total_time = auctions_ctx_.at(i).end_time - auctions_ctx_.at(i).start_time;
      std::cout << "Auction " << i << " time: " << total_time << " microseconds.\n";
    }
    conn_.close_net();
    exit(0);
  }
}

double
seller_server::poisson_next_time()
{
  double u = drand48(); // [0, 1]
  return log(1-u) /(-lambda_); 
}

void
seller_server::generate_auctions()
{
  if (++sent_auctions_ > total_auctions_) {
    return;
  }

  a_req req;
  req.id = sent_auctions_;

  get_page_data(&req); // sets ad slot, and page data
  get_user_data(&req); // sets user and browser data
  req.timestamp = PAPI_get_real_usec();
 
  a_write(A_REQ, req);
 
  // Add context to list 
  auction_context ctx;
  ctx.id = sent_auctions_; 
  ctx.start_time = req.timestamp;
  auctions_ctx_.insert(std::pair<uint32_t, auction_context>(ctx.id, ctx)); 
  
  // Schedule next
  int next_time = ceil(poisson_next_time() * kSecToNanosec); 
  delaycb(0, next_time, wrap(this, &seller_server::generate_auctions)); 
}
