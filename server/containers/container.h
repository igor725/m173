#pragma once

#include "slot.h"

#include <cstdint>
#include <vector>

class IContainer {
  CLASS_NO_COPY(IContainer);
  CLASS_NO_MOVE(IContainer);

  static const SlotId SLOT_OFFSCREEN_CLICK = -999;

  public:
  IContainer(uint32_t slotNum);
  ~IContainer();

  int16_t getSize() const { return m_slots.size(); }

  ItemStack& getItem(SlotId sid);

  virtual bool onSlotClicked(SlotId sid, bool isRmb, bool shift);

  protected:
  void addSlot(Slot&& slot);

  virtual SlotId getItemSlotById(ItemId iid) = 0;

  virtual ItemStack& getHotbarItem(uint8_t iid) = 0;

  protected:
  std::vector<Slot> m_slots;
};
