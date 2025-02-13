#pragma once

#include "items/itemstack.h"

class IStorage {
  CLASS_NO_COPY(IStorage);
  CLASS_NO_MOVE(IStorage);

  public:
  IStorage()          = default;
  virtual ~IStorage() = default;

  virtual ItemStack& getByOffset(SlotId slot) = 0;

  virtual SlotId getSlotId(const ItemStack& is) const = 0;

  virtual void clear() = 0;

  virtual SlotId findItemSlotId(ItemId iid) const { return -1; }

  virtual int16_t getSize() const { return 0; }

  virtual bool push(const ItemStack& is, SlotId* sid = nullptr, SlotId prioritySlot = -1) { return false; };
};
