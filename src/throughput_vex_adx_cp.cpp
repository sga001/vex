#include "throughput_vex_adx_cp.h"
#include "throughput_util.h"
#include "throughput_vex_generator.h"
#include <iostream>

c_sub
throughput_vex_adx_cp::gen_c_d_sub_salted(std::vector<uint8_t> id, d_sub *dec){
  c_sub commitment_msg;
  
  commitment_msg.id = id;
  vex_object vo;
  vex_decommitment vd;

  vd.bid = rand() % MAX_BID;
  vd.ad_tag = g_sample_ad;

  // Create vectors of the right size
  vo.c.resize(HASH_SIZE);
  vo.htag.resize(HASH_SIZE);
  vd.sprime.resize(SALT_SIZE);

  // Generate salt
  vex::random_bits(SALT_SIZE * 8, vd.sprime.data());
  
  int idx = vault_idx_ % cp_num_;
  vault_idx_++;
  std::vector<uint8_t> the_seed = checkpoint_vault_.at(idx).at(0);


  // Generate commitment to MAX_BID - bid and the ad tag
  vex::gen_salted_commit((MAX_BID - vd.bid) + 1, the_seed.data(), SEED_SIZE, 
    vd.sprime.data(), SALT_SIZE, vo.c.data());
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

void
throughput_vex_adx_cp::run_experiment(int iterations, int num_auctions, int num_bidders)
{
  
  for (int i = 0; i < iterations; ++i) {

    std::vector<strbuf> varequests;
    std::vector<strbuf> csubs;
    std::vector<strbuf> signsubs;
    std::vector<strbuf> dsubs;
    std::vector<strbuf> rsubs;
  
    auctions_ctx_.clear();
    vault_idx_ = 0;
    
    // Setup
    for (int j = 0; j < num_auctions; ++j) {
      va_req vareq = gen_vareq();
      
      for (int k = 0; k < num_bidders; ++k) {
        d_sub dsub;
        c_sub csub = gen_c_d_sub_salted(vareq.id, &dsub);   
        csubs.push_back(a_t_write(C_SUB, csub));
        dsubs.push_back(a_t_write(D_SUB, dsub));
      }

      d_req dreq; // needed for r_sub
      sign_sub ssub = gen_signsub(vareq.id, num_bidders, &dreq);
      r_sub rsub = gen_rsub(dreq, num_bidders);
      
      varequests.push_back(a_t_write(A_REQ, vareq));
      signsubs.push_back(a_t_write(SIGN_SUB, ssub));
      rsubs.push_back(a_t_write(R_SUB, rsub));
    }

    vault_idx_ = 0;

    // test
    long start_time = PAPI_get_virt_usec();

    for (int j = 0; j < num_auctions; ++j) {

      read_cb(0, varequests[j]);
      
      for (int k = 1; k <= num_bidders; ++k) 
        read_cb(k, csubs[j*num_bidders + (k-1)]);
      
      read_cb(0, signsubs[j]);

      for (int k = 1; k <= num_bidders; ++k) 
        read_cb(k, dsubs[(j * num_bidders + (k-1))]);
      
      read_cb(0, rsubs[j]);
    }

    long stop_time = PAPI_get_virt_usec();

    std::cout << "Time: "
              << ((double) stop_time - start_time) / (double) num_auctions
              << " microseconds.\n";

  }
}
