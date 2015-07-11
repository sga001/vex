#ifndef _ADX_NET_H_
#define _ADX_NET_H_

#include "net.h"
#include "util.h"
#include "message_types.h"
#include <map>

#define SELLER_IDX 0
#define ALL_BIDDERS -1

// Macro that simplifies writing read_cb() functions
#define PROCESS_REQ(type_name) \
  do { \
    type_name req; \
    ret = parse_message(data, &req); \
    check_ret(ret, "parse_message()"); \
    process_##type_name(conn_idx, req); \
  } while (0);

struct auction_context {
  uint32_t id;
  std::map<int, uint32_t> bids; // map from conn_idx to bid
  std::map<int, std::string> ad_tags;
  std::pair<int, uint32_t> outcome; // conn_idx and price
};

class adx_server {
 public:
  adx_server() : conn_(), auctions_ctx_() {}
  ~adx_server() {}

  //----- Initialization functions -----//

  // Initialize net objects for seller and bidders
  // Also initializes the connection for the bidders, but not
  // the seller (call init_seller_conn to do that).
  void init_conn(int num, str hosts[], int ports[]);

  // Initializes the connection to the seller.
  void init_seller_conn(str host, int port);
  

  //----- Handle events ------//


  // Handle writing from msgpack structures
  template <typename T>
  void a_write(int conn_idx, uint8_t type, const T &req)
  {
    msgpack::sbuffer buf;
    msgpack::pack(&buf, req);
    
    strbuf send_data;
    send_data << str((char*) &type, 1) << str(buf.data(), buf.size());

    if (conn_idx == ALL_BIDDERS) {
      for (int i = 1; i < (int) conn_.size(); ++i)
        conn_[i]->a_write(send_data);

    } else {
      conn_[conn_idx]->a_write(send_data);
    }
  }


  // Read callback
  // conn_idx can be thought of as a "bidder_id"
  // conn_idx = 0 implies the seller.
  void read_cb(int conn_idx, strbuf data);

  // Handle requests
  void process_a_req(int conn_idx, const a_req &req);
  void process_b_sub(int conn_idx, const b_sub &req);

  // Compute the auction
  std::pair<int, uint32_t> compute_auction(auction_context *ctx);

  // Report the outcome
  void report_outcome(auction_context *ctx);

 private:
  vec<ref<net> > conn_;
  std::map<uint32_t, auction_context> auctions_ctx_;
};

#endif
