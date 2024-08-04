#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class Ping: public PacketWriter {
  public:
  Ping(): PacketWriter(Packet::IDs::KeepAlive) {}
};
} // namespace ToClient
} // namespace Packet
