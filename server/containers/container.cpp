#include "container.h"

#include "items/items.h"

#include <spdlog/spdlog.h>

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
            return clickedSlotItem.moveTo(itSlotItem, std::min(maxPossibleTransfer, clickedSlotItem.stackSize));
          }
        }
      }
    } else {
      if (clickedSlotItem.isEmpty()) {
        if (!m_carriedItem.isEmpty() && clickedSlot.isItemValid(m_carriedItem)) {
          auto maxPossibleTransfer = std::min(isRmb ? int16_t(1) : m_carriedItem.stackSize, clickedSlot.getSlotStackLimit());
          if (maxPossibleTransfer == 0) return false;
          clickedSlotItem = m_carriedItem.splitStack(maxPossibleTransfer);
          return true;
        }
      } else if (m_carriedItem.isEmpty()) {
        auto maxPossibleTransfer = isRmb ? (clickedSlotItem.stackSize + 1) / 2 : clickedSlotItem.stackSize;
        if (maxPossibleTransfer == 0) return false;
        m_carriedItem     = clickedSlotItem.splitStack(maxPossibleTransfer);
        m_carriedItemFrom = sid;
        return true;
      } else if (clickedSlot.isItemValid(m_carriedItem)) {
        if (clickedSlotItem.isSimilarTo(m_carriedItem)) {
          auto maxPossibleTransfer = isRmb ? int16_t(1) : m_carriedItem.stackSize;
          maxPossibleTransfer      = std::min(maxPossibleTransfer, clickedSlot.getAvailableRoom());
          if (m_carriedItem.moveTo(clickedSlotItem, maxPossibleTransfer)) return true;
        } else {
          if (m_carriedItem.stackSize <= clickedSlot.getSlotStackLimit()) {
            m_carriedItem.swapWith(clickedSlotItem);
            return true;
          }
        }
      }
    }
  }

  if (!m_carriedItem.isEmpty()) {
    m_carriedItem.moveTo(m_slots.at(m_carriedItemFrom).getHeldItem(), m_carriedItem.stackSize);
    spdlog::warn("Some ItemStack carry operation failed!");
  }

  return false;
}
