#pragma once

#include "entity/entitybase.h"
#include "entry/helper.h"

struct ItemStack {
  int16_t stackSize;
  ItemId  itemId;
  int16_t itemDamage;

  ItemStack(): stackSize(0), itemId(-1), itemDamage(0) {}

  ItemStack(ItemId iid): stackSize(1), itemId(iid), itemDamage(0) {}

  ItemStack(ItemId iid, int16_t ss): stackSize(ss), itemId(iid), itemDamage(0) {}

  ItemStack(ItemId iid, int16_t ss, int16_t dmg): stackSize(ss), itemId(iid), itemDamage(dmg) {}

  ItemStack(const ItemStack& is, int16_t ss): stackSize(ss), itemId(is.itemId), itemDamage(is.itemDamage) {}

  bool validate() const;

  bool decrementBy(uint16_t sz);

  bool incrementBy(uint16_t sz);

  bool isFull() const { return availStackRoom() == 0; }

  int16_t availStackRoom() const;

  bool isEmpty() const { return itemId == -1 || stackSize == 0; }

  bool isSimilarTo(const ItemStack& is) const { return itemId == is.itemId && itemDamage == is.itemDamage; }

  bool moveTo(ItemStack& is, int16_t count);
  bool moveTo(ItemStack& is);

  void swapWith(ItemStack& is);

  bool isDamageable() const;

  int16_t getMaxDamage() const;

  void getDamageVsEntity(Entities::Base* ent, VsDamageInfo& vif) const;

  void onDestroyBlock(const IntVector3& pos, BlockId id, Entities::Base* destroyer);

  void hitEntity(Entities::Base* attacker, Entities::Base* victim);

  bool useItemOnBlock(Entities::Base* user, const IntVector3& pos, int8_t direction);

  bool damageItem(int16_t damage, Entities::Base* damager);

  ItemStack splitStack(int16_t count);
};
