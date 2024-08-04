#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace ToClient {
class TimeUpdate: public PacketWriter {
  public:
  TimeUpdate(int64_t time): PacketWriter(Packet::IDs::TimeUpdate) { writeInteger(time); }
};
} // namespace ToClient
} // namespace Packet
