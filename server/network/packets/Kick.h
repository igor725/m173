#pragma once

#include "../packet.h"

namespace Packet {
namespace FromClient {
class Disconnect: private PacketReader {
  public:
  Disconnect(sockpp::tcp_socket& sock): PacketReader(sock) { readString(m_reason); }

  const auto& getName() const { return m_reason; }

  private:
  std::wstring m_reason;
};
} // namespace FromClient

namespace ToClient {
class Kick: public PacketWriter {
  public:
  Kick(std::wstring& reason): PacketWriter(0xFF) { writeString(reason); }
};
} // namespace ToClient
} // namespace Packet
