#pragma once

#include "../items.h"
#include "entity/player/player.h"

class ItemSnowball: public Item {
  public:
  ItemSnowball(ItemId iid): Item(iid) { maxStackSize = 16; }

  ItemStack& onItemRightClick(ItemStack& is, EntityBase* clicker) {
    if (!is.decrementBy(1)) dynamic_cast<IPlayer*>(clicker)->updateEquipedItem();
    // todo spawn thrown snowball entity
    return is;
  }
};
