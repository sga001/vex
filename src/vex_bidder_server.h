#ifndef _VEX_BIDDER_SERVER_H_
#define _VEX_BIDDER_SERVER_H_

#include "net.h"
#include "vex_util.h"
#include "vex_message_types.h"

#include <map>

// Macro that simplifies writing read_cb() functions
#define PROCESS_REQ(type_name) \
  do { \
    type_name req; \
    ret = parse_message(data, &req); \
    check_ret(ret, "parse_message()"); \
    process_##type_name(req); \
  } while (0);

struct auction_context {
  std::vector<uint8_t> id;  
  vex_object vo;
  vex_decommitment vd;  
  std::vector<uint8_t> sig_id;
  std::vector<uint8_t> sig_vo;
  std::vector<vex_object> bundle;
  uint32_t outcome; // if winner, outcome > 0 
};


class vex_bidder_server {
 public:
  vex_bidder_server() : conn_(), auctions_ctx_() {}

  ~vex_bidder_server() {}

  // Starts the server
  void start_server(int port);

  //------ Handle Events -----//
  
  // Handle writing from msgpack structures
  template <typename T>
  void a_write(uint8_t type, T req) {
    msgpack::sbuffer buf;
    msgpack::pack(&buf, req);

    strbuf send_data;
    send_data << str((char*) &type, 1) << str(buf.data(), buf.size());
    do_a_write(send_data);
  }

  virtual void do_a_write(strbuf data) {
    conn_.a_write(data);
  }

  // read callback
  void read_cb(strbuf data);

  // Handle requests
  virtual void process_c_req(c_req &req);
  void process_d_req(d_req &req);
  void process_va_out_bidder(va_out_bidder &req);  

  // Proprietary logic
  uint32_t compute_bid(c_req &req);

 protected:
  net conn_;
  std::map<std::vector<uint8_t>, auction_context> auctions_ctx_;
};

#endif
