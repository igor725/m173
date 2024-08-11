#include "itemstack.h"

#include "items.h"

bool ItemStack::isDamageable() {
  return Item::getById(itemId)->isDamageable();
}

int16_t ItemStack::getMaxDamage() {
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

const VsDamageInfo& ItemStack::getDamageVsEntity(EntityBase* ent) {
  return Item::getById(itemId)->getDamageVsEntity(ent);
}
