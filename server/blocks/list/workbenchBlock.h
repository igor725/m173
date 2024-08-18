#pragma once

#include "../basicBlock.h"

class WorkbenchBlock: public BasicBlock {
  public:
  WorkbenchBlock(BlockId bid): BasicBlock(bid) {}

  bool blockActivated(const IntVector3& pos, EntityBase* activator) final;
};
