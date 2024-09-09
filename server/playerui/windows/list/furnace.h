#pragma once

#include "../uiwindow.h"
#include "playerui/containers/furnaceContainer.h"

class FurnacehWindow: public UiWindow {
  public:
  FurnacehWindow(PlayerStorage* pstor, SmeltingStorage* sstor);

  ~FurnacehWindow();

  IContainer* container() final;

  bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) final;

  bool onClose() final;

  private:
  FurnaceContainer m_cont;
};
