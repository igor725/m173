#pragma once

#include "../../ids.h"
#include "../../packet.h"
#include "entity/creatures/player.h"
#include "world/world.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
namespace FromClient {
class Respawn: private PacketReader {
  public:
  Respawn(SafeSocket& sock): PacketReader(sock) {
    readInteger<Dimension>(); // Dimension
  }
};
} // namespace FromClient
#endif

namespace ToClient {
class PlayerHealth: public PacketWriter {
  public:
  PlayerHealth(PlayerBase* player): PacketWriter(Packet::IDs::PlayerHealth, 2) { writeInteger<int16_t>(player->getHealth()); }
};

class PlayerRespawn: public PacketWriter {
  public:
  PlayerRespawn(PlayerBase* player, IWorld& world): PacketWriter(Packet::IDs::PlayerRespawn, 1) { writeInteger<Dimension>(player->getDimension()); }
};
} // namespace ToClient
} // namespace Packet
