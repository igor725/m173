#include "block.h"
#include "blocks/blockDB.h"
#include "bow.h"
#include "entity/manager.h"
#include "entity/objects/arrow.h"
#include "entity/objects/snowball.h"
#include "entity/player/player.h"
#include "items/itemDB.h"
#include "lighter.h"
#include "snowBall.h"
#include "world/world.h"

#pragma region("block.h")

bool ItemBlock::onUseItemOnBlock(ItemStack& is, EntityBase* user, const IntVector3& pos, int8_t direction) {
  auto  ply   = dynamic_cast<IPlayer*>(user);
  auto& world = accessWorld();
  auto  npos  = pos;

  if (world.getBlock(pos) == BlockDB::snow.getId()) {
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

  // Should be retrieved before being eaten by ItemStack::decrementBy()
  auto dmg = is.itemDamage;
  if (is.decrementBy(1)) {
    if (world.setBlockWithNotify(npos, m_blockId, getMetadata(dmg), ply)) {
      return true;
    } else { // Uh oh
      is.incrementBy(1);
    }

    ply->updateEquipedItem();
  }

  return false;
}

#pragma endregion()

#pragma region("bow.h")

bool ItemBow::onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
  auto  ply  = dynamic_cast<IPlayer*>(clicker);
  auto& stor = ply->getStorage();

  auto sid = stor.findItemSlotId(ItemDB::arrow.getId());

  if (sid != -1) {
    auto& arrow_is = stor.getByOffset(sid);
    if (arrow_is.decrementBy(1)) {
      ply->resendItem(arrow_is);
      auto apos = ply->getPosition();
      apos.y += ply->getEyeHeight();
      accessEntityManager().AddEntity(createArrow(apos, ply->getEntityId(), ply->getForwardVector()));
    }
  }

  return true;
}

#pragma endregion()

#pragma region("lighter.h")

bool ItemLighter::onUseItemOnBlock(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
  auto firepos = pos;

  switch (dir) {
    case 0: firepos.y -= 1; break;
    case 1: firepos.y += 1; break;
    case 2: firepos.z -= 1; break;
    case 3: firepos.z += 1; break;
    case 4: firepos.x -= 1; break;
    case 5: firepos.x += 1; break;
  }

  if (accessWorld().setBlockWithNotify(firepos, BlockDB::fire.getId(), 0, dynamic_cast<IPlayer*>(clicker))) {
    is.damageItem(1, clicker);
  }

  return true;
}

#pragma endregion()

#pragma region("snowball.h")

bool ItemSnowball::onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
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

#pragma endregion()
