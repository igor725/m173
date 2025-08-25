#pragma once

#include "../entitybase.h"
#include "items/itemstack.h"

#include <memory>

class IPickup: public EntityBase {
  public:
  IPickup(): EntityBase(EntityBase::Type::Pickup) {}

  virtual const ItemStack& getItemStack() const = 0;

  ~IPickup() {}
};

std::unique_ptr<IPickup> createPickup(const DoubleVector3& pos, const ItemStack& is);
