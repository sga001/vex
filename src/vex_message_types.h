#ifndef _VEX_MESSAGE_TYPES_H_
#define _VEX_MESSAGE_TYPES_H_

#include "message_types.h"
#include "vex_util.h"

enum proof_labels { LESS_THAN, EQUAL, GREATER_THAN };

struct va_req {

  // Vex information

  std::vector<uint8_t> id;
  std::vector<uint8_t> sig_id;

  // Impression information

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

  MSGPACK_DEFINE(id, sig_id, url, width, height, reserve_price, cookie_version,
  cookie_age_seconds, user_agent, geo_criteria_id, postal_code, timezone_offset,
  timestamp);
};


struct c_req {

  // Vex information

  std::vector<uint8_t> id;
  std::vector<uint8_t> sig_id;
  std::vector<uint8_t> seed;  // only used in vex_cp

  // Impression information

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

  MSGPACK_DEFINE(id, sig_id, seed, url, width, height, reserve_price, cookie_version,
  cookie_age_seconds, user_agent, geo_criteria_id, postal_code, timezone_offset,
  timestamp, ip, seller_network_id, detected_language);
};

struct vex_object {
  std::vector<uint8_t> c;
  std::vector<uint8_t> htag;

  bool operator==(const vex_object &rhs) const
  {
    if (c == rhs.c && htag == rhs.htag)
      return true;
    return false;
  }

  MSGPACK_DEFINE(c, htag);
};

struct vex_decommitment {
  std::vector<uint8_t> sprime;
  uint32_t bid;
  std::string ad_tag;

  MSGPACK_DEFINE(sprime, bid, ad_tag);
};

struct vex_proof {
  std::vector<uint8_t> proof;
  uint8_t label;

  MSGPACK_DEFINE(proof, label);
};


struct c_sub {
 std::vector<uint8_t> id;
 vex_object vo;

 MSGPACK_DEFINE(id, vo);
};

struct d_req {
  std::vector<uint8_t> id;
  std::vector<uint8_t> sig_vo;
  std::vector<vex_object> bundle;

  MSGPACK_DEFINE(id, sig_vo, bundle);
};

struct d_sub {
  std::vector<uint8_t> id;
  vex_decommitment vd;

  MSGPACK_DEFINE(id, vd);
};

struct va_out_bidder {
  std::vector<uint8_t> id;
  uint32_t sale_price;
  bool valid;

  MSGPACK_DEFINE(id, sale_price, valid);
};

struct va_out_seller {
  std::vector<uint8_t> id;
  uint32_t sale_price;
  int winner_idx;
  std::string ad_tag;
  bool valid;

  MSGPACK_DEFINE(id, sale_price, winner_idx, ad_tag, valid);
};

struct sign_req {
  std::vector<uint8_t> id;
  std::vector<vex_object> bundle;

  MSGPACK_DEFINE(id, bundle);
};

struct sign_sub {
  std::vector<uint8_t> id;
  std::vector<uint8_t> sig_vo;

  MSGPACK_DEFINE(id, sig_vo);
};

struct r_sub {
  std::vector<uint8_t> id;
  std::vector<uint8_t> sig_r;

  MSGPACK_DEFINE(id, sig_r);
};

struct prover_input {
  std::vector<vex_object> vos;
  std::vector<vex_decommitment> vds;
  uint32_t sale_price;
  int winner_idx;
};

struct prover_output {
  std::vector<vex_proof> proofs;
};

struct verifier_input {
  std::vector<vex_object> vos;
  std::vector<uint8_t> htag;
  uint32_t sale_price;
  int winner_idx;
};

#endif
