#include "container.h"

IContainer::IContainer(uint32_t slotsNum) {
  m_slots.reserve(slotsNum);
}

IContainer::~IContainer() {}

void IContainer::addSlot(Slot&& slot) {
  m_slots.emplace_back(std::move(slot));
}

ItemStack& IContainer::getItem(SlotId sid) {
  return m_slots.at(sid).getHeldItem();
}
