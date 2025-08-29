#include "entitybase.h"

#include "entity/creatures/player.h"
#include "entity/manager.h"
#include "network/packets/Entity.h"

void Entities::Base::setRotation(const FloatAngle& rot) {
  const FloatAngle rad = {rot.yaw / 180.0f * 3.1415f, rot.pitch / 180.0f * 3.1415f};

  m_rotation = rot;
  m_forward  = {
      -std::sin(rad.yaw) * std::cos(rad.pitch),
      -std::sin(rad.pitch),
      std::cos(rad.yaw) * std::cos(rad.pitch),
  };
}

void Entities::Base::setCrouching(bool value) {
  m_prevFlags = m_flags;
  if (value)
    m_flags |= Flags::IsCrouching;
  else
    m_flags &= ~Flags::IsCrouching;
}

void Entities::Base::refreshMeta() {
  if (m_isMetaUpdated) return;

  Packet::ToClient::EntityMeta wdata_meta(this);

  auto mds = wdata_meta.startMetaData();
  readMetadata(mds);
  mds.finish();

  Access::manager().IterPlayers([&](PlayerBase* ply) -> bool {
    if (ply->isTrackingEntity(getEntityId())) {
      wdata_meta.sendTo(ply->getSocket());
    }

    return true;
  });

  m_isMetaUpdated = true;
}
