#pragma once

#include "../item.h"
#include "../toolmaterial.h"

class ItemSword: public Item {
  private:
  const ToolMaterial& m_tm;
  VsDamageInfo        m_damage;

  public:
  ItemSword(ItemId iid, ToolMaterial::Index m): Item(iid), m_tm(ToolMaterial::select(m)) {
    m_damage     = VsDamageInfo(4 + m_tm.getDamageVsEntity() * 2, 0.08 + (m_tm.getDamageVsEntity() / 15.0));
    maxDamage    = m_tm.getMaxUses();
    maxStackSize = 1;
  }

  bool hitEntity(ItemStack& is, Entities::Base* atacker, Entities::Base* victim) override {
    is.damageItem(1, atacker);
    return true;
  }

  void getDamageVsEntity(Entities::Base* ent, VsDamageInfo& vif) const override {
    // todo adjust damage based on entity armor
    vif = m_damage;
  }

  bool onBlockDestroyed(ItemStack& is, const IntVector3& pos, BlockId id, Entities::Base* destroyer) override {
    is.damageItem(2, destroyer);
    return true;
  }
};
