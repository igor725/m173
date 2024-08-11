#pragma once

#include "../objectbase.h"

#include <memory>

class ISnowBall: public ObjectBase {
  public:
  ISnowBall(EntityId owner, const DoubleVector3& motion): ObjectBase(Type::Snowball, owner, motion) {}

  ~ISnowBall() {}
};

std::unique_ptr<ISnowBall> createSnowBall(const DoubleVector3& pos, EntityId owner, const DoubleVector3& motion);
