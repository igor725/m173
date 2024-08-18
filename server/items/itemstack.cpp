#include "itemstack.h"

#include "item.h"

bool ItemStack::validate() const {
  return Item::exists(itemId);
}

bool ItemStack::decrementBy(uint16_t sz) {
  if (sz < 1 || stackSize < sz || itemId < 0) return false;
  if ((stackSize -= sz) == 0) *this = ItemStack();
  return stackSize >= 0;
}

bool ItemStack::incrementBy(uint16_t sz) {
  if (sz < 1 || itemId < 0 || (Item::getById(itemId)->getStackLimit() < (stackSize + sz))) return false;
  stackSize += sz;
  return true;
}

int16_t ItemStack::availStackRoom() const {
  return std::max(Item::getById(itemId)->getStackLimit() - stackSize, 0);
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

bool ItemStack::useItemOnBlock(EntityBase* user, const IntVector3& pos, int8_t direction) {
  return Item::getById(itemId)->onUseItemOnBlock(*this, user, pos, direction);
}

void ItemStack::getDamageVsEntity(EntityBase* ent, VsDamageInfo& vif) const {
  return Item::getById(itemId)->getDamageVsEntity(ent, vif);
}

bool ItemStack::damageItem(int16_t damage, EntityBase* damager) {
  if (isDamageable()) {
    itemDamage += damage;
    if (itemDamage > getMaxDamage()) {
      --stackSize;
      if (stackSize < 0) *this = ItemStack();
      return false;
    }

    return true;
  }

  return false;
}

ItemStack ItemStack::splitStack(int16_t count) {
  auto _ns = ItemStack(itemId, count, itemDamage);
  if ((stackSize -= count) == 0) *this = ItemStack();
  return _ns;
}

bool ItemStack::moveTo(ItemStack& is, int16_t count) {
  if (is.isEmpty()) {
    is = ItemStack(itemId, count, itemDamage);
  } else if (!isSimilarTo(is) || !is.incrementBy(count)) {
    return false;
  }
  if ((stackSize -= count) == 0) *this = ItemStack();
  return true;
}

bool ItemStack::moveTo(ItemStack& is) {
  return moveTo(is, stackSize);
}

void ItemStack::swapWith(ItemStack& is) {
  auto _copy = *this;

  *this = is;
  is    = _copy;
}
