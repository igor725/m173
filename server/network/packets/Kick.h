#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class Disconnect: private PacketReader {
  public:
  Disconnect(SafeSocket& sock): PacketReader(sock) { readString(m_reason); }

  const auto& getName() const { return m_reason; }

  private:
  std::wstring m_reason;
};
} // namespace FromClient

namespace ToClient {
class Kick: public PacketWriter {
  public:
  Kick(const std::wstring& reason): PacketWriter(Packet::IDs::Disconnect) { writeString(reason); }
};
} // namespace ToClient
} // namespace Packet
