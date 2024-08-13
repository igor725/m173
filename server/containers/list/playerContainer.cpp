#include "playerContainer.h"

#include "../slots/armorSlot.h"
#include "../slots/basicSlot.h"

PlayerContainer::PlayerContainer(PlayerStorage* stor): IContainer(45), m_crafting(this), m_craftRes(), m_storage(stor) {
  addSlot(std::make_unique<BasicSlot>(&m_craftRes, 0, ISlot::Result));

  for (int32_t y = 0; y < 2; ++y) {
    for (int32_t x = 0; x < 2; ++x) {
      addSlot(std::make_unique<BasicSlot>(&m_crafting, x + y * 2, ISlot::CraftRecipe));
    }
  }

  for (int32_t i = 0; i < 4; ++i) {
    addSlot(std::make_unique<ArmorSlot>(stor, stor->getArmorOffset() + i, ArmorType(i)));
  }

  for (int32_t y = 0; y < 3; ++y) {
    for (int32_t x = 0; x < 9; ++x) {
      addSlot(std::make_unique<BasicSlot>(stor, stor->getInventoryOffset() + x + y * 9, ISlot::Inventory));
    }
  }

  for (int32_t i = 0; i < 9; ++i) {
    addSlot(std::make_unique<BasicSlot>(stor, stor->getHotbarOffset() + i, ISlot::Hotbar));
  }
}

PlayerContainer::~PlayerContainer() {}

SlotId PlayerContainer::getItemSlotById(ItemId iid) {
  return std::find_if(m_slots.begin(), m_slots.end(), [=](auto& s) -> bool { return s->getHeldItem().itemId == iid; })->get()->getAbsoluteSlotId();
}

ItemStack& PlayerContainer::getHotbarItem(uint8_t offset) {
  return m_storage->getByOffset(m_storage->getHotbarOffset() + offset);
}

SlotId PlayerContainer::getStorageItemSlotId(const ItemStack& is) {
  for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
    if (&it->get()->getHeldItem() == &is) return it->get()->getAbsoluteSlotId();
  }

  return -1;
}
