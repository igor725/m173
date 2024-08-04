#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class Thunderbolt: public PacketWriter {
  public:
  Thunderbolt(EntityId eid, IntVector3& pos): PacketWriter(Packet::IDs::Thunderbolt) {
    writeInteger(eid);
    writeBoolean(true);
    writeInteger(pos.x);
    writeInteger(pos.y);
    writeInteger(pos.z);
  }
};
} // namespace ToClient
} // namespace Packet
