#pragma once

#include "helper.h"

#include <cstdint>

class EntityBase {
  public:
  enum Type : int8_t {
    Unspecified,
    Mob,
    Player,
  };

  enum Flags : int8_t {
    None        = 0,
    IsOnFire    = 1 << 0,
    IsCrouching = 1 << 1,
  };

  EntityBase(Type type): m_type(type) {}

  virtual ~EntityBase() = default;

  Type getType() const { return m_type; }

  Dimension getDimension() const { return m_dimension; }

  int16_t getHealth() const { return m_health; }

  bool isOnGround() const { return m_isOnGround; }

  const DoubleVector3& getPosition() const { return m_position; }

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

  int32_t getEntityId() const { return m_id; }

  void _setEntId(int32_t id) { m_id = id; }

  virtual void setPosition(const DoubleVector3& pos) {
    m_prevPosition = m_position;
    m_position     = pos;
  }

  virtual void setRotation(const FloatAngle& rot) { m_rotation = rot; }

  virtual void setCrouching(bool value) {
    m_prevFlags = m_flags;
    if (value)
      m_flags |= Flags::IsCrouching;
    else
      m_flags &= ~Flags::IsCrouching;
  }

  bool isFlagsChanged() const { return m_prevFlags != m_flags; }

  int8_t popFlags() {
    if (m_prevFlags == m_flags) return false;
    m_prevFlags = m_flags;
    return m_flags;
  }

  private:
  void getPositionDiff(DoubleVector3& diff) const {
    diff = {
        .x = m_position.x - m_prevPosition.x,
        .y = m_position.y - m_prevPosition.y,
        .z = m_position.z - m_prevPosition.z,
    };
  };

  protected:
  EntityId  m_id         = -1;
  Type      m_type       = Type::Unspecified;
  Dimension m_dimension  = Dimension::Overworld;
  int16_t   m_health     = 20;
  bool      m_isOnGround = false;
  int8_t    m_flags      = Flags::None;
  int8_t    m_prevFlags  = Flags::None;

  DoubleVector3 m_prevPosition = {0.0, 0.0, 0.0};
  DoubleVector3 m_position     = {0.0, 0.0, 0.0};
  FloatAngle    m_rotation     = {0.0f, 0.0f};
};
