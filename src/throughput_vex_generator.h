#ifndef _TPUT_VEX_GENERATOR_
#define _TPUT_VEX_GENERATOR_

#include "vex_util.h"
#include <papi.h>
#include "vex_message_types.h"
#include <vector>
#include "vex_prover.h"

static esign_priv priv_key = get_private_key();


static void
get_p_data(va_req *req)
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
get_u_data(va_req *req)
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

static va_req
gen_vareq()
{
  va_req req;
  req.id.resize(HASH_SIZE);
  get_p_data(&req);
  get_u_data(&req);

  req.timestamp = PAPI_get_real_usec();

  msgpack::sbuffer buf;
  msgpack::pack(&buf, req.url);
  msgpack::pack(&buf, req.timestamp);

  // Get id = H(url || timestamp) (for now)
  HASH(buf.data(), buf.size(), req.id.data());

  // Sign the id
  bigint sig_id_int = priv_key.sign(str((char*) req.id.data(), req.id.size()));
  get_raw_bigint(sig_id_int, &req.sig_id);
  return req;
}

static c_req
gen_creq()
{

  va_req req = gen_vareq();

  c_req creq;
  creq.id = req.id;
  creq.sig_id = req.sig_id;
  creq.url = req.url;
  creq.width = req.width;
  creq.height = req.height;
  creq.reserve_price = req.reserve_price;
  creq.cookie_version = req.cookie_version;
  creq.cookie_age_seconds = req.cookie_age_seconds;
  creq.cookie_id = req.cookie_id;
  creq.user_agent = req.user_agent;
  creq.geo_criteria_id = req.geo_criteria_id;
  creq.postal_code = req.postal_code;
  creq.timezone_offset = req.timezone_offset;
  creq.timestamp = req.timestamp;

  creq.ip = rand();
  creq.seller_network_id = rand();
  creq.detected_language = rand();

  return creq;
}


static c_sub
gen_c_d_sub(std::vector<uint8_t> id, d_sub *dec, uint32_t max_price = 10000, bool set = false)
{
  c_sub commitment_msg; // vex commitment message

  commitment_msg.id = id;
  vex_object vo;
  vex_decommitment vd;
  
  if (set)
    vd.bid = max_price;
  else
    vd.bid = rand() % max_price;
  
  vd.ad_tag = g_sample_ad;

  // Create vectors of the right size
  vo.c.resize(HASH_SIZE);
  vo.htag.resize(HASH_SIZE);
  vd.sprime.resize(SEED_SIZE);
  uint8_t s[HASH_SIZE];

  // Generate seeds (s' and s)
  vex::setup(HASH_SIZE, g_prefix, vd.sprime.data(), s);

  // Generate commitment to MAX_BID - bid and the ad tag
  vex::gen_commit(MAX_BID - vd.bid, s, HASH_SIZE, vo.c.data());
  vex::gen_commit(1, (uint8_t*) vd.ad_tag.c_str(), vd.ad_tag.size(),
    vo.htag.data());

  // Add commitment and htag to the response
  commitment_msg.vo = vo;

  if (dec) {
    dec->id = id;
    dec->vd = vd;
  }

  return commitment_msg;
}

static sign_req
gen_signreq(std::vector<uint8_t> id, int bidders)
{
  sign_req sreq;
  sreq.id = id;

  for (int i = 0; i < bidders; ++i) {
    sreq.bundle.push_back(gen_c_d_sub(id, NULL).vo);
  }
  return sreq;
}



static void
gen_auction_outcome(prover_input *pi)
{
  uint32_t max_bid = 0;
  uint32_t sp = 0;
  uint32_t c_bid = 0;
  int max_bidder = 1;

  for (int i = 0; i < (int) pi->vds.size(); ++i) {

    c_bid = pi->vds[i].bid;

    if (max_bid < c_bid) {
      sp = max_bid;

      max_bid = c_bid;
      max_bidder = i;
    } else if (sp < c_bid) {
      sp = c_bid;
     }
  }

  pi->sale_price = sp;
  pi->winner_idx = max_bidder;
}

static prover_input
gen_prover_input(std::vector<uint8_t> id, int bidders, uint32_t max_price = MAX_BID)
{
  prover_input pi;

  for (int i = 2; i < bidders; ++i) {
    d_sub d3;
    pi.vos.push_back(gen_c_d_sub(id, &d3, max_price).vo);
    pi.vds.push_back(d3.vd);
  }
  
  d_sub d1;
  pi.vos.push_back(gen_c_d_sub(id, &d1, max_price, true).vo);
  pi.vds.push_back(d1.vd);
 
  d_sub d2;
  pi.vos.push_back(gen_c_d_sub(id, &d2, max_price, true).vo);
  pi.vds.push_back(d2.vd);

  gen_auction_outcome(&pi);

  return pi;
}

static sign_sub
gen_signsub(std::vector<uint8_t> id, int bidders, d_req *dec)
{
  sign_req req = gen_signreq(id, bidders);
  sign_sub sub;

  sub.id = req.id;

  // Get data of all vex objects
  msgpack::sbuffer buf;
  msgpack::pack(&buf, req.id);
  msgpack::pack(&buf, req.bundle);

  str data = str(buf.data(), buf.size());

  // Sign the id || bundle
  bigint sig_vo_int = priv_key.sign(data);
  get_raw_bigint(sig_vo_int, &sub.sig_vo);

  if (dec) {
    dec->id = req.id;
    dec->bundle = req.bundle;
    dec->sig_vo = sub.sig_vo;
  }

  return sub;
}

static d_req
gen_dreq(std::vector<uint8_t> id, int bidders)
{
  d_req req;
  gen_signsub(id, bidders, &req);
  return req;
}


static r_sub
gen_rsub(d_req &dreq, int num_bidders) {

  r_sub sub;

  int winner_idx = (rand() % num_bidders) + 1;
  uint32_t sale_price = rand() % MAX_BID;

  sub.id = dreq.id;

  // Sign id || winner_idx || sale_price || htag of winner

  msgpack::sbuffer buf;
  msgpack::pack(&buf, dreq.id);
  msgpack::pack(&buf, winner_idx);
  msgpack::pack(&buf, sale_price);
  msgpack::pack(&buf, dreq.bundle[(winner_idx-1)].htag);

  str data = str(buf.data(), buf.size());

  // Do the actual signing
  bigint sig_r_int = priv_key.sign(data);
  get_raw_bigint(sig_r_int, &sub.sig_r);
  return sub;
}

#endif
