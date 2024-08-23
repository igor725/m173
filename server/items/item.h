#pragma once

#include "entity/entitybase.h"
#include "helper.h"
#include "itemstack.h"

#include <array>
#include <cstdint>

class Item {
  private:
  ItemId shiftedIndex;
  Item*  containerItem = nullptr;

  protected:
  int16_t maxDamage    = 0;
  int16_t maxStackSize = 64;
  bool    bFull3D      = false;
  bool    bHasSubTypes = false;

  public:
  static Item* getById(ItemId id);
  static bool  exists(ItemId iid);

  Item(ItemId iid);

  ~Item();

  virtual int8_t getMetadata(int16_t damage) const { return 0; }

  virtual bool onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) { return false; }

  virtual bool onBlockDestroyed(ItemStack& is, const IntVector3& pos, BlockId id, EntityBase* destroyer) { return true; }

  virtual bool onEquipedByEntity(ItemStack& is, EntityBase* equiper) { return true; }

  virtual bool onUseItemOnBlock(ItemStack& is, EntityBase* user, const IntVector3& pos, int8_t direction) { return false; }

  virtual bool hitEntity(ItemStack& is, EntityBase* attacker, EntityBase* victim) { return false; }

  int16_t getStackLimit() const { return maxStackSize; }

  int16_t getMaxDamage() const { return maxDamage; }

  bool isDamageable() { return maxDamage > 0 && !bHasSubTypes; }

  ItemId getId() const { return shiftedIndex; }

  virtual void getDamageVsEntity(EntityBase* ent, VsDamageInfo& vif) const { vif = VsDamageInfo(1, 0.1); }
};
