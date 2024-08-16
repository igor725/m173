#include "safesock.h"

bool SafeSocket::write(const void* data, size_t dsize) {
  std::unique_lock lock(m_wlock);
  if (m_queue.size() > 1024 * 1024) return false;
  m_queue.insert(m_queue.end(), static_cast<const char*>(data), static_cast<const char*>(data) + dsize);
  return true;
}

bool SafeSocket::read(void* data, size_t dsize) {
  if (m_closed) return false;
  std::unique_lock lock(m_rlock);
  return m_sock.read(data, dsize) == dsize;
}

bool SafeSocket::pushQueue() {
  std::unique_lock lock(m_wlock);

  if (m_closed) {
    m_queue.clear();
    return false;
  }

  while (!m_closed && m_queue.size() > 0) {
    auto sent = m_sock.write(m_queue.data(), m_queue.size());
    if (sent > 0)
      m_queue.erase(m_queue.begin(), m_queue.begin() + sent);
    else {
      if (++m_fails < 10)
        break;
      else {
        close();
        return false;
      }
    }
  }

  return true;
}

bool SafeSocket::isLocal() const {
  static const subnet localnets[] = {
      {0x7f000000, 0xFF000000}, // 127.x.x.x
      {0x0A000000, 0xFF000000}, // 10.x.x.x
      {0xAC100000, 0xFFF00000}, // 172.16.x.x
      {0xC0A80000, 0xFFFF0000}, // 192.168.x.x
      {0x00000000, 0x00000000}, // End of array
  };

  for (auto _sn = localnets; _sn->net && _sn->mask; ++_sn) {
    if ((m_addr.address() & _sn->mask) == _sn->net) return true;
  }

  return false;
}

std::string SafeSocket::addr() {
  return m_addr.to_string();
}

void SafeSocket::close() {
  m_sock.close();
  m_closed = true;
}
