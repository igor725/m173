#pragma once

#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class PreChunk: public PacketWriter {
  public:
  PreChunk(IntVector2& pos, bool init): PacketWriter(0x32) {
    writeInteger(pos.x);
    writeInteger(pos.z);
    writeBoolean(init);
  }
};
} // namespace ToClient
} // namespace Packet
