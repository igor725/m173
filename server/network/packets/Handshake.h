#pragma once

#include "../packet.h"

namespace Packet {
namespace FromClient {
class Handshake: private PacketReader {
  public:
  Handshake(sockpp::tcp_socket& sock): PacketReader(sock) { readString(m_name_or_hash); }

  const auto& getName() const { return m_name_or_hash; }

  private:
  std::wstring m_name_or_hash;
};
} // namespace FromClient

namespace ToClient {
class Handshake: public PacketWriter {
  public:
  Handshake(std::wstring& connhash): PacketWriter(0x02) { writeString(connhash); }
};
} // namespace ToClient
} // namespace Packet
