#pragma once

#include "../storage.h"
#include "items/itemstack.h"

#include <algorithm>
#include <array>
#include <cstddef>

class PlayerStorage: public IStorage {
  public:
  PlayerStorage(): IStorage() {}

  SlotId getArmorOffset() const { return 0; }

  SlotId getInventoryOffset() const { return 4; }

  SlotId getHotbarOffset() const { return 31; }

  ItemStack& getByOffset(SlotId slot) final { return m_items[slot]; }

  int16_t getSize() const final { return m_items.size(); }

  void clear() final { m_items.fill(ItemStack()); }

  template <typename T>
  SlotId findSlotId(T pred) const {
    auto it = std::find_if(m_items.begin(), m_items.end(), pred);
    if (it == m_items.end()) return -1;
    return (SlotId)std::distance(m_items.begin(), it);
  }

  SlotId getSlotId(const ItemStack& is) const final;

  SlotId findItemSlotId(ItemId iid) const final;

  bool push(const ItemStack& is, SlotId* sid = nullptr, SlotId prioritySlot = -1) final;

  private:
  std::array<ItemStack, 40> m_items;
};
