#pragma once

#include "../slot.h"

class BasicSlot: public ISlot {
  public:
  BasicSlot(IStorage* stor, SlotId sid, Type type): ISlot(stor, sid, type) {}

  bool isItemValid(const ItemStack& is) const final { return true; }

  int16_t getSlotStackLimit() const final { return 64; }
};
