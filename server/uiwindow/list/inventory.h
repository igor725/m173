#pragma once

#include "../uiwindow.h"
#include "containers/list/playerContainer.h"

class InventoryWindow: public UiWindow {
  public:
  InventoryWindow(PlayerContainer& cont): UiWindow("Inventory", UiWindow::Inventory), m_cont(cont) {}

  bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) final {
    // Forwarding call to container
    return m_cont.onSlotClicked(sid, isRmb, isShift, updatedItem);
  }

  bool onClose() final { return m_cont.onClosed(); }

  int8_t getSlotsCount() const { return m_cont.getSize(); }

  private:
  PlayerContainer& m_cont;
};
