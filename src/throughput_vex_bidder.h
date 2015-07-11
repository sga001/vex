#ifndef _TPUT_VEX_BIDDER_H_
#define _TPUT_VEX_BIDDER_H_

#include "vex_bidder_server.h"

class throughput_vex_bidder : public vex_bidder_server {
 public:
  throughput_vex_bidder() {}
  ~throughput_vex_bidder() {}

  virtual void do_a_write(strbuf data) {
    // Do nothing.
  }

  void run_experiment(int iterations, int num_auctions, int num_bidders);
};

#endif
