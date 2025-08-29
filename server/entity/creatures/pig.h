#pragma once

#include "../mobbase.h"

#include <memory>

namespace Entities {
class IPig: public MobBase {
  public:
  IPig(): MobBase(MobBase::Pig) { m_idName = "Mob::Pig"; }

  ~IPig() {}

  virtual void setSaddle(bool active) = 0;
};

namespace Create {
std::unique_ptr<IPig> pig(const DoubleVector3& pos);
}
} // namespace Entities
