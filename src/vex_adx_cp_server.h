#ifndef _VEX_ADX_CP_SERVER_H_
#define _VEX_ADX_CP_SERVER_H_

#include "vex_adx_server.h"

typedef std::vector<std::vector<uint8_t> > checkpoints; 


// Treat s' in auction_context as salt instead of as a seed.

class vex_adx_cp_server : public vex_adx_server {
 public:
  vex_adx_cp_server() 
    : vex_adx_server(), checkpoint_vault_(), cp_(16), vault_idx_(0), cp_num_(1000) {}

  vex_adx_cp_server(int cp_num)
    : vex_adx_server(), checkpoint_vault_(), cp_(16), vault_idx_(0), cp_num_(cp_num) {}
 
  vex_adx_cp_server(int cp_num, int cp_rate)
    : vex_adx_server(), checkpoint_vault_(), cp_(cp_rate), vault_idx_(0), cp_num_(cp_num) {}
  
   ~vex_adx_cp_server() {}
  
  void init_checkpoint_vault();

  
  int find_closest_cp(uint32_t num);  
  
  uint32_t find_remaining(uint32_t num, int closest);
  
  virtual void process_va_req(int conn_idx, const va_req &req);

  virtual bool consistency_check(auction_context *ctx);

 protected:
  std::vector<checkpoints> checkpoint_vault_;
  int cp_;
  int vault_idx_;
  int cp_num_;
};

#endif
