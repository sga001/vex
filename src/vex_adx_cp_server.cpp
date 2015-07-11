#include "vex_adx_cp_server.h"

int
vex_adx_cp_server::find_closest_cp(uint32_t num)
{
  int closest = 0;

  for (int i = 0; i < cp_; ++i) {
    if ((uint32_t) (MAX_BID/cp_) * i <= num)
      closest = i;
    else
      break;
  }
  return closest;
}

uint32_t 
vex_adx_cp_server::find_remaining(uint32_t num, int closest)
{
  uint32_t remaining = num - (closest * (MAX_BID/cp_));

  if (closest == 0)
    remaining++;

  return remaining;
}

void
vex_adx_cp_server::init_checkpoint_vault()
{
  for (int i = 0; i < cp_num_; ++i) {
    std::vector<uint8_t> seed;
    std::vector<uint8_t> c; // 1 checkpoint
    checkpoints cps;        // list of checkpoints

    seed.resize(SEED_SIZE);
    c.resize(HASH_SIZE);
    cps.reserve(cp_);

    vex::setup(HASH_SIZE, g_prefix, seed.data(), c.data()); 
    cps.push_back(seed); // idx 0 = seed

    for (int j = 0; j < cp_-1; ++j) {
      vex::gen_commit((MAX_BID / cp_), c.data(), HASH_SIZE, c.data());
      cps.push_back(c); // add checkpoint to list
    }
    // add list to vault
    checkpoint_vault_.push_back(cps);
  }
}

void 
vex_adx_cp_server::process_va_req(int conn_idx, const va_req &req)
{
  // c_req needs to include seed (potentially 1 cp)
  c_req creq;

  // Create auction context
  auction_context ctx;
  ctx.id = req.id;
  ctx.sig_id = req.sig_id;
  ctx.seed_base_idx = vault_idx_;
  auctions_ctx_.insert(std::pair<std::vector<uint8_t>, auction_context>(req.id, ctx));

  vault_idx_+= conn_.size()-1; // increment the vault idx counter 

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

  for (int i = 1; i < (int) conn_.size(); ++i) {
    creq.seed = checkpoint_vault_.at((ctx.seed_base_idx + (i-1)) % cp_num_).at(0);
    a_write(i, C_REQ, creq);
  }

}

bool 
vex_adx_cp_server::consistency_check(auction_context *ctx)
{
  uint8_t htag[HASH_SIZE];
  bool ret = 1;
  int seed_idx = ctx->seed_base_idx;

  for (int i = 1; i <= (int) ctx->vos.size(); ++i){

    // Get checkpoint closest checkpoint
    vex_decommitment *dec = &(ctx->vds.at(i));
    vex_object *com = &(ctx->vos.at(i));
   
    HASH((uint8_t*) dec->ad_tag.c_str(), dec->ad_tag.size(), htag);

    int closest = find_closest_cp(MAX_BID - dec->bid);
    uint32_t remaining = find_remaining(MAX_BID - dec->bid, closest);
    std::vector<uint8_t> closest_cp = checkpoint_vault_.at(seed_idx % cp_num_).at(closest);

    // treat dec->sprime as if it were a salt instead of a seed.

    if (!vex::verify_salted_proof(remaining, com->c.data(), com->c.size(),
        closest_cp.data(), closest_cp.size(), dec->sprime.data(), dec->sprime.size()) ||
        memcmp(com->htag.data(), htag, HASH_SIZE) != 0) {

        ctx->invalid.insert(std::pair<int, bool>(i, 1)); // 1 implies wrong
        ret = 0;
    }

    seed_idx++;
  }

  return ret; 
}
