#pragma once

#include "../item.h"

class ItemMap: public Item {
  public:
  ItemMap(ItemId iid): Item(iid) {
    maxStackSize = 1;
    maxDamage    = 0;
  }

  bool onEquipedByEntity(ItemStack& is, Entities::Base* equiper) final;
};
