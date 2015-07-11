#ifndef _VEX_CP_ADX_SERVER_H_
#define _VEX_CP_ADX_SERVER_H_

#include "vex_adx_server.h"

typedef std::vector<std::vector<uint8_t> > checkpoint_list;

class vex_cp_adx_server : public vex_adx_server {
 public:
  vex_cp_adx_server() : vex_adx_server(), checkpoint_vault_() {}
  ~vex_cp_adx_server() {}

 private:
  std::map<std::vector<uint8_t>, checkpoint_list> checkpoint_vault_;

};


#endif
