#pragma once

#include "entitybase.h"
#include "entry/helper.h"

#include <cstdint>

namespace Entities {
class ObjectBase: public Entities::Base {
  public:
  enum Type : int8_t {
    Unknown,
    Boat          = 1,
    Minecart      = 10,
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

  ObjectBase(Type type): Entities::Base(Entities::Base::Object), m_objtype(type), m_owner(0), m_motion() {}

  ObjectBase(Type type, EntityId owner, const DoubleVector3& motion)
      : Entities::Base(Entities::Base::Object), m_objtype(type), m_owner(owner), m_motion(motion) {}

  bool isPlayer() const final { return false; }

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
} // namespace Entities
