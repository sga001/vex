#ifndef _TPUT_VEX_CP_BIDDER_H_
#define _TPUT_VEX_CP_BIDDER_H_

#include "vex_bidder_cp_server.h"

class throughput_vex_bidder_cp : public vex_bidder_cp_server {
 public:
  throughput_vex_bidder_cp() {}
  ~throughput_vex_bidder_cp() {}

  virtual void do_a_write(strbuf data) {
    // Do nothing.
  }

  void run_experiment(int iterations, int num_auctions, int num_bidders);
};

#endif
