#pragma once

#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace ToClient {
class HealthUpdate: public PacketWriter {
  public:
  HealthUpdate(int16_t health): PacketWriter(Packet::IDs::HealthUpdate) { writeInteger(health); }
};
} // namespace ToClient
} // namespace Packet
