// Message types
// Based on Doubleclick's RTB protocol, but heavily simpified.
// Extended to include VEX's data types.

#ifndef _MESSAGE_TYPES_H_
#define _MESSAGE_TYPES_H_
 
#include <vector>
#include <msgpack.hpp>
#include "util.h"

// Base Request types
enum base_type { START, A_REQ, B_REQ, B_SUB, A_OUT };

// Vex Request types
enum vex_type { C_REQ = 5, C_SUB, SIGN_REQ, SIGN_SUB, D_REQ, D_SUB, R_SUB };

// Parsing function
template <typename T>
int parse_message(str data, T* obj)
{
  
  const char* raw_data = data.cstr();
  size_t len = data.len() - 1;
  
  if (len > 0) { // if there is data in addition to the type 
    try {
      msgpack::unpacked buf;
      msgpack::unpack(&buf, raw_data + 1, len);
      *obj = buf.get().as<T>();
    } catch (msgpack::type_error e) {
      return -1;
    }
  }

  return len;
}

static inline uint8_t get_type(str data) {
  return (uint8_t) data[0];
}

struct a_req {
  uint32_t id;

  std::string url;
  int width;
  int height;

  uint32_t reserve_price;

  uint32_t cookie_version;
  int cookie_age_seconds;
  uint32_t cookie_id;

  std::string user_agent;
  int geo_criteria_id;
  int postal_code;
  int timezone_offset;

  double timestamp;
  
  MSGPACK_DEFINE(id, url, width, height, reserve_price, cookie_version, 
  cookie_age_seconds, cookie_id, user_agent, geo_criteria_id, postal_code, 
  timezone_offset, timestamp);
};

struct b_req {
  uint32_t id;

  // Same as auction request
  std::string url;
  int width;
  int height;

  uint32_t reserve_price;

  uint32_t cookie_version;
  int cookie_age_seconds;
  uint32_t cookie_id;

  std::string user_agent;
  int geo_criteria_id;
  int postal_code;
  int timezone_offset;
  double timestamp;
  
  // Additional info added by AdX
  uint32_t ip;
  int seller_network_id;
  int detected_language;   
 
  MSGPACK_DEFINE(id, url, width, height, reserve_price, cookie_version, 
  cookie_age_seconds, user_agent, geo_criteria_id, postal_code, timezone_offset, 
  timestamp, ip, seller_network_id, detected_language);
};

struct b_sub {
  uint32_t id;
  uint32_t bid;           // max_cpm

  std::string ad_tag;   // html/javascript snippet
  int width;
  int height;
  
  std::string result_url;
  
  MSGPACK_DEFINE(id, bid, ad_tag, width, height, result_url);
};

struct a_out_bidder {
  uint32_t id;
  uint32_t price;

  MSGPACK_DEFINE(id, price);
};

struct a_out_seller {
  uint32_t id;
  uint32_t price;

  std::string ad_tag;
  std::string result_url;

  int width;
  int height; 

  MSGPACK_DEFINE(id, price, ad_tag, result_url, width, height);
};



#endif
