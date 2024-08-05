#pragma once

#include <mutex>
#include <sockpp/tcp_socket.h>

class SafeSocket {
  public:
  SafeSocket(sockpp::tcp_socket&& sock, sockpp::inet_address&& addr): m_sock(sock), m_addr(addr) {}

  bool write(const void* data, size_t dsize) {
    std::unique_lock lock(m_lock);
    return m_sock.write(data, dsize) == dsize;
  }

  bool read(void* data, size_t dszie) {
    std::unique_lock lock(m_lock);
    return m_sock.read(data, dszie);
  }

  private:
  sockpp::tcp_socket&   m_sock;
  sockpp::inet_address& m_addr;
  std::mutex            m_lock;
};
