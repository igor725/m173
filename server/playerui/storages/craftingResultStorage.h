#pragma once

#include "../storage.h"
#include "items/itemstack.h"

class CraftingResultStorage: public IStorage {
  public:
  CraftingResultStorage() {}

  ~CraftingResultStorage() {}

  ItemStack& getByOffset(SlotId slot) final { return m_result; }

  SlotId getSlotId(const ItemStack& is) const final { return &is == &m_result ? 0 : -1; }

  void clear() final { m_result = ItemStack(); }

  private:
  ItemStack m_result;
};
