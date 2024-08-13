#pragma once

#include "block.h"

class ItemSapling: public ItemBlock {
  public:
  ItemSapling(ItemId iid): ItemBlock(iid) {
    maxStackSize = 64;
    maxDamage    = 0;
  }

  int8_t getMetadata(int16_t dmg) const final { return int8_t(dmg); }
};
