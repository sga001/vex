#ifndef _NET_H_
#define _NET_H_

#include "sfslite/async.h"

#define LISTEN_BACKLOG 5

typedef callback<void, strbuf>::ref cbsb;

class net {

 public:
	net() : fd_(0), write_queue_() {}	
	~net(){}

  // Functions to manage the connection
	void tcp_connect(str host, int port); 
  void tcp_listen(int port, cbsb cb); // this calls tcp_accept
  void tcp_accept(int tmp_socket, cbsb cb);

  // Functions to write and read asyncrhonously
	void a_write(str data);
	void a_read(cbsb cb);
  void do_write();
  void do_read(cbsb cb);
  void do_read_partial(cbsb cb, strbuf data, int remaining);
  void do_write_partial(strbuf data);
  void disable_read(void);
  void close_net(void);
  
  // Default read callback (just prints the data)
  void read_cb(strbuf data);

 private:
	int fd_;
  vec<str> write_queue_;
};

#endif
