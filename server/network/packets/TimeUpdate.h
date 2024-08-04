#pragma once

#include "../packet.h"

namespace Packet {
namespace ToClient {
class TimeUpdate: public PacketWriter {
  public:
  TimeUpdate(int64_t time): PacketWriter(0x04) { writeInteger(time); }
};
} // namespace ToClient
} // namespace Packet
