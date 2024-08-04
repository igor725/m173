#pragma once

#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class SpawnPosition: public PacketWriter {
  public:
  SpawnPosition(const IntVector3& pos): PacketWriter(0x06) {
    writeInteger(pos.x);
    writeInteger(pos.y);
    writeInteger(pos.z);
  }
};
} // namespace ToClient
} // namespace Packet
