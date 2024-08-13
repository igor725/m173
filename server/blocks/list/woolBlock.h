#pragma once

#include "../block.h"
#include "helper.h"
#include "items/list/wool.h"

class WoolBlock: public Block {
  public:
  WoolBlock(BlockId bid): Block(bid), m_item((ItemId)bid - 256) {}

  BlockId getId() const { return m_index; }

  private:
  BlockId  m_index;
  ItemWool m_item;
};
