#pragma once

#include "../item.h"

class ItemBow: public Item {
  public:
  ItemBow(ItemId iid): Item(iid) { maxStackSize = 1; }

  bool onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) final;
};
