#pragma once

#include "entity/entitybase.h"
#include "helper.h"

struct ItemStack {
  int16_t stackSize;
  ItemId  itemId;
  int16_t itemDamage;

  ItemStack(): stackSize(0), itemId(-1), itemDamage(0) {}

  ItemStack(ItemId iid): stackSize(0), itemId(iid), itemDamage(0) {}

  ItemStack(ItemId iid, int16_t ss): stackSize(ss), itemId(iid), itemDamage(0) {}

  ItemStack(ItemId iid, int16_t ss, int16_t dmg): stackSize(ss), itemId(iid), itemDamage(dmg) {}

  ItemStack(const ItemStack& is, int16_t ss): itemId(is.itemId), itemDamage(is.itemDamage), stackSize(ss) {}

  bool decrementBy(int16_t sz);

  bool incrementBy(int16_t sz);

  bool isEmpty() const { return itemId == -1 || stackSize == 0; }

  bool isSimilarTo(const ItemStack& is) const { return itemId == is.itemId && itemDamage == is.itemDamage; }

  bool moveTo(ItemStack& is, int16_t count);

  void swapWith(ItemStack& is);

  bool isDamageable() const;

  int16_t getMaxDamage() const;

  const VsDamageInfo& getDamageVsEntity(EntityBase* ent) const;

  void onDestroyBlock(const IntVector3& pos, BlockId id, EntityBase* destroyer);

  void hitEntity(EntityBase* attacker, EntityBase* victim);

  bool useItem(EntityBase* user, const IntVector3& pos, int8_t direction);

  void damageItem(int16_t damage, EntityBase* damager);

  ItemStack splitStack(int16_t count);

  void fullStack() { stackSize = 64; }
};
