#pragma once

#include "../items.h"
#include "world/world.h"

class ItemBlock: public Item {
  public:
  ItemBlock(BlockId bid): Item((ItemId)bid - 256), m_blockId(bid) { maxStackSize = 64; }

  bool onUseItem(ItemStack& is, EntityBase* user, const IntVector3& pos, int8_t direction) override {
    auto  ply   = dynamic_cast<IPlayer*>(user);
    auto& world = accessWorld();
    auto  npos  = pos;

    if (world.getBlock(pos) == 78) {
      direction = 0;
    } else {
      switch (direction) {
        case 0: npos.y -= 1; break;
        case 1: npos.y += 1; break;
        case 2: npos.z -= 1; break;
        case 3: npos.z += 1; break;
        case 4: npos.x -= 1; break;
        case 5: npos.x += 1; break;
      }
    }

    if (is.decrementBy(1)) {
      if (world.setBlockWithNotify(npos, m_blockId, getMetadata(is.itemDamage), ply)) {
        return true;
      } else { // Uh oh
        is.incrementBy(1);
      }

      ply->updateEquipedItem();
    }

    return false;
  }

  private:
  BlockId m_blockId;
};
