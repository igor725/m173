#pragma once

#include "../uiwindow.h"
#include "containers/list/furnaceContainer.h"

class FurnacehWindow: public UiWindow {
  public:
  FurnacehWindow(PlayerStorage* pstor, SmeltingStorage* sstor): UiWindow("Furnace", UiWindow::Furnace), m_cont(pstor, sstor) {}

  ~FurnacehWindow() {}

  IContainer* container() final { return &m_cont; }

  bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) final {
    // Forwarding call to container
    return m_cont.onSlotClicked(sid, isRmb, isShift, updatedItem);
  }

  bool onClose() final { return m_cont.onClosed(); }

  private:
  FurnaceContainer m_cont;
};
