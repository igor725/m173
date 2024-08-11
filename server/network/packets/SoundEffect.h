#pragma once

#include "../ids.h"
#include "../packet.h"
#include "helper.h"

namespace Packet {
namespace ToClient {
class SoundEffect: public PacketWriter {
  public:
  enum EffectId : int32_t {
    Click2     = 1000,
    Click1     = 1001,
    BowFire    = 1002,
    DoorToggle = 1003,
    Extinguish = 1004,
    RecordPlay = 1005,
    Smoke      = 2000,
    BlockBreak = 2001,
  };

  SoundEffect(EffectId eid, const IntVector3& pos, int32_t data): PacketWriter(Packet::IDs::SoundEffect, 17) {
    writeInteger<EffectId>(eid);
    writeInteger<int32_t>(pos.x);
    writeInteger<int8_t>(pos.y);
    writeInteger<int32_t>(pos.z);
    writeInteger<int32_t>(data);
  }

  SoundEffect(EffectId eid, const DoubleVector3& pos, int32_t data)
      : SoundEffect(eid,
                    IntVector3 {
                        static_cast<int32_t>(pos.x),
                        static_cast<int32_t>(pos.y),
                        static_cast<int32_t>(pos.z),
                    },
                    data) {}
};
} // namespace ToClient
} // namespace Packet
