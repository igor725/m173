#include "container.h"

#include "items/item.h"

#include <spdlog/spdlog.h>

IContainer::IContainer(uint32_t slotsNum) {
  m_slots.reserve(slotsNum);
}

IContainer::~IContainer() {}

void IContainer::addSlot(std::unique_ptr<ISlot>&& slot) {
  slot->setAbsoluteSlotId(m_slots.size());
  m_slots.emplace_back(std::move(slot));
}

ItemStack& IContainer::getItem(SlotId sid) {
  return m_slots.at(sid)->getHeldItem();
}

bool IContainer::onWindowClosed() {
  bool shouldUpdateInv = false;

  for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
    if (auto slot = it->get()) {
      if (slot->getSlotType() != ISlot::CraftRecipe) continue;
      auto& craftItem = slot->getHeldItem();
      if (craftItem.isEmpty()) continue;
      for (auto it_r = m_slots.rbegin(); it_r != m_slots.rend(); ++it_r) {
        if (auto slot_r = it_r->get()) {
          switch (slot_r->getSlotType()) {
            case ISlot::Type::Inventory:
            case ISlot::Type::Hotbar: break;
            default: continue;
          }

          if (craftItem.moveTo(slot_r->getHeldItem())) {
            shouldUpdateInv = true;
            break;
          }
        }
      }
    }
  }

  return shouldUpdateInv;
}

bool IContainer::onSlotClicked(SlotId sid, bool isRmb, bool shift) {
  bool putCarriedItemBack = true;

  if (sid == SLOT_OFFSCREEN_CLICK) {
    // todo handle item dropping
  } else {
    auto  clickedSlot     = m_slots.at(sid).get(); // User will be kicked immediately if this slot does not exists
    auto& clickedSlotItem = clickedSlot->getHeldItem();

    if (shift) {
      auto oppositeType = clickedSlot->getSlotType() == ISlot::Hotbar ? ISlot::Inventory : ISlot::Hotbar;
      for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
        if (it->get()->getSlotType() == oppositeType) {
          auto& itSlotItem = it->get()->getHeldItem();
          if (itSlotItem.isEmpty() || itSlotItem.isSimilarTo(clickedSlotItem)) {
            auto maxPossibleTransfer = Item::getById(clickedSlotItem.itemId)->getStackLimit();
            if (itSlotItem.stackSize > 0) maxPossibleTransfer -= itSlotItem.stackSize;
            if (maxPossibleTransfer == 0) continue;
            return clickedSlotItem.moveTo(itSlotItem, std::min(maxPossibleTransfer, clickedSlotItem.stackSize));
          }
        }
      }
    } else {
      if (clickedSlotItem.isEmpty() && !m_carriedItem.isEmpty()) {
        if (clickedSlot->isItemValid(m_carriedItem)) {
          auto maxPossibleTransfer = std::min(isRmb ? int16_t(1) : m_carriedItem.stackSize, clickedSlot->getSlotStackLimit());
          if (maxPossibleTransfer == 0) return false;
          clickedSlotItem = m_carriedItem.splitStack(maxPossibleTransfer);
        }
        // Special case, we should not to do anything resetting if non-applicable item passed to special slot
        return true;
      } else if (m_carriedItem.isEmpty()) {
        auto maxPossibleTransfer = isRmb ? (clickedSlotItem.stackSize + 1) / 2 : clickedSlotItem.stackSize;
        if (maxPossibleTransfer == 0) return false;
        m_carriedItem     = clickedSlotItem.splitStack(maxPossibleTransfer);
        m_carriedItemFrom = sid;
        return true;
      } else if (clickedSlot->isItemValid(m_carriedItem)) {
        if (clickedSlotItem.isSimilarTo(m_carriedItem)) {
          auto maxPossibleTransfer = isRmb ? int16_t(1) : m_carriedItem.stackSize;
          maxPossibleTransfer      = std::min(maxPossibleTransfer, clickedSlot->getAvailableRoom());
          m_carriedItem.moveTo(clickedSlotItem, maxPossibleTransfer);
          return true;
        } else {
          if (m_carriedItem.stackSize <= clickedSlot->getSlotStackLimit()) {
            m_carriedItem.swapWith(clickedSlotItem);
            return true;
          }
        }
      } else {
        // Another special case, no resetting needed there
        return true;
      }
    }
  }

  if (putCarriedItemBack && !m_carriedItem.isEmpty()) {
    m_carriedItem.moveTo(m_slots.at(m_carriedItemFrom)->getHeldItem(), m_carriedItem.stackSize);
    spdlog::warn("Some ItemStack carry operation failed!");
  }

  return false;
}
