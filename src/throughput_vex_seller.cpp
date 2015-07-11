#include "throughput_vex_seller.h"
#include "throughput_util.h"
#include "throughput_vex_generator.h"
#include <iostream>

void
throughput_vex_seller::run_experiment(int iterations, int num_auctions, int num_bidders)
{
  
  for (int i = 0; i < iterations; ++i) {

    std::vector<strbuf> requests;
    std::vector<strbuf> outcomes;

    auctions_ctx_.clear();

    // Auction generation
    long start_gen_time = PAPI_get_virt_usec();
    for (int j = 0; j < num_auctions; ++j) {
      generate_auctions();
    }
    long stop_gen_time = PAPI_get_virt_usec();

    // Setup requests and outcomes

   for (auto it = auctions_ctx_.begin(); it != auctions_ctx_.end(); ++it) {

      sign_req sreq = gen_signreq(it->first, num_bidders);
      va_out_seller out;
      out.id = it->first;
      out.sale_price = rand() % MAX_BID;
      out.winner_idx = (rand() % num_bidders) + 1;
      out.ad_tag = g_sample_ad;

      requests.push_back(a_t_write(SIGN_REQ, sreq));
      outcomes.push_back(a_t_write(A_OUT, out));
   }

    // test
    long start_time = PAPI_get_virt_usec();

    for (int j = 0; j < num_auctions; j++) {
      read_cb(requests[j]);
      read_cb(outcomes[j]);
    }

    long stop_time = PAPI_get_virt_usec();

    long total_time = (stop_time - start_time) + (stop_gen_time - start_gen_time);

    std::cout << "Time: "
              << ((double) total_time) / (double) num_auctions
              << " microseconds.\n";

  }
}
