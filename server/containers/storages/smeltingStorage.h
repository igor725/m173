#pragma once

#include "../container.h"
#include "../storage.h"
#include "items/itemstack.h"

#include <algorithm>
#include <array>
#include <cstdint>

class SmeltingStorage: public IStorage {
  public:
  SmeltingStorage(IContainer* handler): m_handler(handler) {}

  ~SmeltingStorage() {}

  ItemStack& getByOffset(SlotId slot) final { return m_items[slot]; }

  SlotId getSlotId(const ItemStack& is) const final {
    auto it = std::find_if(m_items.begin(), m_items.end(), [&](const ItemStack& o) -> bool { return &o == &is; });
    if (it == m_items.end()) return -1;
    return (SlotId)std::distance(m_items.begin(), it);
  }

  void clear() final { m_items.fill(ItemStack()); }

  private:
  std::array<ItemStack, 3> m_items;
  IContainer*              m_handler;
};
