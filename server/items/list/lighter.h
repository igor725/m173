#pragma once

#include "../items.h"
#include "entity/player/player.h"
#include "world/world.h"

class ItemLighter: public Item {
  public:
  ItemLighter(ItemId iid): Item(iid) {
    maxStackSize = 1;
    maxDamage    = 64;
  }

  bool onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
    auto firepos = pos;

    switch (dir) {
      case 0: firepos.y -= 1; break;
      case 1: firepos.y += 1; break;
      case 2: firepos.z -= 1; break;
      case 3: firepos.z += 1; break;
      case 4: firepos.x -= 1; break;
      case 5: firepos.x += 1; break;
    }

    if (accessWorld().setBlockWithNotify(firepos, 51, 0, dynamic_cast<IPlayer*>(clicker))) {
      is.damageItem(1, clicker);
    }

    return true;
  }
};
