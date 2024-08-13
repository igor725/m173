#pragma once

#include "helper.h"
#include "items/itemstack.h"
#include "storage.h"

#include <cstdint>

class ISlot {
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

  ISlot(IStorage* storage, SlotId slotid, Type type): m_slotType(type), m_storageSlotIndex(slotid), m_storage(storage) {}

  virtual ~ISlot() = default;

  virtual int16_t getSlotStackLimit() const = 0;

  virtual bool isItemValid(const ItemStack& is) const = 0;

  int16_t getAvailableRoom() { return getSlotStackLimit() - getHeldItem().stackSize; }

  SlotId getAbsoluteSlotId() const { return m_slotIndex; }

  Type getSlotType() const { return m_slotType; }

  void setAbsoluteSlotId(SlotId asid) { m_slotIndex = asid; }

  ItemStack& getHeldItem() { return m_storage->getByOffset(m_storageSlotIndex); }

  private:
  Type      m_slotType;
  SlotId    m_slotIndex;
  SlotId    m_storageSlotIndex;
  IStorage* m_storage;
};
