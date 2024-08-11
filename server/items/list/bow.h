#pragma once

#include "../items.h"
#include "entity/manager.h"
#include "entity/objects/arrow.h"
#include "entity/player/player.h"

class ItemBow: public Item {
  public:
  ItemBow(ItemId iid): Item(iid) { maxStackSize = 1; }

  ItemStack& onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
    auto  ply  = dynamic_cast<IPlayer*>(clicker);
    auto& stor = ply->getStorage();

    auto sid = stor.findItemSlotId(262 /* Search for bow */);

    if (sid != -1) {
      auto& arrow_is = stor.getByOffset(sid);
      if (arrow_is.decrementBy(1)) {
        ply->resendItem(arrow_is);
        auto apos = ply->getPosition();
        apos.y += ply->getEyeHeight();
        accessEntityManager().AddEntity(createArrow(apos, ply->getEntityId(), ply->getForwardVector()));
      }
    }

    return is;
  }
};
