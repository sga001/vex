#ifndef _TPUT_VEX_ADX_H_
#define _TPUT_VEX_ADX_H_

#include "vex_adx_server.h"

class throughput_vex_adx : public vex_adx_server {
 public:
  throughput_vex_adx() {}
  ~throughput_vex_adx() {}

  virtual void do_a_write(int conn_idx, strbuf send_data) {
    // Do nothing.
  }
  
  void fake_init_conn(int num) {
    conn_.reserve(num+1);
    for (int i = 0; i < num+1; ++i)
      conn_.push_back(New refcounted<net>());
  }

  void run_experiment(int iterations, int num_auctions, int num_bidders);
};

#endif
