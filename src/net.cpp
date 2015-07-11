#include "net.h"
#include "util.h"
#include <iostream>


// Make a new TCP connection (this blocks)
void
net::tcp_connect(str host, int port) 
{
	int ret;
  int flag;
  struct sockaddr_in sin;

	// Get socket
	fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	check_ret(fd_, "socket()");

  // Disable Nagle buffering algorithm
  tcp_nodelay(fd_);

	// Zero structure
	memset(&sin, 0, sizeof(sin));

	// Set to IPV4
	sin.sin_family = AF_INET;
	
	// Convert address
	ret = inet_pton(AF_INET, host.cstr(), &sin.sin_addr.s_addr);
	check_ret(ret, "inet_pton()");

	// Set port
	sin.sin_port = htons(port);
	
	// Establish connection to the server
	ret = connect(fd_, (struct sockaddr*) &sin, sizeof(sin));
	check_ret(ret, "connect()");	

  // Make fd asynchronous
  make_async(fd_); 

	// We are done
}

// Listen for connections
void
net::tcp_listen(int port, cbsb cb)
{
  int ret;
  int tmp_socket;

  // Init socket bound to port
  tmp_socket = inetsocket(SOCK_STREAM, port);
  check_ret(tmp_socket, "inetsocket()");
  
  // Make fd asynchronous
  make_async(tmp_socket);
  
  // Listen for a connection
  ret = listen(tmp_socket, LISTEN_BACKLOG);
  check_ret(ret, "listen()");

  // Schedule event to accept incoming connection 
  fdcb(tmp_socket, selread, wrap(this, &net::tcp_accept, tmp_socket, cb));
}

// Accept an incoming TCP connection
void
net::tcp_accept(int tmp_socket, cbsb cb)
{
  struct sockaddr_in sin;
  uint32_t sinlen = sizeof(sin); 
 
  fd_ = accept(tmp_socket, (struct sockaddr*) &sin, &sinlen);
  
  tcp_nodelay(fd_);

  if (fd_ < 0 && errno != EAGAIN)
    fatal << "accept() failed. errno = " << errno << "\n";
  
  a_read(cb);
}

// close connection
void
net::close_net()
{
  fdcb(fd_, selread, 0);
  fdcb(fd_, selwrite, 0);
  close(fd_);
}

// Asynchronous read
void 
net::a_read(cbsb cb) 
{
	fdcb(fd_, selread, wrap(this, &net::do_read, cb));
}

// Asynchronous write
void 
net::a_write(str data) 
{
  write_queue_.push_back(data);
   
  if (write_queue_.size() == 1)
    fdcb(fd_, selwrite, wrap(this, &net::do_write));

}

// If you wish to stop triggering read_cb whenever you
// get an incoming message, just call this function.
void
net::disable_read()
{
  fdcb(fd_, selread, 0);
}

// Default read callback
void 
net::read_cb(strbuf data)
{
	std::cout << str(data) << "\n";
}

//--------------- Utility Functions --------------//

void
net::do_write() 
{

  if (write_queue_.empty()) {
	  fdcb(fd_, selwrite, 0);
    return;
  }

  strbuf data = write_queue_.pop_front();
  
  uint32_t size = data.len();
  int res = write(fd_, (void*) &size, sizeof(uint32_t));

  if (res != sizeof(uint32_t))
    fatal << "write failed()";

  res = data.tosuio()->output(fd_);
  check_ret(res, "write()");

	// Schedule write again.
	if (data.tosuio()->resid()){
    //warn << "Scheduling write again\n";
	  fdcb(fd_, selwrite, wrap(this, &net::do_write_partial, data));
  	return;
  }
}

// This is only invoked if only a fraction of the message was sent.
void
net::do_write_partial(strbuf data)
{
  int res = data.tosuio()->output(fd_);
  check_ret(res, "write()");

  // Schedule partial write again.
  if (data.tosuio()->resid())
    return;
  
  // Go back to normal write.
  fdcb(fd_, selwrite, wrap(this, &net::do_write));
}

void
net::do_read(cbsb cb) 
{
	strbuf data;
  int size;
  int res;

  res = read(fd_, (void*) &size, sizeof(int));

  if (res != sizeof(int)) {
    if (res == 0 || errno == ECONNRESET) { 
      // Connection was closed. Disable reading.
      warn << "Connection has been closed\n";
      fdcb(fd_, selread, 0);
      exit(0);
      //return;
    } else if (errno == EAGAIN) { // Just ignore.
      return;
    } else {
      //TODO: Handle case where only a fraction of first 4 bytes is read.
      fatal << "read() failed. errno = " << errno << "\n";
    }
  }

  res = data.tosuio()->input(fd_, size);
 
  if (res != size) {
    if (res == 0 || errno == ECONNRESET) {
      warn << "Connection has been closed\n";
      fdcb(fd_, selread, 0);
      return;
    } else if (res < 0) {
      if (errno == EAGAIN) {
        //warn << "Reading partial message\n";
        fdcb(fd_, selread, wrap(this, &net::do_read_partial, cb, data, size));
        return;
      }

      fatal << "input() failed. errno: " << errno <<"\n";
    } else { // res is > and  < size
      // partial message received.
      //warn << "Reading partial message\n";
      fdcb(fd_, selread, wrap(this, &net::do_read_partial, cb, data, size - res));
      return; 
    }
  }

  // Call function
  (*cb)(data);  
}

// This is called if only part of a message was previously read.
void
net::do_read_partial(cbsb cb, strbuf data, int remaining)
{
  int res;
  res = data.tosuio()->input(fd_, remaining);

  if (res != remaining) {
    if (res == 0 || errno == ECONNRESET) {
      warn << "Connection has been closed\n";
      fdcb(fd_, selread, 0);
      exit(0);
      //return;
    } else if (res < 0) {
      fatal << "input() failed\n";
    } else { // res is > and  < remaining
      // partial message received.
      fdcb(fd_, selread, wrap(this, &net::do_read_partial, cb, data, remaining - res));
      return; 
    }
  }

  fdcb(fd_, selread, wrap(this, &net::do_read, cb));
  // Call function
  (*cb)(data);  
}
