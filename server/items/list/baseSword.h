#pragma once

#include "../items.h"

class ItemSword: public Item {
  public:
  enum Material {
    Wood,
    Stone,
    Iron,
    Gold,
    Diamond,
  };

  ItemSword(ItemId iid, Material m): Item(iid) {}

  bool hitEntity(ItemStack& is, EntityBase* atacker, EntityBase* victim) override {
    is.damageItem(1, atacker);
    return true;
  }

  const ItemStack::VsDamageInfo& getDamageVsEntity(EntityBase* ent) {
    static const ItemStack::VsDamageInfo baseDamage(4, 0.24);
    return baseDamage;
  }

  bool onBlockDestroyed(ItemStack& is, const IntVector3& pos, BlockId id, EntityBase* destroyer) override {
    is.damageItem(2, destroyer);
    return true;
  }
};
