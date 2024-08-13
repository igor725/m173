#pragma once

#include "helper.h"
#include "items/itemstack.h"
#include "storage.h"

#include <cstdint>

class Slot {
  public:
  enum Type {
    Result,
    CraftRecipe,
    Armor,
    Inventory,
    Hotbar,
    FurnaceFuel,
    FurnaceCook,
  };

  Slot(IStorage* storage, SlotId slotid, Type type): m_storageSlotIndex(slotid), m_storage(storage), m_slotType(type) {}

  virtual int16_t getSlotStackLimit() { return 64; }

  virtual int16_t getAvailableRoom() { return getSlotStackLimit() - getHeldItem().stackSize; }

  virtual bool isItemValid(const ItemStack& is) const { return true; }

  virtual SlotId getAbsoluteSlotId() const { return m_slotIndex; }

  Type getSlotType() const { return m_slotType; }

  void setAbsoluteSlotId(SlotId asid) { m_slotIndex = asid; }

  ItemStack& getHeldItem() { return m_storage->getByOffset(m_storageSlotIndex); }

  private:
  Type      m_slotType;
  SlotId    m_slotIndex;
  SlotId    m_storageSlotIndex;
  IStorage* m_storage;
};
