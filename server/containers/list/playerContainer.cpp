#include "playerContainer.h"

#include "../slots/armorSlot.h"

PlayerContainer::PlayerContainer(PlayerStorage* stor): IContainer(45), m_crafting(this), m_craftRes(), m_storage(stor) {
  addSlot(Slot(&m_craftRes, 0));

  for (int32_t y = 0; y < 2; ++y) {
    for (int32_t x = 0; x < 2; ++x) {
      addSlot(Slot(&m_crafting, x + y * 2));
    }
  }

  for (int32_t i = 0; i < 4; ++i) {
    addSlot(ArmorSlot(stor, stor->getArmorOffset() + i, i));
  }

  for (int32_t y = 0; y < 3; ++y) {
    for (int32_t x = 0; x < 9; ++x) {
      addSlot(Slot(stor, stor->getInventoryOffset() + x + y * 9));
    }
  }

  for (int32_t i = 0; i < 9; ++i) {
    addSlot(Slot(stor, stor->getHotbarOffset() + i));
  }
}

PlayerContainer::~PlayerContainer() {}

SlotId PlayerContainer::getItemSlotById(ItemId iid) {
  return std::find_if(m_slots.begin(), m_slots.end(), [=](Slot& s) -> bool { return s.getHeldItem().itemId == iid; })->getSlotId();
}

ItemStack& PlayerContainer::getHotbarItem(uint8_t offset) {
  return m_storage->getByOffset(m_storage->getHotbarOffset() + offset);
}
