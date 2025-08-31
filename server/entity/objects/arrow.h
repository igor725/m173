#pragma once

#include "../objectbase.h"

#include <memory>

namespace Entities {
class ArrowBase: public ObjectBase {
  public:
  ArrowBase(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::Arrow, owner, motion) {}

  ~ArrowBase() {}
};

namespace Create {
std::unique_ptr<ArrowBase> arrow(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
}
} // namespace Entities
