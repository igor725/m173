#pragma once

#include "helper.h"

#include <cstdint>

class EntityBase {
  public:
  enum Type {
    Mob,
    Player,
  };

  enum Dimension : int8_t {
    Overworld = 0,
    Nether    = -1,
  };

  EntityBase(Type type): m_type(type) {}

  virtual ~EntityBase() = default;

  Type getType() const { return m_type; }

  Dimension getDimension() const { return m_dimension; }

  bool isOnGround() const { return m_isOnGround; }

  const DoubleVector3* getPosition() const { return &m_position; }

  const float* getRotation() const { return m_rotation; }

  private:
  Type          m_type;
  Dimension     m_dimension;
  bool          m_isOnGround;
  DoubleVector3 m_position    = {0.0, 0.0, 0.0};
  float         m_rotation[2] = {0.0f, 0.0f};
};
