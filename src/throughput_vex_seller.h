#ifndef _TPUT_VEX_SELLER_H_
#define _TPUT_VEX_SELLER_H_

#include "vex_seller_server.h"

class throughput_vex_seller : public vex_seller_server {
 public:
  throughput_vex_seller(int aucts, double lambda) : vex_seller_server(aucts, lambda){}
  ~throughput_vex_seller() {}

  virtual void do_a_write(strbuf data) {
    // Do nothing.
  }

  void run_experiment(int iterations, int num_auctions, int num_bidders);
};

#endif
