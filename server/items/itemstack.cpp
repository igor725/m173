#include "itemstack.h"

#include "items.h"

bool ItemStack::decrementBy(int16_t sz) {
  if (sz < 1 || stackSize < sz || itemId < 0) return false;
  if ((stackSize -= sz) == 0) itemId = -1;
  return stackSize >= 0;
}

bool ItemStack::incrementBy(int16_t sz) {
  if (sz < 1 || itemId < 0) return false;
  stackSize += sz;
  return true;
}

bool ItemStack::isDamageable() const {
  return Item::getById(itemId)->isDamageable();
}

int16_t ItemStack::getMaxDamage() const {
  return Item::getById(itemId)->getMaxDamage();
}

void ItemStack::onDestroyBlock(const IntVector3& pos, BlockId id, EntityBase* destroyer) {
  Item::getById(itemId)->onBlockDestroyed(*this, pos, id, destroyer);
}

void ItemStack::hitEntity(EntityBase* attacker, EntityBase* victim) {
  Item::getById(itemId)->hitEntity(*this, attacker, victim);
}

bool ItemStack::useItem(EntityBase* user, const IntVector3& pos, int8_t direction) {
  return Item::getById(itemId)->onUseItem(*this, user, pos, direction);
}

const VsDamageInfo& ItemStack::getDamageVsEntity(EntityBase* ent) const {
  return Item::getById(itemId)->getDamageVsEntity(ent);
}

void ItemStack::damageItem(int16_t damage, EntityBase* damager) {
  if (isDamageable()) {
    itemDamage += damage;
    if (itemDamage > getMaxDamage()) {
      --stackSize;
      if (stackSize < 0) stackSize = 0;
      itemDamage = 0;
    }
  }
}

ItemStack ItemStack::splitStack(int16_t count) {
  stackSize -= count;
  return ItemStack(itemId, count, itemDamage);
}

void ItemStack::moveTo(ItemStack& is, int16_t count) {
  if (is.isEmpty()) is = ItemStack(itemId, count, itemDamage);
  if ((stackSize -= count) == 0) *this = ItemStack(); // Clear the ItemStack if it's 0
}
