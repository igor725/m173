#pragma once

#include "../ids.h"
#include "../packet.h"
#include "entity/objectbase.h"

namespace Packet {
namespace ToClient {
class ObjectSpawn: public PacketWriter {
  public:
  ObjectSpawn(Entities::ObjectBase* object): PacketWriter(Packet::IDs::SpawnObject, 21) {
    writeInteger<EntityId>(object->getEntityId());
    writeInteger<int8_t>(object->getObjectType());
    writeAIVector(object->getPosition());
    if (auto owner = object->getOwner()) {
      writeInteger<EntityId>(owner);
      writeMotion(object->getStartMotion());
    } else {
      writeInteger<EntityId>(0);
    }
  }
};
} // namespace ToClient
} // namespace Packet
