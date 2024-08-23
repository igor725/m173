#pragma once

#include "../mobbase.h"

#include <memory>

class IPig: public MobBase {
  public:
  IPig(): MobBase(Type::Pig) {}

  ~IPig() {}
};

std::unique_ptr<IPig> createPig(const DoubleVector3& pos);
