#pragma once

#include "../ids.h"
#include "../packet.h"
#include "entity/entitybase.h"
#include "entity/interact/pickup.h"
#include "entity/mobbase.h"
#include "entry/helper.h"
#include "network/ids.h"

#include <cstdint>

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
class PickupSpawn: public PacketWriter {
  public:
  PickupSpawn(Entities::Base* ent): PacketWriter(Packet::IDs::PickupSpawn, 24) {
    if (auto pickup = dynamic_cast<Entities::PickupBase*>(ent)) {

      writeInteger<int32_t>(pickup->getEntityId());

      auto& is = pickup->getItemStack();
      writeInteger<int32_t>(is.itemId);
      writeInteger<int8_t>(is.stackSize);
      writeInteger<int16_t>(is.itemDamage);

      writeAIVector(pickup->getPosition());
      writeFullAngle(pickup->getRotation());
    }
  }
};

class CollectItem: public PacketWriter {
  public:
  CollectItem(EntityId picker, EntityId item): PacketWriter(Packet::IDs::CollectItem, 8) {
    writeInteger<int32_t>(picker);
    writeInteger<int32_t>(item);
  }
};

class MobSpawn: public PacketWriter {
  public:
  MobSpawn(EntityId eid, Entities::MobBase::Type type, const DoubleVector3& position, const FloatAngle& rotation): PacketWriter(Packet::IDs::SpawnMob, 20) {
    writeInteger<int32_t>(eid);
    writeInteger<int8_t>(type);
    writeAIVector(position);
    writePartAngle(rotation);

    /**
     * @todo Metadata for mobs:
     *
     * Credits https://wiki.vg/User:Olive/Beta_Protocol
     *
     * Creeper: 16 - fuse status (int8), 17 - charged creeper (bool)
     * Skeleton: (empty)
     * Spider: (empty)
     * GiantZombie: (empty)
     * Zombie: (empty)
     * Slime: 16 - slime size (byte, from 0 to 2)
     * Ghast: 16 - red eyes on (bool)
     * ZombiePigman: (empty)
     * Pig: 16 - has saddle (bool)
     * Sheep: 16 - bitflags (byte, bit 0x10 set if sheep's sheared, lower 4 bits indicate the sheep's color)
     * Cow: (empty)
     * Chicken: (empty)
     * Squid: (empty)
     * Wolf: 16 - bitflags (byte, 1st bit - is sitting, 2nd bit - aggressive, 3rd bit - tamed), 17 - tamer name (string), 18 - health (int32)
     *
     *
     * Sheep colors:
     *   0 - White, 1 - Orange, 2 - Magenta, 3 - LightBlue, 4 - Yellow,
     *   5 - Lime, 6 - Pink, 7 - Gray, 8 - Silver, 9 - Cyan, 10 - Purple,
     *   11 - Blue, 12 - Brown, 13 - Green, 14 - Red, 15 - Black
     *
     * Shared metadata for all entities is a set of bitflags (int8) that is installed by setting index 0:
     * 1st bit - is entity on fire, 2nd bit - is entity crouching, 3rd bit - is entity riding something
     *
     */
    writeInteger<int8_t>(127);
  }
};

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
  EntityRelaMove(Entities::Base* entity): PacketWriter(Packet::IDs::EntityRelMove, 7) {
    DoubleVector3 diff;
    entity->popPositionDiff(diff);

    writeInteger<EntityId>(entity->getEntityId());
    writeABVector(diff);
  }
};

class EntityLook: public PacketWriter {
  public:
  EntityLook(Entities::Base* entity): PacketWriter(Packet::IDs::EntityLook, 6) {
    writeInteger<EntityId>(entity->getEntityId());
    writePartAngle(entity->getRotation());
  }
};

class EntityLookRM: public PacketWriter {
  public:
  EntityLookRM(Entities::Base* entity): PacketWriter(Packet::IDs::EntityLookRM, 9) {
    DoubleVector3 diff;
    entity->popPositionDiff(diff);

    writeInteger<EntityId>(entity->getEntityId());
    writeABVector(diff);
    writePartAngle(entity->getRotation());
  }
};

class EntitySetPos: public PacketWriter {
  public:
  EntitySetPos(Entities::Base* entity): PacketWriter(Packet::IDs::EntitySetPos, 18) {
    writeInteger<EntityId>(entity->getEntityId());
    writeAIVector(entity->getPosition());
    writePartAngle(entity->getRotation());
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

  EntityMeta(Entities::Base* ent): PacketWriter(Packet::IDs::EntityMeta, 10) { writeInteger<EntityId>(ent->getEntityId()); }
};

class SpawnThunderbolt: public PacketWriter {
  public:
  SpawnThunderbolt(Entities::Base* ent): PacketWriter(Packet::IDs::Thunderbolt, 17) {
    writeInteger<EntityId>(ent->getEntityId());
    writeBoolean(true);
    writeAIVector(ent->getPosition());
  }
};
} // namespace ToClient
} // namespace Packet
