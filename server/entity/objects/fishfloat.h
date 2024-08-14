#pragma once

#include "../objectbase.h"

#include <memory>

class IFishFloat: public ObjectBase {
  public:
  IFishFloat(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::FishingFloat, owner, motion) {}

  virtual ~IFishFloat() = default;

  virtual void lure() {}
};

std::unique_ptr<IFishFloat> createFishFloat(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
