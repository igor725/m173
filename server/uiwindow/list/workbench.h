#pragma once

#include "../uiwindow.h"
#include "containers/list/workbenchContainer.h"

class WorkbenchWindow: public UiWindow {
  public:
  WorkbenchWindow(PlayerStorage* pstor): UiWindow("Crafting", UiWindow::Workbench), m_cont(pstor) {}

  bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) final {
    // Forwarding call to container
    return m_cont.onSlotClicked(sid, isRmb, isShift, updatedItem);
  }

  bool onClose() final { return m_cont.onClosed(); }

  int8_t getSlotsCount() const { return m_cont.getSize(); }

  private:
  WorkbenchContainer m_cont;
};
