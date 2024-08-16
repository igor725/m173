#include "block.h"
#include "blocks/blockDB.h"
#include "bow.h"
#include "entity/manager.h"
#include "entity/objects/arrow.h"
#include "entity/objects/fishfloat.h"
#include "entity/objects/snowball.h"
#include "entity/player/player.h"
#include "fishingRod.h"
#include "items/itemDB.h"
#include "lighter.h"
#include "sign.h"
#include "snowBall.h"
#include "world/world.h"

#pragma region("block.h")

bool ItemBlock::onUseItemOnBlock(ItemStack& is, EntityBase* user, const IntVector3& pos, int8_t direction) {
  auto  ply  = dynamic_cast<IPlayer*>(user);
  auto& ppos = ply->getPosition();

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
  if (DoubleVector3(pos.x + 0.5, pos.y + 0.5, pos.z + 0.5).distanceTo(ppos) < 8.0) {
    if (is.decrementBy(1)) {
      if (world.setBlockWithNotify(npos, m_blockId, getMetadata(dmg), ply)) {
        return true;
      } else { // Uh oh
        is.incrementBy(1);
      }

      ply->resendItem(ply->getHeldItem());
    }
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

#pragma region("fishingRod.h")

bool ItemFishingRod::onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
  auto ply = dynamic_cast<IPlayer*>(clicker);

  if (auto attEnt = ply->getAttachedEntity()) {
    if (auto fishfloat = dynamic_cast<IFishFloat*>(attEnt)) {
      fishfloat->lure();
    }
  } else {
    if (is.damageItem(1, clicker)) {
      auto apos = ply->getPosition();
      apos.y += ply->getEyeHeight();
      accessEntityManager().AddEntity(createFishFloat(apos, ply->getEntityId(), ply->getForwardVector()));
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

#pragma region("sign.h")

bool ItemSign::onUseItemOnBlock(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) {
  if (dir == 0) return false;
  // todo check if block is solid

  auto placepos = pos;

  switch (dir) {
    case 1: placepos.y += 1; break;
    case 2: placepos.z -= 1; break;
    case 3: placepos.z += 1; break;
    case 4: placepos.x -= 1; break;
    case 5: placepos.x += 1; break;
  }

  if (is.decrementBy(1)) {
    int8_t signMeta = dir;
    if (dir == 1) {
      auto& prot = clicker->getRotation();
      signMeta   = static_cast<int8_t>(std::roundf(((prot.yaw + 180.0f) * 16.0f / 360.0f) + 0.5f));
    }

    return accessWorld().setBlockWithNotify(placepos, BlockDB::sign.getId(), signMeta, dynamic_cast<IPlayer*>(clicker));
  }

  return false;
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
