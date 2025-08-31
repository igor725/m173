#pragma once

#include "../entitybase.h"

#include <memory>

namespace Entities {

class ThunderboltBase: public Entities::Base {
  public:
  ThunderboltBase(): Entities::Base(Type::Thunderbolt) {}

  bool isPlayer() const final { return false; }

  ~ThunderboltBase() {}
};

namespace Create {
std::unique_ptr<ThunderboltBase> thunderbolt(const DoubleVector3& pos);
}
} // namespace Entities
