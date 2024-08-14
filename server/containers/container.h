#pragma once

#include "slot.h"

#include <cstdint>
#include <memory>
#include <vector>

class IContainer {
  CLASS_NO_COPY(IContainer);
  CLASS_NO_MOVE(IContainer);

  static const SlotId SLOT_OFFSCREEN_CLICK = -999;

  public:
  IContainer(uint32_t slotNum);
  ~IContainer();

  int16_t getSize() const { return m_slots.size(); }

  ISlot* getSlot(SlotId sid) const;

  ItemStack& getItem(SlotId sid);

  virtual SlotId getItemSlotByItemStack(const ItemStack& is) = 0;

  virtual bool push(const ItemStack& is, SlotId* sid, SlotId prioritySlot) = 0;

  virtual bool onSlotClicked(SlotId sid, bool isRmb, bool shift);

  virtual bool onWindowClosed();

  protected: // Protected functions
  void addSlot(std::unique_ptr<ISlot>&& slot);

  virtual SlotId getItemSlotById(ItemId iid) = 0;

  virtual ItemStack& getHotbarItem(uint8_t iid) = 0;

  protected: // Protected variables
  std::vector<std::unique_ptr<ISlot>> m_slots;
  ItemStack                           m_carriedItem;
  SlotId                              m_carriedItemFrom;
};
