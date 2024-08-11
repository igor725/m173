#pragma once

#include "../objectbase.h"

#include <memory>

class IArrow: public ObjectBase {
  public:
  IArrow(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::Arrow, owner, motion) {}

  ~IArrow() {}
};

std::unique_ptr<IArrow> createArrow(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
