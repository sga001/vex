#ifndef _SELLER_NET_H_
#define _SELLER_NET_H_

#include "net.h"
#include "util.h"
#include "message_types.h"
#include <map>
#include <papi.h> // for time measurements


// Macro that simplifies writing read_cb() functions
#define PROCESS_REQ(type_name) \
  do { \
    type_name req; \
    ret = parse_message(data, &req); \
    check_ret(ret, "parse_message()"); \
    process_##type_name(req); \
  } while (0);

struct auction_context {
  uint32_t id;
  std::string ad_tag;
  uint32_t price;
  long start_time;
  long end_time; 
};

class seller_server {
 public:
  seller_server(int aucts, double lambda)
    : conn_(), auctions_ctx_(), total_auctions_(aucts), sent_auctions_(0),
      lambda_(lambda) {
        if(PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
          fatal << "Wrong PAPI version\n";  
    }
 
  ~seller_server() {}

  // Starts the server
  void start_server(int port);
  
  //--------- Handle events -------//

  // Handle writing from msgpack structures
  
  template <typename T>
  void a_write(uint8_t type, const T &req) {
    msgpack::sbuffer buf;
    msgpack::pack(&buf, req);
  
    strbuf send_data;  
    send_data << str((char*)&type, 1) << str(buf.data(), buf.size());
    conn_.a_write(send_data);
  }
  
  // Read callback
  void read_cb(strbuf data);

  // Handle requests
  void process_a_out_seller(const a_out_seller &req);
  
  // Generate and send an auction
  void generate_auctions(void);

  // Next request
  double poisson_next_time(void);

 private:
  net conn_;
  std::map<uint32_t, auction_context> auctions_ctx_;
  int total_auctions_;
  int sent_auctions_;
  double lambda_;
};
#endif
