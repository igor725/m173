#pragma once

#include "../block.h"
#include "helper.h"
#include "items/list/sapling.h"

class SaplingBlock: public Block {
  public:
  SaplingBlock(BlockId bid): Block(bid), m_item((ItemId)bid - 256) {}

  BlockId getId() const { return m_index; }

  private:
  BlockId     m_index;
  ItemSapling m_item;
};
