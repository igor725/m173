#pragma once

#include "../../entity/player/player.h"
#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class PlayerPosAndLook: private PacketReader {
  public:
  PlayerPosAndLook(SafeSocket& sock): PacketReader(sock) {
    readFloating<double_t>(); // X
    readFloating<double_t>(); // Y
    readFloating<double_t>(); // Stance
    readFloating<double_t>(); // Z
    readFloating<float_t>();  // Yaw
    readFloating<float_t>();  // Pitch
    readBoolean();            // On ground
  }

  private:
};

class PlayerPos: private PacketReader {
  public:
  PlayerPos(SafeSocket& sock): PacketReader(sock) {
    readFloating<double_t>(); // X
    readFloating<double_t>(); // Y
    readFloating<double_t>(); // Stance
    readFloating<double_t>(); // Z
    readBoolean();            // On ground
  }

  private:
};

class PlayerLook: private PacketReader {
  public:
  PlayerLook(SafeSocket& sock): PacketReader(sock) {
    readFloating<float_t>(); // X
    readFloating<float_t>(); // Y
    readBoolean();           // On ground
  }

  private:
};

class PlayerFall: private PacketReader {
  public:
  PlayerFall(SafeSocket& sock): PacketReader(sock) {
    readBoolean(); // On ground
  }

  private:
};

class PlayerDig: private PacketReader {
  public:
  PlayerDig(SafeSocket& sock): PacketReader(sock) {
    readInteger<DigStatus>(); // Status
    readInteger<int32_t>();   // X
    readInteger<int8_t>();    // Y
    readInteger<int32_t>();   // Z
    readInteger<int8_t>();    // Face
  }

  private:
};

class BlockPlace: public PacketReader {
  public:
  BlockPlace(SafeSocket& sock): PacketReader(sock) {
    readInteger<int32_t>();                               // X
    readInteger<int8_t>();                                // Y
    readInteger<int32_t>();                               // Z
    readInteger<int8_t>();                                // Direction
    if ((m_item_or_block = readInteger<int16_t>()) > 0) { // Block or Item ID
      readInteger<int8_t>();                              // Amount
      readInteger<int16_t>();                             // Damage
    }
  }

  private:
  int16_t m_item_or_block;
};
} // namespace FromClient

namespace ToClient {
class PlayerPosAndLook: public PacketWriter {
  public:
  PlayerPosAndLook(IPlayer* player): PacketWriter(Packet::IDs::PlayerPnL) {
    auto position = player->getPosition();
    auto rotation = player->getRotation();

    /* Player position */
    writeFloating(position->x);
    writeFloating(position->y);
    writeFloating(position->y);
    writeFloating(position->z);

    /* Player rotation */
    writeFloating(rotation[0]);
    writeFloating(rotation[1]);

    /* Player physics values */
    writeBoolean(player->isOnGround());
  }
};
} // namespace ToClient
} // namespace Packet
