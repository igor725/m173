#pragma once

#include "../mobbase.h"

#include <memory>

namespace Entities {
class PigBase: public MobBase {
  public:
  PigBase(): MobBase(MobBase::Pig) { m_idName = "Mob::Pig"; }

  ~PigBase() {}

  virtual void setSaddle(bool active) = 0;
};

namespace Create {
std::unique_ptr<PigBase> pig(const DoubleVector3& pos);
}
} // namespace Entities
