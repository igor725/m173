#pragma once

#include "../../entity/player/player.h"
#include "../ids.h"
#include "../packet.h"

namespace Packet {
namespace FromClient {
class Respawn: private PacketReader {
  public:
  Respawn(SafeSocket& sock): PacketReader(sock) {
    m_dim = readInteger<Dimension>(); // Dimension
  }

  const auto getDimension() const { return m_dim; }

  private:
  Dimension m_dim;
};

class PlayerPosAndLook: private PacketReader {
  public:
  PlayerPosAndLook(SafeSocket& sock): PacketReader(sock) {
    m_pos.x       = readFloating<double_t>(); // X
    m_pos.y       = readFloating<double_t>(); // Y
    m_stance      = readFloating<double_t>(); // Stance
    m_pos.z       = readFloating<double_t>(); // Z
    m_angle.yaw   = readFloating<float_t>();  // Yaw
    m_angle.pitch = readFloating<float_t>();  // Pitch
    m_isOnGround  = readBoolean();            // On ground
  }

  const DoubleVector3& getPosition() const { return m_pos; }

  double_t getStance() const { return m_stance; }

  const FloatAngle& getAngle() const { return m_angle; }

  bool isOnGround() const { return m_isOnGround; }

  private:
  DoubleVector3 m_pos;
  FloatAngle    m_angle;
  double_t      m_stance;
  bool          m_isOnGround;
};

class PlayerPos: private PacketReader {
  public:
  PlayerPos(SafeSocket& sock): PacketReader(sock) {
    m_pos.x      = readFloating<double_t>(); // X
    m_pos.y      = readFloating<double_t>(); // Y
    m_stance     = readFloating<double_t>(); // Stance
    m_pos.z      = readFloating<double_t>(); // Z
    m_isOnGround = readBoolean();            // On ground
  }

  const DoubleVector3& getPosition() const { return m_pos; }

  double_t getStance() const { return m_stance; }

  private:
  DoubleVector3 m_pos;
  double_t      m_stance;
  bool          m_isOnGround;
};

class PlayerLook: private PacketReader {
  public:
  PlayerLook(SafeSocket& sock): PacketReader(sock) {
    m_angle.yaw   = readFloating<float_t>(); // Yaw
    m_angle.pitch = readFloating<float_t>(); // Pitch
    m_isOnGround  = readBoolean();           // On ground
  }

  const FloatAngle& getAngle() const { return m_angle; }

  private:
  FloatAngle m_angle;
  bool       m_isOnGround;
};

class PlayerFall: private PacketReader {
  public:
  PlayerFall(SafeSocket& sock): PacketReader(sock) {
    m_isOnGround = readBoolean(); // On ground
  }

  private:
  bool m_isOnGround;
};

class PlayerDig: private PacketReader {
  public:
  PlayerDig(SafeSocket& sock): PacketReader(sock) {
    m_status = readInteger<DigStatus>(); // Status
    m_pos.x  = readInteger<int32_t>();   // X
    m_pos.y  = readInteger<int8_t>();    // Y
    m_pos.z  = readInteger<int32_t>();   // Z
    readInteger<int8_t>();               // Face
  }

  bool isDiggingFinished() const { return m_status == DigStatus::Finished; }

  const IntVector3& getPosition() const { return m_pos; }

  private:
  DigStatus  m_status;
  IntVector3 m_pos;
};

class PlayerHold: private PacketReader {
  public:
  PlayerHold(SafeSocket& sock): PacketReader(sock) {
    m_slot = readInteger<SlotId>(); // Slot Id
  }

  SlotId getSlotId() const { return m_slot; }

  private:
  SlotId m_slot;
};

class BlockPlace: private PacketReader {
  public:
  BlockPlace(SafeSocket& sock): PacketReader(sock) {
    m_pos.x     = readInteger<int32_t>();                 // X
    m_pos.y     = readInteger<int8_t>();                  // Y
    m_pos.z     = readInteger<int32_t>();                 // Z
    m_direction = readInteger<int8_t>();                  // Direction
    if ((m_item_or_block = readInteger<int16_t>()) > 0) { // Block or Item ID
      m_amount = readInteger<int8_t>();                   // Amount
      m_damage = readInteger<int16_t>();                  // Damage
    }
  }

  private:
  IntVector3 m_pos;
  int8_t     m_direction;
  int8_t     m_amount;
  int16_t    m_damage;
  int16_t    m_item_or_block;
};

class Disconnect: private PacketReader {
  public:
  Disconnect(SafeSocket& sock): PacketReader(sock) { readString(m_reason); }

  const auto& getReason() const { return m_reason; }

  private:
  std::wstring m_reason;
};
} // namespace FromClient

namespace ToClient {
class PlayerHealth: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  PlayerHealth(int16_t health): PacketWriter(Packet::IDs::PlayerHealth) { writeInteger(health); }
};

class PlayerRespawn: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  PlayerRespawn(Dimension dim): PacketWriter(Packet::IDs::PlayerRespawn) { writeInteger<Dimension>(dim); }
};

class PlayerSpawn: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  PlayerSpawn(IPlayer* player): PacketWriter(Packet::IDs::PlayerSpawn) {
    auto& position = player->getPosition();
    auto& rotation = player->getRotation();

    writeInteger<EntityId>(player->getEntityId());
    writeString(player->getName());

    /* Player position */
    writeInteger<int32_t>(static_cast<int32_t>(position.x * 32.0));
    writeInteger<int32_t>(static_cast<int32_t>(position.y * 32.0));
    writeInteger<int32_t>(static_cast<int32_t>(position.z * 32.0));

    /* Player rotation */
    writeInteger<int8_t>(rotation.yawToByte());   // Yaw
    writeInteger<int8_t>(rotation.pitchToByte()); // Pitch

    /* Other data */
    writeInteger(player->getHeldItem());
  }
};

class PlayerPosAndLook: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  PlayerPosAndLook(IPlayer* player): PacketWriter(Packet::IDs::PlayerPnL) {
    auto& position = player->getPosition();
    auto& rotation = player->getRotation();

    /* Player position */
    writeFloating(position.x);
    writeFloating(player->getStance());
    writeFloating(position.y);
    writeFloating(position.z);

    /* Player rotation */
    writeFloating(rotation.yaw);
    writeFloating(rotation.pitch);

    /* Player physics values */
    writeBoolean(player->isOnGround());
  }
};

class PlayerKick: private PacketWriter {
  public:
  using PacketWriter::sendTo;

  PlayerKick(const std::wstring& reason): PacketWriter(Packet::IDs::ConnectionFin) { writeString(reason); }
};
} // namespace ToClient
} // namespace Packet
