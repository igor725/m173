#pragma once

#include "../item.h"

class ItemArmor: public Item {
  public:
  ItemArmor(ItemId iid, ArmorType t): Item(iid), m_type(t) { maxStackSize = 1; }

  bool isValidType(ArmorType t) const { return m_type == t; }

  private:
  ArmorType m_type;
};
