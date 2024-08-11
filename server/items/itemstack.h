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

  bool decrementBy(int16_t sz) {
    if (sz < 1 || stackSize < sz || itemId < 0) return false;
    if ((stackSize -= sz) == 0) itemId = -1;
    return stackSize >= 0;
  }

  bool incrementBy(int16_t sz) {
    if (sz < 1 || itemId < 0) return false;
    stackSize += sz;
    return true;
  }

  bool isDamageable();

  int16_t getMaxDamage();

  void onDestroyBlock(const IntVector3& pos, BlockId id, EntityBase* destroyer);

  void hitEntity(EntityBase* attacker, EntityBase* victim);

  void damageItem(int16_t damage, EntityBase* damager) {
    if (isDamageable()) {
      itemDamage += damage;
      if (itemDamage > getMaxDamage()) {
        --stackSize;
        if (stackSize < 0) stackSize = 0;
        itemDamage = 0;
      }
    }
  }

  const VsDamageInfo& getDamageVsEntity(EntityBase* ent);

  void fullStack() { stackSize = 64; }
};
