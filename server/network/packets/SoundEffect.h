#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class SoundEffect: public PacketWriter {
  public:
  enum EffectId {
    Click2     = 1000,
    Click1     = 1001,
    BowFire    = 1002,
    DoorToggle = 1003,
    Extinguish = 1004,
    RecordPlay = 1005,
    Smoke      = 2000,
    BlockBreak = 2001,
  };

  SoundEffect(EffectId eid, const IntVector3& pos): PacketWriter(Packet::IDs::SoundEffect) {
    writeInteger(eid);
    writeBoolean(true);
    writeInteger(pos.x);
    writeInteger<int8_t>(pos.y);
    writeInteger(pos.z);
  }
};
} // namespace ToClient
} // namespace Packet
