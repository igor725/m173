#pragma once

#include "../ids.h"
#include "../packet.h"
#include "entity/entitybase.h"

namespace Packet {
#ifdef M173_ACTIVATE_READER_API
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
#endif

namespace ToClient {
class EntityEquipment: public PacketWriter {
  public:
  EntityEquipment(EntityId eid, SlotId sid, const ItemStack& is): PacketWriter(Packet::IDs::EntityEquip, 10) {
    writeInteger<EntityId>(eid);
    writeInteger<SlotId>(sid);
    writeInteger<ItemId>(is.itemId);
    writeInteger<int16_t>(is.itemDamage);
  }
};

class EntityVelocity: public PacketWriter {
  public:
  EntityVelocity(EntityId eid, const DoubleVector3& motion): PacketWriter(Packet::IDs::EntityVel, 10) {
    writeInteger<EntityId>(eid);
    writeMotion(motion);
  }
};

class EntityDestroy: public PacketWriter {
  public:
  EntityDestroy(EntityId eid): PacketWriter(Packet::IDs::EntityDestroy, 4) { writeInteger<EntityId>(eid); }
};

class EntityIdle: public PacketWriter {
  public:
  EntityIdle(EntityId eid): PacketWriter(Packet::IDs::EntityIdle, 4) { writeInteger<EntityId>(eid); }
};

class EntityRelaMove: public PacketWriter {
  public:
  EntityRelaMove(EntityBase* entity): PacketWriter(Packet::IDs::EntityRelMove, 7) {
    DoubleVector3 diff;
    entity->popPositionDiff(diff);

    writeInteger<EntityId>(entity->getEntityId());
    writeABVector(diff);
  }
};

class EntityLook: public PacketWriter {
  public:
  EntityLook(EntityBase* entity): PacketWriter(Packet::IDs::EntityLook, 6) {
    auto& rot = entity->getRotation();
    writeInteger<EntityId>(entity->getEntityId());
    writeInteger<int8_t>(rot.yawToByte());
    writeInteger<int8_t>(rot.pitchToByte());
  }
};

class EntityLookRM: public PacketWriter {
  public:
  EntityLookRM(EntityBase* entity): PacketWriter(Packet::IDs::EntityLookRM, 9) {
    DoubleVector3 diff;
    entity->popPositionDiff(diff);
    auto& rot = entity->getRotation();

    writeInteger<EntityId>(entity->getEntityId());
    writeABVector(diff);
    writeInteger<int8_t>(rot.yawToByte());
    writeInteger<int8_t>(rot.pitchToByte());
  }
};

class EntitySetPos: public PacketWriter {
  public:
  EntitySetPos(EntityBase* entity): PacketWriter(Packet::IDs::EntitySetPos, 18) {
    auto& pos = entity->getPosition();
    auto& rot = entity->getRotation();

    writeInteger<EntityId>(entity->getEntityId());
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

  EntityStatus(EntityId eid, Type type): PacketWriter(Packet::IDs::EntityStatus, 5) {
    writeInteger<EntityId>(eid);
    writeInteger<Type>(type);
  }
};

class EntityMeta: public PacketWriter {
  public:
  EntityMeta(EntityId eid): PacketWriter(Packet::IDs::EntityMeta, 10 /* Not the actual packet size, just a reservation */) { writeInteger<EntityId>(eid); }

  EntityMeta(EntityBase* ent): PacketWriter(Packet::IDs::EntityMeta, 7) {
    writeInteger<EntityId>(ent->getEntityId());

    MetaDataStream mds(*this);
    mds.putByte(0, ent->popFlags());
  }
};

class SpawnThunderbolt: public PacketWriter {
  public:
  SpawnThunderbolt(EntityId eid, const IntVector3& pos): PacketWriter(Packet::IDs::Thunderbolt, 17) {
    writeInteger<EntityId>(eid);
    writeBoolean(true);
    writeInteger<int32_t>(pos.x);
    writeInteger<int32_t>(pos.y);
    writeInteger<int32_t>(pos.z);
  }
};
} // namespace ToClient
} // namespace Packet
