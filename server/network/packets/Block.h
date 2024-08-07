#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace ToClient {
class BlockChange: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  BlockChange(const IntVector3& pos, BlockId type, int8_t meta): PacketWriter(Packet::IDs::BlockChg) {
    writeInteger(pos.x);
    writeInteger<int8_t>(pos.y);
    writeInteger(pos.z);
    writeInteger(type);
    writeInteger(meta);
  }
};
} // namespace ToClient
} // namespace Packet
