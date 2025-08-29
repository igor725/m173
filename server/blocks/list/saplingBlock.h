#pragma once

#include "../block.h"
#include "entry/helper.h"
#include "items/list/sapling.h"

class SaplingBlock: public Block {
  public:
  SaplingBlock(BlockId bid): Block(bid), m_item((ItemId)bid - 256) { m_hardness = 0.0f; }

  private:
  ItemSapling m_item;
};
