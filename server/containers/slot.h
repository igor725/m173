#pragma once

#include "helper.h"
#include "items/itemstack.h"
#include "storage.h"

#include <cstdint>

class Slot {
  public:
  Slot(IStorage* storage, SlotId slotid): m_slotIndex(slotid), m_storage(storage) {}

  virtual int16_t getSlotStackLimit() { return 64; }

  virtual bool isItemValid(ItemStack& is) { return true; }

  virtual SlotId getSlotId() const { return m_slotIndex; }

  ItemStack& getHeldItem() { return m_storage->getByOffset(m_slotIndex); }

  private:
  SlotId    m_slotIndex;
  IStorage* m_storage;
};
