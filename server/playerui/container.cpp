#include "container.h"

#include "items/item.h"
#include "recipes/crafting/recipe.h"

#include <exception>
#include <spdlog/spdlog.h>

class FreeSlotException: public std::exception {
  public:
  FreeSlotException() {}

  const char* what() const noexcept override { return "getFreeSlot() function returned fully occupied slot!"; }
};

IContainer::IContainer(uint32_t slotsNum) {
  m_slots.reserve(slotsNum);
}

IContainer::~IContainer() {}

ISlot* IContainer::getSlot(SlotId sid) const {
  return m_slots.at(sid).get();
}

void IContainer::addSlot(std::unique_ptr<ISlot>&& slot) {
  slot->setAbsoluteSlotId(m_slots.size());
  m_slots.emplace_back(std::move(slot));
}

ItemStack& IContainer::getItem(SlotId sid) {
  return m_slots.at(sid)->getHeldItem();
}

bool IContainer::onClosed() {
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

bool IContainer::onSlotClicked(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) {
  bool transactFail = true, craftFlag = true;
  *updatedItem = nullptr;

  if (sid == SLOT_OFFSCREEN_CLICK) {
    // todo handle item dropping
    *updatedItem = &m_slots.at(0)->getHeldItem(); // Temporary workaround for disappearing crafting result item
  } else {
    auto  clickedSlot     = m_slots.at(sid).get(); // User will be kicked immediately if this slot does not exists
    auto& clickedSlotItem = clickedSlot->getHeldItem();

    /**
     * Probably this fuckery covers all the click cases,
     * need to optimize it someday. Minecraft inventory
     * behavior is quite inconsistent and this is why
     * this thing is so big. I'm sure it can be
     * shrinked, but I'm too lazy for this rn.
     *
     */
    if (isShift) {
      auto getFreeSlot = [this, &clickedSlot, &clickedSlotItem]() -> ISlot* {
        ISlot* lastFree = nullptr;

        switch (clickedSlot->getSlotType()) {
          case ISlot::Inventory: {
            for (auto it = m_slots.rbegin(); (it != m_slots.rend()) && ((*it)->getSlotType() == ISlot::Hotbar); ++it) {
              if ((*it).get() == clickedSlot) continue;
              auto slotItem = (*it)->getHeldItem();
              if ((slotItem.isEmpty() || slotItem.isSimilarTo(clickedSlotItem)) && slotItem.availStackRoom() > 0) lastFree = (*it).get();
            }
          } break;
          case ISlot::CraftRecipe: {
            for (auto it = m_slots.begin(); (it != m_slots.end()); ++it) {
              if ((*it).get() == clickedSlot) continue;
              auto type = (*it)->getSlotType();
              if (type != ISlot::Hotbar && type != ISlot::Inventory) continue;
              auto slotItem = (*it)->getHeldItem();
              if (slotItem.isSimilarTo(clickedSlotItem) && slotItem.availStackRoom() > 0) return (*it).get();
            }
          } break;
          case ISlot::Result: {
            for (auto it = m_slots.rbegin(); (it != m_slots.rend()) && ((*it)->getSlotType() == ISlot::Hotbar); ++it) {
              if ((*it).get() == clickedSlot) continue;
              auto slotItem = (*it)->getHeldItem();
              if ((slotItem.isEmpty() || slotItem.isSimilarTo(clickedSlotItem)) && slotItem.availStackRoom() > 0) return (*it).get();
            }
          } break;

          default: break;
        }

        if (lastFree) return lastFree;

        for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
          if ((*it).get() == clickedSlot) continue;
          auto type = (*it)->getSlotType();
          if (type != ISlot::Hotbar && type != ISlot::Inventory) continue;
          if ((*it)->isItemValid(clickedSlotItem) && (*it)->getAvailableRoom() > 0) {
            auto slotItem = (*it)->getHeldItem();
            if (slotItem.isEmpty() || slotItem.isSimilarTo(clickedSlotItem)) return (*it).get();
          }
        }

        return nullptr;
      };

      if (auto freeSlot = getFreeSlot()) {
        auto& itSlotItem = freeSlot->getHeldItem();
        if (itSlotItem.isEmpty() || itSlotItem.isSimilarTo(clickedSlotItem)) {
          auto maxPossibleTransfer = Item::getById(clickedSlotItem.itemId)->getStackLimit();
          if (itSlotItem.stackSize > 0) maxPossibleTransfer -= itSlotItem.stackSize;
          if (maxPossibleTransfer <= 0) throw FreeSlotException();
          if (clickedSlotItem.moveTo(itSlotItem, std::min(maxPossibleTransfer, clickedSlotItem.stackSize)) && clickedSlotItem.stackSize > 0) {
            return onSlotClicked(sid, isRmb, isShift, updatedItem);
          }

          // Should be always false
          transactFail   = (clickedSlotItem.stackSize > 0);
          updatedItem[0] = &itSlotItem;
          updatedItem[1] = &clickedSlotItem;
        }
      } else {
        transactFail = false;
      }
    } else {
      if (clickedSlotItem.isEmpty() && !m_carriedItem.isEmpty()) {
        if (clickedSlot->isItemValid(m_carriedItem)) {
          auto maxPossibleTransfer = std::min(isRmb ? int16_t(1) : m_carriedItem.stackSize, clickedSlot->getSlotStackLimit());
          if (maxPossibleTransfer > 0) transactFail = !m_carriedItem.splitStack(maxPossibleTransfer).moveTo(clickedSlotItem);
        } else {
          // Special case, we should not to do any resetting if non-applicable item passed to special slot
          transactFail = false;
        }
      } else if (!clickedSlotItem.isEmpty() && m_carriedItem.isEmpty()) {
        auto maxPossibleTransfer = isRmb ? (clickedSlotItem.stackSize + 1) / 2 : clickedSlotItem.stackSize;
        if (maxPossibleTransfer > 0) {
          m_carriedItem     = clickedSlotItem.splitStack(maxPossibleTransfer);
          m_carriedItemFrom = sid;
          transactFail      = false;
        }
      } else if (clickedSlot->isItemValid(m_carriedItem)) {
        if (clickedSlotItem.isSimilarTo(m_carriedItem)) {
          auto maxPossibleTransfer = isRmb ? int16_t(1) : m_carriedItem.stackSize;
          maxPossibleTransfer      = std::min(maxPossibleTransfer, clickedSlot->getAvailableRoom());
          transactFail             = !m_carriedItem.moveTo(clickedSlotItem, maxPossibleTransfer);
        } else {
          if (m_carriedItem.stackSize <= clickedSlot->getSlotStackLimit()) {
            m_carriedItem.swapWith(clickedSlotItem);
            transactFail = false;
          }
        }
      } else if (clickedSlot->getSlotType() == ISlot::Result) {
        auto maxPossibleTransfer = std::min(clickedSlotItem.stackSize, m_carriedItem.availStackRoom());
        if (maxPossibleTransfer > 0) {
          transactFail = !clickedSlotItem.splitStack(maxPossibleTransfer).moveTo(m_carriedItem);
        } else {
          craftFlag    = false;
          transactFail = false;
        }
      } else {
        // Another special case, no resetting needed there too
        transactFail = false;
      }
    }

    if (!transactFail) {
      switch (clickedSlot->getSlotType()) {
        case ISlot::CraftRecipe: {
          *updatedItem = onCraftingUpdated();
        } break;
        case ISlot::Result: {
          if (craftFlag)
            *updatedItem = onCraftingDone();
          else
            *updatedItem = &clickedSlotItem;
        } break;

        default: break;
      }
    }

    if (*updatedItem == nullptr) *updatedItem = &clickedSlotItem;
  }

  if (transactFail && !m_carriedItem.isEmpty()) {
    m_carriedItem.moveTo(m_slots.at(m_carriedItemFrom)->getHeldItem(), m_carriedItem.stackSize);
    spdlog::warn("Some ItemStack carry operation failed!");
  }

  return !transactFail;
}

