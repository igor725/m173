#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class PreChunk: public PacketWriter {
  public:
  PreChunk(const IntVector2& pos, bool init): PacketWriter(Packet::IDs::PreChunk) {
    writeInteger(pos.x);
    writeInteger(pos.z);
    writeBoolean(init);
  }
};
} // namespace ToClient
} // namespace Packet
