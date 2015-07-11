#ifndef _BIDDER_NET_H_
#define _BIDDER_NET_H_

#include "net.h"
#include "util.h"
#include "message_types.h"
#include <map>
#include <stdlib.h>

// Macro that simplifies writing read_cb() functions
#define PROCESS_REQ(type_name) \
  do { \
    type_name req; \
    ret = parse_message(data, &req); \
    check_ret(ret, "parse_message()"); \
    process_##type_name(req); \
  } while (0);


struct auction_context {
  uint32_t id;    // auction id
  uint32_t bid;   // bid submitted
  std::string ad; // ad used
  uint32_t outcome; // if >0, then winner.
};


class bidder_server {
 public:
  bidder_server() : conn_(), auctions_ctx_() {}

  ~bidder_server() {}

  // Starts the server
  void start_server(int port);
  
  //--------- Handle events -------//

  // Handle writing from msgpack structures
  
  template <typename T>
  void a_write(uint8_t type, const T &req) {
    msgpack::sbuffer buf;
    msgpack::pack(&buf, req);
  
    strbuf send_data;  
    send_data << str((char*) &type, 1) << str(buf.data(), buf.size());
    conn_.a_write(send_data);
  }
  
  // Read callback
  void read_cb(strbuf data);

  // Handle requests
  void process_b_req(const b_req &req);
  void process_a_out_bidder(const a_out_bidder &req);
  
  // Proprietary logic
  uint32_t compute_bid(const b_req &req);

 private:
  net conn_;
  std::map<uint32_t, auction_context> auctions_ctx_;
};

#endif
