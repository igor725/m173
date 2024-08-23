#pragma once

#include "entitybase.h"
#include "network/packet.h"

#include <memory>

class CreatureBase: public EntityBase {
  public:
  CreatureBase(EntityBase::Type t): EntityBase(t) { m_maxHealth = 20; }

  ~CreatureBase() {}

  IntVector2 getCurrentChunk();
  IntVector2 getPreviousChunk();

  virtual void onMove(const DoubleVector3& dir) {}

  virtual void onChunkChanged(const IntVector2& prev, const IntVector2& curr) {}

  virtual void onHealthChanged(int16_t hearts, bool isDead) {}

  virtual void onFall(double_t distance);

  virtual bool addVelocity(const DoubleVector3& motion);

  void broadcastToTrackers(PacketWriter& pw);

  void updateGroundState(bool ground);

  void setPosition(const DoubleVector3& pos) override;

  void setHealth(int16_t health);

  private:
  double_t m_lastGround = -1.0;
};
