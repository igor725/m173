#pragma once

#include "../basicBlock.h"

class NoteBlock: public BasicBlock {
  public:
  NoteBlock(BlockId bid): BasicBlock(bid) {}

  bool blockActivated(const IntVector3& pos, Entities::Base* activator) final;
};
