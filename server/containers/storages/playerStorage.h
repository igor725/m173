#pragma once

#include "../storage.h"
#include "items/itemstack.h"

#include <array>

class PlayerStorage: public IStorage {
  public:
  PlayerStorage() {}

  SlotId getArmorOffset() const { return 0; }

  SlotId getInventoryOffset() const { return 4; }

  SlotId getHotbarOffset() const { return 31; }

  ItemStack& getByOffset(SlotId slot) final { return m_items[slot]; }

  size_t getSize() const final { return m_items.size(); }

  void clear() final { m_items.fill(ItemStack()); }

  template <typename T>
  SlotId findSlotId(T pred) {
    auto it = std::find_if(m_items.begin(), m_items.end(), pred);
    if (it == m_items.end()) return -1;
    return (SlotId)std::distance(m_items.begin(), it);
  }

  SlotId getSlotId(const ItemStack& is) final {
    return findSlotId([&](const ItemStack& o) -> bool { return &o == &is; });
  }

  SlotId findItemSlotId(ItemId iid) final {
    return findSlotId([&](const ItemStack& o) -> bool { return o.itemId == iid; });
  }

  bool push(const ItemStack& is, SlotId* sid = nullptr) final {
    for (auto it = m_items.rbegin(); it != m_items.rend(); ++it) {
      if (it->isEmpty()) {
        if (sid != nullptr) *sid = (SlotId)std::distance(m_items.rbegin(), it);
        *it = is;
        return true;
      }
    }

    return false;
  }

  private:
  std::array<ItemStack, 40> m_items;
};
