#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class MapChunk: public PacketWriter {
  public:
  MapChunk(const IntVector3& pos, const ByteVector3& size, int32_t datasize): PacketWriter(Packet::IDs::MapChunk) {
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
