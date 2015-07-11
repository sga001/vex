#ifndef _VEX_BIDDER_SERVER_H_
#define _VEX_BIDDER_SERVER_H_

#include "net.h"
#include "vex_util.h"
#include "vex_message_types.h"
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
  std::vector<uint8_t> id;
  std::vector<vex_object> bundle;
  int winner_idx;
  uint32_t sale_price;
  std::string ad_tag;

  // measuremnet informantion
  long start_time;
  long end_time;
};

class vex_seller_server {
 public:
  vex_seller_server(int aucts, double lambda)
    : conn_(), auctions_ctx_(), total_auctions_(aucts), sent_auctions_(0),
      lambda_(lambda), priv_key_(bigint(g_p_hex, BASE_16), 
      bigint(g_q_hex, BASE_16), ESIGN_E), last_id_() {

        if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
          fatal << "Wrong PAPI version\n";
    }

  ~vex_seller_server(){}

  // Starts the server
  void start_server(int port);

  //------- Handle Events-------//

  // Handle writing from msgpack structures

  template <typename T>
  void a_write(uint8_t type, const T &req) {
    msgpack::sbuffer buf;
    msgpack::pack(&buf, req);

    strbuf send_data;
    send_data << str((char*) &type, 1) << str(buf.data(), buf.size());
    do_a_write(send_data);
  }

  virtual void do_a_write(strbuf data) {
    conn_.a_write(data);
  } 

  // Read callback
  void read_cb(strbuf data);

  // Handle requests
  void process_sign_req(const sign_req &req);
  void process_va_out_seller(const va_out_seller &req);

  // Schedule auction
  void schedule_auctions(void);

  // Generate and send an auction
  void generate_auctions(void);
  
  // Next request
  double poisson_next_time(void);

 protected:
  net conn_;
  std::map<std::vector<uint8_t>, auction_context> auctions_ctx_;
  int total_auctions_;
  int sent_auctions_;
  double lambda_;
  esign_priv priv_key_;
  std::vector<uint8_t> last_id_; // just to know when to print stuff
};

// Utility functions

static void
get_page_data(va_req *req)
{
  assert(req);

  int len = rand() % 100;
  char buf[len];
  gen_random_string(buf, len);

  req->url = std::string(buf);
  req->width = rand() % 1000;
  req->height = rand() % 1000;
}

static void
get_user_data(va_req *req)
{
  assert(req);

  int len = rand() % 100;
  char buf[len];
  gen_random_string(buf, len);

  req->cookie_version = rand();
  req->cookie_age_seconds = rand();
  req->cookie_id = rand();

  req->user_agent = std::string(buf);
  req->geo_criteria_id = rand() % 20;
  req->postal_code = rand() % 99999;
  req->timezone_offset = rand() % 24;
}

#endif
