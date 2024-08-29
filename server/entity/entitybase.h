#pragma once

#include "helper.h"

#include <cstdint>

class EntityBase {
  friend class EntityManager;

  public:
  enum Type : int8_t {
    Unspecified,
    Creature,
    Object,
    Thunderbolt,
  };

  enum Flags : int8_t {
    None        = 0,
    IsOnFire    = 1 << 0,
    IsCrouching = 1 << 1,
  };

  EntityBase(Type type): m_type(type) {}

  virtual ~EntityBase() = default;

  virtual void onSpawned() {}

  virtual float_t getEyeHeight() const { return 1.6; }

  virtual void setPosition(const DoubleVector3& pos) {
    m_prevPosition = m_position;
    m_position     = pos;
  }

  virtual void setRotation(const FloatAngle& rot);

  virtual void setCrouching(bool value);

  virtual bool isPlayer() const = 0;

  Type getType() const { return m_type; }

  Dimension getDimension() const { return m_dimension; }

  int16_t getHealth() const { return m_health; }

  bool isOnGround() const { return m_isOnGround; }

  bool isMarkedForDestruction() const { return m_shouldBeDestroyed; }

  const DoubleVector3& getPosition() const { return m_position; }

  const DoubleVector3& getPreviousPosition() const { return m_prevPosition; }

  void popPositionDiff(DoubleVector3& pos) {
    getPositionDiff(pos);
    m_prevPosition = m_position;
  }

  double_t getMoveDistance() const {
    DoubleVector3 diff;
    getPositionDiff(diff);
    return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
  }

  const FloatAngle& getRotation() const { return m_rotation; }

  EntityId getEntityId() const { return m_id; }

  void _setEntId(int32_t id) { m_id = id; }

  const DoubleVector3& getForwardVector() const { return m_forward; }

  bool isFlagsChanged() const { return m_prevFlags != m_flags; }

  int8_t popFlags() {
    if (m_prevFlags == m_flags) return false;
    m_prevFlags = m_flags;
    return m_flags;
  }

  virtual void tick(double_t delta) {}

  private:
  void getPositionDiff(DoubleVector3& diff) const {
    diff = {
        m_position.x - m_prevPosition.x,
        m_position.y - m_prevPosition.y,
        m_position.z - m_prevPosition.z,
    };
  };

  protected:
  EntityId  m_id                = -1;
  Type      m_type              = Type::Unspecified;
  Dimension m_dimension         = Dimension::Overworld;
  int16_t   m_health            = 20;
  bool      m_isOnGround        = false;
  bool      m_shouldBeDestroyed = false;
  int8_t    m_flags             = Flags::None;
  int8_t    m_prevFlags         = Flags::None;
  int16_t   m_maxHealth         = 1;

  DoubleVector3 m_prevPosition {0.0, 0.0, 0.0};
  DoubleVector3 m_position {0.0, 0.0, 0.0};
  DoubleVector3 m_forward {0.0, 0.0, 0.0};
  FloatAngle    m_rotation {0.0f, 0.0f};
};
