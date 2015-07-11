#include "throughput_vex_adx.h"
#include "throughput_util.h"
#include "throughput_vex_generator.h"
#include <iostream>

void
throughput_vex_adx::run_experiment(int iterations, int num_auctions, int num_bidders)
{
  
  for (int i = 0; i < iterations; ++i) {

    std::vector<strbuf> varequests;
    std::vector<strbuf> csubs;
    std::vector<strbuf> signsubs;
    std::vector<strbuf> dsubs;
    std::vector<strbuf> rsubs;
  
    auctions_ctx_.clear();

    // Setup
    for (int j = 0; j < num_auctions; ++j) {
      va_req vareq = gen_vareq();
      d_sub dsub;
      c_sub csub = gen_c_d_sub(vareq.id, &dsub);
      d_req dreq; // needed for r_sub
      sign_sub ssub = gen_signsub(vareq.id, num_bidders, &dreq);
      r_sub rsub = gen_rsub(dreq, num_bidders);
      
      varequests.push_back(a_t_write(A_REQ, vareq));
      csubs.push_back(a_t_write(C_SUB, csub));
      signsubs.push_back(a_t_write(SIGN_SUB, ssub));
      dsubs.push_back(a_t_write(D_SUB, dsub));
      rsubs.push_back(a_t_write(R_SUB, rsub));
    }

    // test
    long start_time = PAPI_get_virt_usec();

    for (int j = 0; j < num_auctions; ++j) {

      read_cb(0, varequests[j]);
      
      for (int k = 1; k <= num_bidders; ++k) 
        read_cb(k, csubs[j]);
      
      read_cb(0, signsubs[j]);

      for (int k = 1; k <= num_bidders; ++k) 
        read_cb(k, dsubs[j]);
      
      read_cb(0, rsubs[j]);
    }

    long stop_time = PAPI_get_virt_usec();

    std::cout << "Time: "
              << ((double) stop_time - start_time) / (double) num_auctions
              << " microseconds.\n";

  }
}
