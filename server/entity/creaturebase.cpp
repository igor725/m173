#include "creaturebase.h"

#include "creatures/player.h"
#include "manager.h"
#include "world/chunk.h"

IntVector2 CreatureBase::getCurrentChunk() {
  return Chunk::toChunkCoords(IntVector2 {
      static_cast<int32_t>(std::round(m_position.x)),
      static_cast<int32_t>(std::round(m_position.z)),
  });
}

IntVector2 CreatureBase::getPreviousChunk() {
  return Chunk::toChunkCoords(IntVector2 {
      static_cast<int32_t>(std::round(m_prevPosition.x)),
      static_cast<int32_t>(std::round(m_prevPosition.z)),
  });
}

void CreatureBase::setPosition(const DoubleVector3& pos) {
  Entities::Base::setPosition(pos);

  onMove(m_position - m_prevPosition);

  const auto prevchunk = getPreviousChunk();
  const auto currchunk = getCurrentChunk();

  if (prevchunk != currchunk) onChunkChanged(prevchunk, currchunk);
}

void CreatureBase::setHealth(int16_t health) {
  auto prevHealth = m_health;
  m_health        = std::max(int16_t(0), std::min(health, m_maxHealth));
  onHealthChanged(m_health - prevHealth, m_health == 0);
}

void CreatureBase::onFall(double_t dist) {
  setHealth(m_health - std::max(0.0, std::ceil(dist - 3.0)));
}

bool CreatureBase::addVelocity(const DoubleVector3& motion) {
  // todo
  return true;
}

// todo move it somewhere?
void CreatureBase::broadcastToTrackers(PacketWriter& pw) {
  Entities::Access::manager().IterPlayers([this, &pw](Entities::PlayerBase* ply) -> bool {
    if (ply->isTrackingEntity(getEntityId())) pw.sendTo(ply->getSocket());
    return true;
  });
}

void CreatureBase::updateGroundState(bool ground) {
  if (m_lastGround < 0.0) m_lastGround = m_position.y;

  if (m_isOnGround != ground) {
    if ((m_isOnGround = ground) == true) {
      auto fallDist = m_lastGround - m_position.y;
      if (fallDist > 2.0) onFall(fallDist);
    } else {
      m_lastGround = m_position.y;
    }
  }
}
