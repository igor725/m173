#pragma once

#include "../ids.h"
#include "../packet.h"
#include "entity/creatures/player.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
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

  bool isOnGround() const { return m_isOnGround; }

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

  bool isOnGround() const { return m_isOnGround; }

  private:
  bool m_isOnGround;
};

class PlayerDig: private PacketReader {
  public:
  enum DigStatus : int8_t {
    Started  = 0,
    Finished = 2,
    DropItem = 4,
  };

  PlayerDig(SafeSocket& sock): PacketReader(sock) {
    m_status = readInteger<DigStatus>(); // Status
    m_pos.x  = readInteger<int32_t>();   // X
    m_pos.y  = readInteger<int8_t>();    // Y
    m_pos.z  = readInteger<int32_t>();   // Z
    readInteger<int8_t>();               // Face
  }

  bool isDiggingFinished() const { return m_status == DigStatus::Finished; }

  bool isDroppingBlock() const { return m_status == DigStatus::DropItem; }

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

  const IntVector3& getClickPosition() const { return m_pos; }

  int16_t getId() const { return m_item_or_block; }

  int8_t getDirection() const { return m_direction; }

  private:
  IntVector3 m_pos;
  int8_t     m_direction;
  int8_t     m_amount;
  int16_t    m_damage;
  int16_t    m_item_or_block;
};

class PlayerAnim: private PacketReader {
  public:
  PlayerAnim(SafeSocket& sock): PacketReader(sock) {
    readInteger<int32_t>();
    m_anim = readInteger<AnimId>();
  }

  const auto& getAnimation() const { return m_anim; }

  private:
  AnimId m_anim;
};

class PlayerAction: private PacketReader {
  public:
  PlayerAction(SafeSocket& sock): PacketReader(sock) {
    readInteger<EntityId>();
    m_action = readInteger<int8_t>();
  }

  const auto& getAction() const { return m_action; }

  private:
  int8_t m_action;
};

class Disconnect: private PacketReader {
  public:
  Disconnect(SafeSocket& sock): PacketReader(sock) { readString(m_reason); }

  const auto& getReason() const { return m_reason; }

  private:
  std::wstring m_reason;
};
} // namespace FromClient
#endif

namespace ToClient {
class PlayerHealth: public PacketWriter {
  public:
  PlayerHealth(int16_t health): PacketWriter(Packet::IDs::PlayerHealth, 2) { writeInteger(health); }
};

class PlayerRespawn: public PacketWriter {
  public:
  PlayerRespawn(Dimension dim): PacketWriter(Packet::IDs::PlayerRespawn, 1) { writeInteger<Dimension>(dim); }
};

class PlayerAnim: public PacketWriter {
  public:
  PlayerAnim(EntityId eid, AnimId aid): PacketWriter(Packet::IDs::PlayerAnim, 5) {
    writeInteger<EntityId>(eid);
    writeInteger<AnimId>(aid);
  }
};

class PlayerPosAndLook: public PacketWriter {
  public:
  PlayerPosAndLook(PlayerBase* player): PacketWriter(Packet::IDs::PlayerPnL, 41) {
    auto& position = player->getPosition();
    auto& rotation = player->getRotation();

    /* Player position */
    writeFloating<double_t>(position.x);
    writeFloating<double_t>(position.y);
    writeFloating<double_t>(player->getStance());
    writeFloating<double_t>(position.z);

    /* Player rotation */
    writeFloating<float_t>(rotation.yaw);
    writeFloating<float_t>(rotation.pitch);

    /* Player physics values */
    writeBoolean(player->isOnGround());
  }
};

class PlayerSpawn: public PacketWriter {
  public:
  PlayerSpawn(PlayerBase* player): PacketWriter(Packet::IDs::PlayerSpawn, 22 + player->getName().size()) {
    auto& position = player->getPosition();
    auto& rotation = player->getRotation();

    writeInteger<EntityId>(player->getEntityId());
    writeString(player->getName());

    /* Player position */
    writeAIVector(position);

    /* Player rotation */
    writeInteger<int8_t>(rotation.yawToByte());   // Yaw
    writeInteger<int8_t>(rotation.pitchToByte()); // Pitch

    /* Other data */
    auto heldItem = player->getHeldItem().itemId;
    writeInteger<ItemId>(heldItem < 0 ? 0 : heldItem);
  }
};

class PlayerKick: public PacketWriter {
  public:
  PlayerKick(const std::wstring_view reason): PacketWriter(Packet::IDs::ConnectionFin) { writeString(reason); }
};
} // namespace ToClient
} // namespace Packet
