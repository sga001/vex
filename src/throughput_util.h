#ifndef _TPUT_UTIL_
#define _TPUT_UTIL_

#include "util.h"
#include <papi.h>

#define PROCESS_T_REQ(req) \
  do { \
    uint8_t type; \
    int ret; \
    type = get_type(data); \
    ret = parse_message(data, &req); \
    check_ret(ret, "parse_message()"); \
  } while (0);


// Handle writing from msgpack structures
template <typename T>
strbuf a_t_write(uint8_t type, const T &req)
{
  msgpack::sbuffer buf;
  msgpack::pack(&buf, req);
  
  strbuf send_data;
  send_data << str((char*) &type, 1) << str(buf.data(), buf.size());
  return send_data;
}


static a_req 
generate_a_req(int i)
{
  a_req req;
  req.id = i;
  get_page_data(&req);
  get_user_data(&req);
  req.timestamp = PAPI_get_real_usec();
  return req;
}

static b_req
generate_b_req(int i)
{
  b_req req;
  req.id = i;
  get_page_data((a_req*) &req);
  get_user_data((a_req*) &req);
  req.timestamp = PAPI_get_real_usec();

  req.ip = rand();
  req.seller_network_id = rand();
  req.detected_language = rand();
  return req;
}

static b_sub
generate_bid(int i)
{
  b_sub res;
  int len = rand() % 200;
  char buf[len];
  
  res.id = i;
  res.bid = rand() % MAX_BID;
  res.ad_tag = g_sample_ad;
  
  res.width = rand() % 2000;
  res.height = rand() % 2000;
  
  gen_random_string(buf, len);
  res.result_url = std::string(buf);
 
  return res;
}

#endif
