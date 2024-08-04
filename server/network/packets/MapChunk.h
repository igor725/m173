#pragma once

#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class MapChunk: public PacketWriter {
  public:
  MapChunk(IntVector3& pos, ByteVector3& size, int32_t datasize): PacketWriter(0x33) {
    writeInteger(pos.x);
    writeInteger<int16_t>(pos.y);
    writeInteger(pos.z);
    writeInteger(size.x);
    writeInteger(size.y);
    writeInteger(size.z);
    writeInteger(datasize);
  }
};
} // namespace ToClient
} // namespace Packet
