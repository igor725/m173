#pragma once

#include "../item.h"

class ItemSnowball: public Item {
  public:
  ItemSnowball(ItemId iid): Item(iid) { maxStackSize = 16; }

  bool onItemRightClick(ItemStack& is, Entities::Base* clicker, const IntVector3& pos, int8_t dir) final;
};
