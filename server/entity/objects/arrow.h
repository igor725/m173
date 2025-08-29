#pragma once

#include "../objectbase.h"

#include <memory>

namespace Entities {
class IArrow: public ObjectBase {
  public:
  IArrow(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::Arrow, owner, motion) {}

  ~IArrow() {}
};

namespace Create {
std::unique_ptr<IArrow> arrow(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
}
} // namespace Entities
