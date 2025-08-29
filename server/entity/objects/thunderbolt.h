#pragma once

#include "../entitybase.h"

#include <memory>

namespace Entities {

class IThunderbolt: public Entities::Base {
  public:
  IThunderbolt(): Entities::Base(Type::Thunderbolt) {}

  bool isPlayer() const final { return false; }

  ~IThunderbolt() {}
};

namespace Create {
std::unique_ptr<IThunderbolt> thunderbolt(const DoubleVector3& pos);
}
} // namespace Entities
