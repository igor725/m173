#pragma once

#include "block.h"
#include "items/list/block.h"

class BasicBlock: public Block {
  public:
  BasicBlock(BlockId bid, float_t hardness = 1.0f): Block(bid), m_item(bid) { m_hardness = hardness; }

  private:
  ItemBlock m_item;
};
