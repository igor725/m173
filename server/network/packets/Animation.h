#pragma once

#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace FromClient {
class Animation: private PacketReader {
  public:
  Animation(sockpp::tcp_socket& sock): PacketReader(sock) {
    readInteger<int32_t>();
    m_anim = readInteger<AnimId>();
  }

  const auto& getName() const { return m_anim; }

  private:
  AnimId m_anim;
};
} // namespace FromClient

namespace ToClient {
class Animation: public PacketWriter {
  public:
  Animation(EntityId eid, AnimId aid): PacketWriter(0x12) {
    writeInteger(eid);
    writeInteger(aid);
  }
};
} // namespace ToClient
} // namespace Packet
