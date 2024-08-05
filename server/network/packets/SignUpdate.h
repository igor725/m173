#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace ToClient {
class SignUpdate: public PacketWriter {
  public:
  SignUpdate(const IntVector3& pos, const std::wstring& data): PacketWriter(Packet::IDs::TimeUpdate) {
    writeInteger(pos.x);
    writeInteger<int16_t>(pos.y);
    writeInteger(pos.z);
    // todo different text for each line
    writeString(data);
    writeString(data);
    writeString(data);
    writeString(data);
  }
};
} // namespace ToClient
} // namespace Packet
