#pragma once

#include "helper.h"

#include <cstdint>

class EntityBase {
  public:
  enum Type {
    Unspecified,
    Mob,
    Player,
  };

  EntityBase(Type type): m_type(type) {}

  virtual ~EntityBase() = default;

  Type getType() const { return m_type; }

  Dimension getDimension() const { return m_dimension; }

  bool isOnGround() const { return m_isOnGround; }

  const DoubleVector3* getPosition() const { return &m_position; }

  const float* getRotation() const { return m_rotation; }

  int32_t getEntityId() const { return m_id; }

  void _setEntId(int32_t id) { m_id = id; }

  protected:
  EntityId      m_id          = -1;
  Type          m_type        = Type::Unspecified;
  Dimension     m_dimension   = Dimension::Overworld;
  bool          m_isOnGround  = false;
  DoubleVector3 m_position    = {0.0, 0.0, 0.0};
  float         m_rotation[2] = {0.0f, 0.0f};
};
