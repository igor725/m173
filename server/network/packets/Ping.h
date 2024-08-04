#pragma once

#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class Ping: public PacketWriter {
  public:
  Ping(): PacketWriter(0x0) {}
};
} // namespace ToClient
} // namespace Packet
