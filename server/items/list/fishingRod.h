#pragma once

#include "../item.h"

class ItemFishingRod: public Item {
  public:
  ItemFishingRod(ItemId iid): Item(iid) {
    maxStackSize = 1;
    maxDamage    = 64;
  }

  bool onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) final;
};
