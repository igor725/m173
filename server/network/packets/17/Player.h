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
    readInteger<int8_t>();    // Difficulty
    readInteger<int8_t>();    // Game type
    readInteger<int16_t>();   // Unused
    readInteger<int64_t>();   // Seed
  }
};
} // namespace FromClient
#endif

namespace ToClient {
class PlayerHealth: public PacketWriter {
  public:
  PlayerHealth(PlayerBase* player): PacketWriter(Packet::IDs::PlayerHealth, 8) {
    writeInteger<int16_t>(player->getHealth());
    writeInteger<int16_t>(player->getHunger());
    writeFloating<float_t>(player->getSatur());
  }
};

class PlayerRespawn: public PacketWriter {
  public:
  PlayerRespawn(PlayerBase* player, IWorld& world): PacketWriter(Packet::IDs::PlayerRespawn, 13) {
    writeInteger<Dimension>(player->getDimension());
    writeInteger<int8_t>(world.getDifficulty()); // Difficulty
    writeInteger<int8_t>(world.getGameType());   // Game type
    writeInteger<int16_t>(128);
    writeInteger<int64_t>(world.getSeed());
  }
};
} // namespace ToClient
} // namespace Packet
