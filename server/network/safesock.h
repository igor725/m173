#pragma once

#include <mutex>
#include <sockpp/tcp_socket.h>
#include <string>
#include <vector>

class SafeSocket {
  public:
  SafeSocket(sockpp::tcp_socket&& sock, sockpp::inet_address&& addr): m_sock(sock), m_addr(addr), m_closed(false) {
    m_sock.write_timeout(std::chrono::milliseconds(100));
  }

  bool isClosed() const { return m_closed; }

  bool write(const void* data, size_t dsize);

  bool read(void* data, size_t dszie);

  void pushQueue();

  bool isLocal() const;

  std::string addr();

  void close();

  private:
  struct subnet {
    unsigned long net;
    unsigned long mask;
  };

  std::recursive_mutex        m_rlock;
  std::recursive_mutex        m_wlock;
  std::vector<char>           m_queue;
  sockpp::tcp_socket&         m_sock;
  const sockpp::inet_address& m_addr;
  bool                        m_closed = false;
  int32_t                     m_fails  = 0;
};
