#pragma once

#include "../slot.h"
#include "blocks/blockDB.h"
#include "items/item.h"
#include "items/list/armor.h"

class ArmorSlot: public ISlot {
  public:
  ArmorSlot(IStorage* stor, SlotId sid, ArmorType armorType): ISlot(stor, sid, ISlot::Armor), m_armorType(armorType) {}

  bool isItemValid(const ItemStack& is) const final;

  int16_t getSlotStackLimit() const final { return 64; }

  ArmorType getArmorType() const { return m_armorType; }

  private:
  bool armorTest(ItemId iid) const;

  private:
  ArmorType m_armorType;
};
