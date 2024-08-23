#pragma once

#include "entitybase.h"
#include "helper.h"

#include <cstdint>

class ObjectBase: public EntityBase {
  public:
  enum Type : int8_t {
    Unknown,
    Boat          = 1,
    StorageCart   = 11,
    PoweredCart   = 12,
    ActivatedTNT  = 50,
    Arrow         = 60,
    Snowball      = 61,
    Egg           = 62,
    FallingSand   = 70,
    FallingGravel = 71,
    FishingFloat  = 90,
  };

  ObjectBase(Type type): EntityBase(EntityBase::Object), m_objtype(type), m_owner(0), m_motion() {}

  ObjectBase(Type type, EntityId owner, const DoubleVector3& motion): EntityBase(EntityBase::Object), m_objtype(type), m_owner(owner), m_motion(motion) {}

  virtual ~ObjectBase() = default;

  Type getObjectType() const { return m_objtype; }

  EntityId getOwner() const { return m_owner; }

  const DoubleVector3& getStartMotion() const { return m_motion; }

  private:
  Type                m_objtype;
  EntityId            m_owner;
  const DoubleVector3 m_motion;

  protected:
};
