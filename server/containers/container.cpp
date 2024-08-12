#include "container.h"

#include "items/items.h"

IContainer::IContainer(uint32_t slotsNum) {
  m_slots.reserve(slotsNum);
}

IContainer::~IContainer() {}

void IContainer::addSlot(Slot&& slot) {
  slot.setAbsoluteSlotId(m_slots.size());
  m_slots.emplace_back(std::move(slot));
}

ItemStack& IContainer::getItem(SlotId sid) {
  return m_slots.at(sid).getHeldItem();
}

bool IContainer::onSlotClicked(SlotId sid, bool isRmb, bool shift) {
  if (sid == SLOT_OFFSCREEN_CLICK) {
    // todo handle item dropping
    return false;
  } else {
    auto  clickedSlot     = m_slots.at(sid); // User will be kicked immediately if this slot does not exists
    auto& clickedSlotItem = clickedSlot.getHeldItem();

    if (shift) {
      auto oppositeType = clickedSlot.getSlotType() == Slot::Hotbar ? Slot::Inventory : Slot::Hotbar;
      for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
        if (it->getSlotType() == oppositeType) {
          auto& itSlotItem = it->getHeldItem();
          if (itSlotItem.isEmpty() || itSlotItem.isSimilarTo(clickedSlotItem)) {
            auto maxPossibleTransfer = Item::getById(clickedSlotItem.itemId)->getStackLimit();
            if (itSlotItem.stackSize > 0) maxPossibleTransfer -= itSlotItem.stackSize;
            if (maxPossibleTransfer == 0) continue;
            clickedSlotItem.moveTo(itSlotItem, std::min(maxPossibleTransfer, clickedSlotItem.stackSize));
            return true;
          }
        }
      }
    }
  }

  return false;
}
