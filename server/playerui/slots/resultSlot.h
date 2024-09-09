#pragma once

#include "../slot.h"

class ResultSlot: public ISlot {
  public:
  ResultSlot(IStorage* stor, SlotId sid): ISlot(stor, sid, ISlot::Result) {}

  bool isItemValid(const ItemStack& is) const final { return false; }

  int16_t getSlotStackLimit() const final { return 64; }
};
