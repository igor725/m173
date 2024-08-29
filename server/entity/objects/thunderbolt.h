#pragma once

#include "../entitybase.h"

#include <memory>

class IThunderbolt: public EntityBase {
  public:
  IThunderbolt(): EntityBase(Type::Thunderbolt) {}

  bool isPlayer() const final { return false; }

  ~IThunderbolt() {}
};

std::unique_ptr<IThunderbolt> createThunderbolt(const DoubleVector3& pos);
