#pragma once

#include "entry/helper.h"
#include "items/itemstack.h"
#include "storage.h"

#include <algorithm>
#include <cstdint>

class ISlot {
  CLASS_NO_COPY(ISlot);
  CLASS_NO_MOVE(ISlot);

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

  int16_t getAvailableRoom() {
    auto& is = getHeldItem();
    return std::min(getSlotStackLimit(), is.availStackRoom());
  }

  SlotId getAbsoluteSlotId() const { return m_slotIndex; }

  SlotId getRelativeSlotId() const { return m_storageSlotIndex; }

  Type getSlotType() const { return m_slotType; }

  void setAbsoluteSlotId(SlotId asid) { m_slotIndex = asid; }

  ItemStack& getHeldItem() { return m_storage->getByOffset(m_storageSlotIndex); }

  private:
  Type      m_slotType;
  SlotId    m_slotIndex;
  SlotId    m_storageSlotIndex;
  IStorage* m_storage;
};
