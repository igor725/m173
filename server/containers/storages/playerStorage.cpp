#include "playerStorage.h"

SlotId PlayerStorage::getSlotId(const ItemStack& is) const {
  return findSlotId([&](const ItemStack& o) -> bool { return &o == &is; });
}

SlotId PlayerStorage::findItemSlotId(ItemId iid) const {
  return findSlotId([&](const ItemStack& o) -> bool { return o.itemId == iid; });
}

bool PlayerStorage::push(const ItemStack& is, SlotId* sid) {
  for (auto it = m_items.rbegin(); it != m_items.rend(); ++it) {
    if (it->isEmpty()) {
      if (sid != nullptr) *sid = (SlotId)std::distance(m_items.begin(), it.base()) - 1;
      *it = is;
      return true;
    }
  }

  return false;
}
