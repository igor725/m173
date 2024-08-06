#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class Thunderbolt: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  Thunderbolt(EntityId eid, const IntVector3& pos): PacketWriter(Packet::IDs::Thunderbolt) {
    writeInteger(eid);
    writeBoolean(true);
    writeInteger(pos.x);
    writeInteger(pos.y);
    writeInteger(pos.z);
  }
};
} // namespace ToClient
} // namespace Packet
