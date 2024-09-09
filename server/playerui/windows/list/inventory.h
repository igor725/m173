#pragma once

#include "../uiwindow.h"
#include "playerui/containers/playerContainer.h"

class InventoryWindow: public UiWindow {
  public:
  InventoryWindow(PlayerContainer& cont);

  ~InventoryWindow();

  IContainer* container() final;

  bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) final;

  bool onClose() final;

  private:
  PlayerContainer& m_cont;
};
