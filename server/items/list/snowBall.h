#pragma once

#include "../items.h"
#include "entity/manager.h"
#include "entity/objects/snowball.h"
#include "entity/player/player.h"

class ItemSnowball: public Item {
  public:
  ItemSnowball(ItemId iid): Item(iid) { maxStackSize = 16; }

  bool onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
    auto ply = dynamic_cast<IPlayer*>(clicker);
    if (!is.decrementBy(1)) {
      ply->updateEquipedItem();
      return true;
    }

    auto apos = ply->getPosition();
    apos.y += ply->getEyeHeight();
    accessEntityManager().AddEntity(createSnowBall(apos, ply->getEntityId(), ply->getForwardVector()));
    // todo spawn thrown snowball entity
    return true;
  }
};
