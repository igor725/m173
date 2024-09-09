#include "playerStorage.h"

SlotId PlayerStorage::getSlotId(const ItemStack& is) const {
  return findSlotId([&](const ItemStack& o) -> bool { return &o == &is; });
}

SlotId PlayerStorage::findItemSlotId(ItemId iid) const {
  return findSlotId([&](const ItemStack& o) -> bool { return o.itemId == iid; });
}

bool PlayerStorage::push(const ItemStack& is, SlotId* sid, SlotId prioritySlot) {
  if (!is.validate()) {
    if (sid != nullptr) *sid = -1;
    return false;
  }

  if (prioritySlot > -1 && prioritySlot < m_items.size()) {
    auto& pslot = m_items[prioritySlot];
    if (pslot.isEmpty() || (pslot.isSimilarTo(is) && pslot.incrementBy(is.stackSize))) {
      pslot = is;
      if (sid != nullptr) *sid = prioritySlot;
      return true;
    }
  }

  for (auto it = m_items.rbegin(); it != m_items.rend(); ++it) {
    if (it->isEmpty()) {
      *it = is;
      if (sid != nullptr) *sid = (SlotId)std::distance(m_items.begin(), it.base()) - 1;
      return true;
    }
  }

  if (sid != nullptr) *sid = -1;
  return false;
}
