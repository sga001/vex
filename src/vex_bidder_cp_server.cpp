#include "vex_bidder_cp_server.h"

void
vex_bidder_cp_server::process_c_req(c_req &req)
{
  c_sub commitment_msg; // vex commitment message
  auction_context ctx; // auction context

  commitment_msg.id = req.id;
  ctx.id = req.id;

  ctx.sig_id = req.sig_id;
  ctx.vd.bid = compute_bid(req);
  ctx.vd.ad_tag = g_sample_ad;

  // Create vectors of the right size
  ctx.vo.c.resize(HASH_SIZE);
  ctx.vo.htag.resize(HASH_SIZE);
  ctx.vd.sprime.resize(SALT_SIZE); // SALT, not a seed anymore

  // Generate a salt
  vex::random_bits(SALT_SIZE * 8, ctx.vd.sprime.data());

  // Generate salted commitment to (MAX_BID - bid) + 1 and the ad tag
  // The +1 is because we are starting from s' and not from s.
  vex::gen_salted_commit((MAX_BID - ctx.vd.bid) + 1, req.seed.data(), SEED_SIZE, 
    ctx.vd.sprime.data(), SALT_SIZE, ctx.vo.c.data());
  vex::gen_commit(1, (uint8_t*) ctx.vd.ad_tag.c_str(), ctx.vd.ad_tag.size(),
    ctx.vo.htag.data());

  // Add commitment and htag to the response
  commitment_msg.vo = ctx.vo;

  // Add new context to the list
  auctions_ctx_.insert(std::pair<std::vector<uint8_t>, auction_context>(
                    ctx.id, ctx));

  // Send the commitment
  a_write(C_SUB, commitment_msg);
}
