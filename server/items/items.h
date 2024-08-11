#pragma once

#include "entity/entitybase.h"
#include "helper.h"
#include "itemstack.h"

#include <array>
#include <cstdint>
#include <spdlog/spdlog.h>

class Item {
  public:
  ItemId shiftedIndex;

  private:
  int16_t maxDamage     = 0;
  Item*   containerItem = nullptr;

  protected:
  int16_t maxStackSize;
  int32_t iconIndex;
  bool    bFull3D      = false;
  bool    bHasSubTypes = false;

  public:
  static Item* getById(ItemId id);

  Item(ItemId iid);

  ~Item();

  Item* setIconIndex(int32_t ii) {
    iconIndex = ii;
    return this;
  }

  Item* setIconIndexCoord(int32_t x, int32_t y) {
    iconIndex = y + x * 16;
    return this;
  }

  int32_t getIconFromDamage(int dmg) { return iconIndex; }

  int32_t getIconIndex(const ItemStack& is) { return getIconFromDamage(is.itemDamage); }

  Item* setMaxStackSize(int16_t size) {
    maxStackSize = size;
    return this;
  }

  virtual int8_t getMetadata(int16_t damage) { return 0; }

  virtual ItemStack& onItemRightClick(ItemStack& is, EntityBase* clicker, const IntVector3& pos, int8_t dir) { return is; }

  virtual bool onBlockDestroyed(ItemStack& is, const IntVector3& pos, BlockId id, EntityBase* destroyer) { return true; }

  virtual bool onUseItem(ItemStack& is, EntityBase* user, const IntVector3& pos, int8_t direction) { return false; }

  int16_t getStackLimit() const { return maxStackSize; }

  Item* setMaxDamage(int16_t md) {
    maxDamage = md;
    return this;
  }

  int16_t getMaxDamage() const { return maxDamage; }

  bool isDamageable() { return maxDamage > 0 && !bHasSubTypes; }

  virtual bool hitEntity(ItemStack& is, EntityBase* attacker, EntityBase* victim) { return false; }

  virtual const VsDamageInfo& getDamageVsEntity(EntityBase* ent) {
    static const VsDamageInfo baseDamage(1, 0.1);
    return baseDamage;
  }
};
