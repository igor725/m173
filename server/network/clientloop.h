#pragma once

#include <sockpp/tcp_socket.h>

class ClientLoop {
  public:
  ClientLoop(sockpp::tcp_socket& sock, sockpp::inet_address& addr);

  private:
  static void ThreadLoop(sockpp::tcp_socket sock, sockpp::inet_address addr, uint64_t ref);
};
