#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class SpawnPosition: public PacketWriter {
  public:
  SpawnPosition(const IntVector3& pos): PacketWriter(Packet::IDs::SpawnPos) {
    writeInteger(pos.x);
    writeInteger(pos.y);
    writeInteger(pos.z);
  }
};
} // namespace ToClient
} // namespace Packet
