#pragma once

#include "../storage.h"
#include "items/itemstack.h"

#include <cstdint>

template <int8_t X, int8_t Y>
class CraftingStorage: public IStorage {
  public:
  CraftingStorage(IContainer* handler): m_handler(handler) {}

  ~CraftingStorage() {}

  ItemStack& getByOffset(SlotId slot) final { return m_items[slot]; }

  SlotId getSlotId(const ItemStack& is) final {
    auto it = std::find_if(m_items.begin(), m_items.end(), [&](const ItemStack& o) -> bool { return &o == &is; });
    if (it == m_items.end()) return -1;
    return (SlotId)std::distance(m_items.begin(), it);
  }

  void clear() final { m_items.fill(ItemStack()); }

  private:
  std::array<ItemStack, X * Y> m_items;
  IContainer*                  m_handler;
};
