#pragma once

#include "../objectbase.h"

#include <memory>

namespace Entities {
class ISnowBall: public ObjectBase {
  public:
  ISnowBall(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::Snowball, owner, motion) {}

  ~ISnowBall() {}
};

namespace Create {
std::unique_ptr<ISnowBall> snowball(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
}
} // namespace Entities
