#pragma once

#include "../entitybase.h"
#include "items/itemstack.h"

#include <memory>

namespace Entities {
class PickupBase: public Entities::Base {
  public:
  PickupBase(): Entities::Base(Entities::Base::Type::Pickup) {}

  virtual const ItemStack& getItemStack() const = 0;

  ~PickupBase() {}
};

namespace Create {
std::unique_ptr<PickupBase> pickup(const DoubleVector3& pos, const ItemStack& is);
}
} // namespace Entities
