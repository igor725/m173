#pragma once

#include <exception>
#include <format>
#include <sockpp/tcp_socket.h>

class CreateReader {
  public:
  class UnknownPacketException: public std::exception {
public:
    UnknownPacketException(int8_t id) { m_what = std::format("Unknown packet id {:02x}", id); }

    const char* what() const noexcept override { return m_what.c_str(); }

private:
    std::string m_what;
  };

  CreateReader(sockpp::tcp_socket& sock, sockpp::inet_address& addr);

  private:
  static void ThreadLoop(sockpp::tcp_socket sock, sockpp::inet_address addr);
};
