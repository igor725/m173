#pragma once

#include "../item.h"

class ItemLighter: public Item {
  public:
  ItemLighter(ItemId iid): Item(iid) {
    maxStackSize = 1;
    maxDamage    = 64;
  }

  bool onUseItemOnBlock(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) final;
};
