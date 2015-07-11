#ifndef _TPUT_VEX_ADX_CP_H_
#define _TPUT_VEX_ADX_CP_H_

#include "vex_adx_cp_server.h"

class throughput_vex_adx_cp : public vex_adx_cp_server {
 public:
  throughput_vex_adx_cp() {}
  throughput_vex_adx_cp(int cp_num, int cp_rate) : vex_adx_cp_server(cp_num, cp_rate) {}

  ~throughput_vex_adx_cp() {}

  virtual void do_a_write(int conn_idx, strbuf send_data) {
    // Do nothing.
  }
  
  void fake_init_conn(int num) {
    conn_.reserve(num+1);
    for (int i = 0; i < num+1; ++i)
      conn_.push_back(New refcounted<net>());
  }

  c_sub gen_c_d_sub_salted(std::vector<uint8_t> id, d_sub *sub);

  void run_experiment(int iterations, int num_auctions, int num_bidders);
};

#endif
