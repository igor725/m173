#pragma once

#include "../ids.h"
#include "../packet.h"
#include "entity/entitybase.h"

namespace Packet {
namespace FromClient {
class EntityClick: private PacketReader {
  public:
  EntityClick(SafeSocket& sock): PacketReader(sock) {
    readInteger<EntityId>();                 // EntityID of the clicker
    m_target      = readInteger<EntityId>(); // EntityID of the target
    m_isLeftClick = readBoolean();
  }

  auto getTarget() const { return m_target; }

  auto isLeftClick() const { return m_isLeftClick; }

  private:
  EntityId m_target;
  bool     m_isLeftClick;
};
} // namespace FromClient

namespace ToClient {
class EntityEquipment: public PacketWriter {
  public:
  EntityEquipment(EntityId eid, SlotId sid, ItemId iid, int16_t damage): PacketWriter(Packet::IDs::EntityEquip) {
    writeInteger(eid);
    writeInteger(sid);
    writeInteger(iid);
    writeInteger(damage);
  }
};

class EntityVelocity: public PacketWriter {
  public:
  EntityVelocity(EntityId eid, const DoubleVector3& motion): PacketWriter(Packet::IDs::EntityVel) {
    writeInteger(eid);
    writeInteger(static_cast<int16_t>(motion.x * 8000.0));
    writeInteger(static_cast<int16_t>(motion.y * 8000.0));
    writeInteger(static_cast<int16_t>(motion.z * 8000.0));
  }
};

class EntityDestroy: public PacketWriter {
  public:
  EntityDestroy(EntityId eid): PacketWriter(Packet::IDs::EntityDestroy) { writeInteger(eid); }
};

class EntityIdle: public PacketWriter {
  public:
  EntityIdle(EntityId eid): PacketWriter(Packet::IDs::EntityIdle) { writeInteger(eid); }
};

class EntityRelaMove: public PacketWriter {
  public:
  EntityRelaMove(EntityBase* entity): PacketWriter(Packet::IDs::EntityRelMove) {
    DoubleVector3 diff;
    entity->popPositionDiff(diff);

    writeInteger(entity->getEntityId());
    writeABVector(diff);
  }
};

class EntityLook: public PacketWriter {
  public:
  EntityLook(EntityBase* entity): PacketWriter(Packet::IDs::EntityLook) {
    auto& rot = entity->getRotation();
    writeInteger(entity->getEntityId());
    writeInteger<int8_t>(rot.yawToByte());
    writeInteger<int8_t>(rot.pitchToByte());
  }
};

class EntityLookRM: public PacketWriter {
  public:
  EntityLookRM(EntityBase* entity): PacketWriter(Packet::IDs::EntityLookRM) {
    DoubleVector3 diff;
    entity->popPositionDiff(diff);
    auto& rot = entity->getRotation();

    writeInteger(entity->getEntityId());
    writeABVector(diff);
    writeInteger<int8_t>(rot.yawToByte());
    writeInteger<int8_t>(rot.pitchToByte());
  }
};

class EntitySetPos: public PacketWriter {
  public:
  EntitySetPos(EntityBase* entity): PacketWriter(Packet::IDs::EntitySetPos) {
    auto& pos = entity->getPosition();
    auto& rot = entity->getRotation();

    writeInteger(entity->getEntityId());
    writeAIVector(pos);
    writeInteger<int8_t>(rot.yawToByte());
    writeInteger<int8_t>(rot.pitchToByte());
  }
};

class EntityStatus: public PacketWriter {
  public:
  enum Type : int8_t {
    Unknown1,
    Unknown2,
    Hurted,
    Dead,
    Unknown3,
    Unknown4,
    WolfHeartsFX_off,
    WolfHeartsFX_on,
    WolfShaking, // ???
  };

  EntityStatus(EntityId eid, Type type): PacketWriter(Packet::IDs::EntityStatus) {
    writeInteger(eid);
    writeInteger(type);
  }
};

class EntityMeta: public PacketWriter {
  public:
  EntityMeta(EntityId eid): PacketWriter(Packet::IDs::EntityMeta) { writeInteger(eid); }

  void putHeader(int8_t type, int8_t valueid) { writeInteger<int8_t>((type << 5 | valueid & 31) & 255); }

  void putByte(int valueid, int8_t value) {
    putHeader(0, valueid);
    writeInteger(value);
  }

  void putShort(int valueid, int16_t value) {
    putHeader(1, valueid);
    writeInteger(value);
  }

  void putInt(int valueid, int32_t value) {
    putHeader(2, valueid);
    writeInteger(value);
  }

  void putFloat(int valueid, float_t value) {
    putHeader(3, valueid);
    writeFloating(value);
  }

  void putString(int valueid, const std::wstring& str) {
    putHeader(4, valueid);
    writeString(str);
  }

  void putItem(int valueid, ItemId iid, int8_t count, int16_t damage) {
    putHeader(5, valueid);
    writeInteger(iid);
    writeInteger(count);
    writeInteger(damage);
  }

  void putVector(int valueid, const IntVector3& vec) {
    putHeader(5, valueid);
    writeIVector(vec);
  }

  void finish() { writeInteger<int8_t>(0x7f); }
};

class SpawnThunderbolt: public PacketWriter {
  public:
  SpawnThunderbolt(EntityId eid, const IntVector3& pos): PacketWriter(Packet::IDs::Thunderbolt) {
    writeInteger(eid);
    writeBoolean(true);
    writeInteger(pos.x);
    writeInteger(pos.y);
    writeInteger(pos.z);
  }
};
} // namespace ToClient
} // namespace Packet
