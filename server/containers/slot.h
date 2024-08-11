#pragma once

#include "helper.h"
#include "items/itemstack.h"
#include "storage.h"

#include <cstdint>

class Slot {
  public:
  Slot(IStorage* storage, SlotId slotid): m_storageSlotIndex(slotid), m_storage(storage) {}

  virtual int16_t getSlotStackLimit() { return 64; }

  virtual bool isItemValid(ItemStack& is) { return true; }

  virtual SlotId getAbsoluteSlotId() const { return m_slotIndex; }

  void setAbsoluteSlotId(SlotId asid) { m_slotIndex = asid; }

  ItemStack& getHeldItem() { return m_storage->getByOffset(m_storageSlotIndex); }

  private:
  SlotId    m_slotIndex;
  SlotId    m_storageSlotIndex;
  IStorage* m_storage;
};
