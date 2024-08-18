#include "workbenchContainer.h"

#include "../slots/basicSlot.h"
#include "../slots/resultSlot.h"

WorkbenchContainer::WorkbenchContainer(PlayerStorage* pstor): IContainer(45), m_crafting(this), m_craftRes() {
  addSlot(std::make_unique<ResultSlot>(&m_craftRes, 0));

  for (int32_t y = 0; y < getRecipeHeight(); ++y) {
    for (int32_t x = 0; x < getRecipeWidth(); ++x) {
      addSlot(std::make_unique<BasicSlot>(&m_crafting, x + y * getRecipeWidth(), ISlot::CraftRecipe));
    }
  }

  for (int32_t y = 0; y < 3; ++y) {
    for (int32_t x = 0; x < 9; ++x) {
      addSlot(std::make_unique<BasicSlot>(pstor, pstor->getInventoryOffset() + x + y * 9, ISlot::Inventory));
    }
  }

  for (int32_t i = 0; i < 9; ++i) {
    addSlot(std::make_unique<BasicSlot>(pstor, pstor->getHotbarOffset() + i, ISlot::Hotbar));
  }
}

WorkbenchContainer::~WorkbenchContainer() {}

SlotId WorkbenchContainer::getItemSlotById(ItemId iid) {
  auto it = std::find_if(m_slots.begin(), m_slots.end(), [=](auto& s) -> bool { return s->getHeldItem().itemId == iid; });
  if (it == m_slots.end()) return -1;
  return it->get()->getAbsoluteSlotId();
}

SlotId WorkbenchContainer::getItemSlotByItemStack(const ItemStack& is) {
  auto it = std::find_if(m_slots.begin(), m_slots.end(), [&](auto& s) -> bool { return &s->getHeldItem() == &is; });
  if (it == m_slots.end()) return -1;
  return it->get()->getAbsoluteSlotId();
}

SlotId WorkbenchContainer::getStorageItemSlotId(const ItemStack& is) {
  for (auto it = m_slots.begin(); it != m_slots.end(); ++it) {
    if (&it->get()->getHeldItem() == &is) return it->get()->getAbsoluteSlotId();
  }

  return -1;
}

bool WorkbenchContainer::push(const ItemStack& is, SlotId* sid, SlotId prioritySlot) {
  return false;
}
