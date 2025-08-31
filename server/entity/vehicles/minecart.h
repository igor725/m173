#pragma once

#include "../objectbase.h"

#include <memory>

namespace Entities {
class MinecartBase: public ObjectBase {
  public:
  MinecartBase(): ObjectBase(Type::Minecart) {}

  ~MinecartBase() {}
};

namespace Create {
std::unique_ptr<MinecartBase> minecart(const DoubleVector3& pos);
}
} // namespace Entities
