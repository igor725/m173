#pragma once

#include "../item.h"

class ItemSign: public Item {
  public:
  ItemSign(ItemId iid): Item(iid) {
    maxStackSize = 1;
    maxDamage    = 0;
  }

  bool onUseItemOnBlock(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) final;
};
