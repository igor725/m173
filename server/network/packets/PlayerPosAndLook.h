#pragma once

#include "../../entity/player/player.h"
#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class PlayerPosAndLook: private PacketReader {
  public:
  PlayerPosAndLook(sockpp::tcp_socket& sock): PacketReader(sock) {
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
  PlayerPos(sockpp::tcp_socket& sock): PacketReader(sock) {
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
  PlayerLook(sockpp::tcp_socket& sock): PacketReader(sock) {
    readFloating<float_t>(); // X
    readFloating<float_t>(); // Y
    readBoolean();           // On ground
  }

  private:
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
