#pragma once

#include "../objectbase.h"

#include <memory>

namespace Entities {
class SnowBallBase: public ObjectBase {
  public:
  SnowBallBase(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::Snowball, owner, motion) {}

  ~SnowBallBase() {}
};

namespace Create {
std::unique_ptr<SnowBallBase> snowball(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
}
} // namespace Entities
