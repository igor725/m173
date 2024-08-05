#pragma once

#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace FromClient {
class EntityAction: private PacketReader {
  public:
  EntityAction(SafeSocket& sock): PacketReader(sock) {
    readInteger<EntityId>();
    m_action = readInteger<int8_t>();
  }

  const auto& getAction() const { return m_action; }

  private:
  int8_t m_action;
};
} // namespace FromClient
} // namespace Packet
