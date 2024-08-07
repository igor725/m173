#pragma once

#include <mutex>
#include <sockpp/tcp_socket.h>
#include <vector>

class SafeSocket {
  public:
  SafeSocket(sockpp::tcp_socket&& sock, sockpp::inet_address&& addr): m_sock(sock), m_addr(addr), m_closed(false) {}

  bool write(const void* data, size_t dsize) {
    std::unique_lock lock(m_wlock);
    if (m_queue.size() > 1024 * 1024) return false;
    m_queue.insert(m_queue.end(), static_cast<const char*>(data), static_cast<const char*>(data) + dsize);
    return true;
  }

  bool read(void* data, size_t dszie) {
    if (m_closed) return false;
    std::unique_lock lock(m_rlock);
    return m_sock.read(data, dszie);
  }

  void pushQueue() {
    std::unique_lock lock(m_rlock);

    if (m_closed) {
      m_queue.clear();
      return;
    }

    while (m_queue.size() > 0) {
      auto sent = m_sock.write(m_queue.data(), m_queue.size());
      if (sent > 0) m_queue.erase(m_queue.begin(), m_queue.begin() + sent);
    }
  }

  void close() {
    m_sock.close();
    m_closed = true;
  }

  bool isClosed() const { return m_closed; }

  private:
  std::recursive_mutex  m_rlock;
  std::recursive_mutex  m_wlock;
  std::vector<char>     m_queue;
  sockpp::tcp_socket&   m_sock;
  sockpp::inet_address& m_addr;
  bool                  m_closed;
};
