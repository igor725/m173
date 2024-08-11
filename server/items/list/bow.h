#pragma once

#include "../items.h"
#include "entity/manager.h"
#include "entity/objects/arrow.h"
#include "entity/player/player.h"

class ItemBow: public Item {
  public:
  ItemBow(ItemId iid): Item(iid) { maxStackSize = 1; }

  ItemStack& onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
    auto ply = dynamic_cast<IPlayer*>(clicker);

    auto sid = ply->findItemById(Item::getById(262)->shiftedIndex);

    if (sid != -1) {
      auto& is = ply->getItemBySlotId(sid);
      if (is.decrementBy(1)) {
        ply->resendItem(is);
        auto apos = ply->getPosition();
        apos.y += ply->getEyeHeight();
        accessEntityManager().AddEntity(createArrow(apos, ply->getEntityId(), ply->getForwardVector()));
      }
    }

    return is;
  }
};
