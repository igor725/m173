#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class Ping: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  Ping(): PacketWriter(Packet::IDs::KeepAlive) {}
};
} // namespace ToClient
} // namespace Packet
