#pragma once

#include "../uiwindow.h"
#include "playerui/containers/workbenchContainer.h"

class WorkbenchWindow: public UiWindow {
  public:
  WorkbenchWindow(PlayerStorage* pstor);
  ~WorkbenchWindow();

  IContainer* container() final;

  bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) final;

  bool onClose() final;

  private:
  WorkbenchContainer m_cont;
};
