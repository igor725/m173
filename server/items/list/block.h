#pragma once

#include "../items.h"

class ItemBlock: public Item {
  public:
  ItemBlock(BlockId bid): Item((ItemId)bid - 256) { maxStackSize = 64; }
};
