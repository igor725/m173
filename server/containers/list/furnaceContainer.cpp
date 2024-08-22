#include "furnaceContainer.h"

#include "../slots/basicSlot.h"
#include "../slots/resultSlot.h"

FurnaceContainer::FurnaceContainer(PlayerStorage* pstor, SmeltingStorage* sstor): IContainer(38), m_smelting(sstor) {
  addSlot(std::make_unique<BasicSlot>(sstor, 0, ISlot::FurnaceCook));
  addSlot(std::make_unique<BasicSlot>(sstor, 1, ISlot::FurnaceFuel));
  addSlot(std::make_unique<ResultSlot>(sstor, 2, ISlot::Result));

  for (int32_t y = 0; y < 3; ++y) {
    for (int32_t x = 0; x < 9; ++x) {
      addSlot(std::make_unique<BasicSlot>(pstor, pstor->getInventoryOffset() + x + y * 9, ISlot::Inventory));
    }
  }

  for (int32_t i = 0; i < 9; ++i) {
    addSlot(std::make_unique<BasicSlot>(pstor, pstor->getHotbarOffset() + i, ISlot::Hotbar));
  }
}

FurnaceContainer::~FurnaceContainer() {}

SlotId FurnaceContainer::getItemSlotById(ItemId iid) {
  auto it = std::find_if(m_slots.begin(), m_slots.end(), [=](auto& s) -> bool { return s->getHeldItem().itemId == iid; });
  if (it == m_slots.end()) return -1;
  return it->get()->getAbsoluteSlotId();
}

ISlot* FurnaceContainer::getItemSlotByItemStack(const ItemStack& is) {
  auto it = std::find_if(m_slots.begin(), m_slots.end(), [&](auto& s) -> bool { return &s->getHeldItem() == &is; });
  if (it == m_slots.end()) return nullptr;
  return it->get();
}

bool FurnaceContainer::push(const ItemStack& is, SlotId* sid, SlotId prioritySlot) {
  return false;
}
