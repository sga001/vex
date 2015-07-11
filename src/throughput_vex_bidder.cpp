#include "throughput_vex_bidder.h"
#include "throughput_util.h"
#include "throughput_vex_generator.h"
#include <iostream>

void
throughput_vex_bidder::run_experiment(int iterations, int num_auctions, int num_bidders)
{
  
  for (int i = 0; i < iterations; ++i) {

    std::vector<strbuf> crequests;
    std::vector<strbuf> drequests;
    std::vector<strbuf> outcomes;
  
    auctions_ctx_.clear();

    for (int j = 0; j < num_auctions; ++j) {
      c_req creq = gen_creq();
      d_req dreq = gen_dreq(creq.id, num_bidders);
      va_out_bidder out;
      out.id = creq.id;
      out.sale_price = rand() % MAX_BID;

      crequests.push_back(a_t_write(C_REQ, creq));
      drequests.push_back(a_t_write(D_REQ, dreq));
      outcomes.push_back(a_t_write(A_OUT, out));
    }


    // test
    long start_time = PAPI_get_virt_usec();

    for (int j = 0; j < num_auctions; j++) {
      read_cb(crequests[j]);
      read_cb(drequests[j]);
      read_cb(outcomes[j]);
    }

    long stop_time = PAPI_get_virt_usec();

    std::cout << "Time: "
              << ((double) stop_time - start_time) / (double) num_auctions
              << " microseconds.\n";

  }
}