bool IContainer::getRecipe(ItemStack** array, ItemStack** result, uint8_t& rW, uint8_t& rH) {
  if (array == nullptr || result == nullptr) return false;
  rW = getRecipeWidth(), rH = getRecipeHeight();
  auto craftContSize = (rW * rH);
  if (craftContSize == 0 || craftContSize > 9) return false; // todo throw exception on craftContSize > 9? This should not happen like at all

  uint8_t index = 0;

  for (auto it = m_slots.begin(); (index < craftContSize) && (it != m_slots.end()); ++it) {
    if (auto slot = it->get()) {
      switch (slot->getSlotType()) {
        case ISlot::Result: {
          *result = &slot->getHeldItem();
        } break;
        case ISlot::CraftRecipe: {
          auto cidx = index++;
          // Filling the recipe array with crafting slots items
          array[(cidx / rW) * rH + (cidx % rW)] = &slot->getHeldItem();
        } break;

        default: break;
      }
    }
  }

  return index > 0 && *result != nullptr;
}

ItemStack* IContainer::onCraftingUpdated() {
  ItemStack* resultSlot = nullptr;

  if (CraftingRecipe::scan(this, &resultSlot)) {
    // todo???
    return resultSlot;
  }

  return nullptr;
}

ItemStack* IContainer::onCraftingDone() {
  for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
    if (auto slot = (*it).get()) {
      if (slot->getSlotType() == ISlot::CraftRecipe) {
        slot->getHeldItem().decrementBy(1);
      }
    }
  }

  return onCraftingUpdated();
}
