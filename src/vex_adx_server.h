#ifndef _VEX_ADX_SERVER_H_
#define _VEX_ADX_SERVER_H_

#include "net.h"
#include "vex_util.h"
#include "vex_message_types.h"
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
  std::vector<uint8_t> id;
  std::map<int, vex_object> vos; // map from conn_idx to vex object
  std::map<int, vex_decommitment> vds;
  std::pair<int, int> outcome; // winner_idx and price_idx pair 
  std::vector<uint8_t> sig_id;
  std::vector<uint8_t> sig_vo;
  std::vector<uint8_t> sig_r;
  std::map<int, bool> invalid; // conn and correctly formed commitment
  std::map<int, vex_proof> proofs;  // for "free" audit optimization
  int seed_base_idx;  // seed_id (for VEX-CP)
};


class vex_adx_server {
 public:
  vex_adx_server() : conn_(), auctions_ctx_() {}
  ~vex_adx_server() {}

  //----- Initialization functions -----//

  // Initialize net objects for seller and bidders
  // Also initializes the connection for the bidders, but not
  // the seller (call init_seller_conn to do that).
  void init_conn(int num, str hosts[], int ports[]);

  // Initializes the connection to the seller.
  void init_seller_conn(str host, int port);

  //------- Handle events --------//

  // Handle writing from msgpack structures
  template <typename T>
  void a_write(int conn_idx, uint8_t type, const T &req)
  {
    msgpack::sbuffer buf;
    msgpack::pack(&buf, req);

    strbuf send_data;
    send_data << str((char*)&type, 1) << str(buf.data(), buf.size());
    do_a_write(conn_idx, send_data);
  }

  virtual void
  do_a_write(int conn_idx, strbuf send_data) {
 
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
  
  // auction request
  virtual void process_va_req(int conn_idx, const va_req &req);
  // commitment request
  void process_c_sub(int conn_idx, const c_sub &req);
  // signed bundle
  void process_sign_sub(int conn_idx, const sign_sub &req);
  // decommitment submission
  void process_d_sub(int conn_idx, const d_sub &req);
  // receipt submission
  void process_r_sub(int conn_idx, const r_sub &req);

  // Compute the auction
  std::pair<int, uint32_t> compute_auction(auction_context *ctx);

  // Consistency check
  virtual bool consistency_check(auction_context *ctx);
  bool consistency_and_proof(auction_context *ctx);

  // Report the outcome
  void report_outcome(auction_context *ctx);

 protected:
  vec<ref<net> > conn_;
  std::map<std::vector<uint8_t>, auction_context> auctions_ctx_;

};

#endif
