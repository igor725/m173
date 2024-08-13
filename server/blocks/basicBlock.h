#pragma once

#include "block.h"
#include "items/list/block.h"

class BasicBlock: public Block {
  public:
  BasicBlock(BlockId bid): Block(bid), m_item(bid) {}

  private:
  ItemBlock m_item;
};
