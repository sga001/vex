#ifndef _VEX_BIDDER_CP_SERVER_H_
#define _VEX_BIDDER_CP_SERVER_H_

#include "vex_bidder_server.h"

class vex_bidder_cp_server : public vex_bidder_server {
 public:
  vex_bidder_cp_server() {}
  ~vex_bidder_cp_server() {}

  virtual void process_c_req(c_req &req);
};

#endif
