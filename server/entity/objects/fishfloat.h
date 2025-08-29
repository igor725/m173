#pragma once

#include "../objectbase.h"

#include <memory>

namespace Entities {
class FishFloatBase: public ObjectBase {
  public:
  FishFloatBase(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::FishingFloat, owner, motion) {}

  virtual ~FishFloatBase() = default;

  virtual void lure() {}
};

namespace Create {
std::unique_ptr<FishFloatBase> fishFloat(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
}
} // namespace Entities
