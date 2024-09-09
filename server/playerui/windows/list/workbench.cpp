#include "workbench.h"

WorkbenchWindow::WorkbenchWindow(PlayerStorage* pstor): UiWindow("Crafting", UiWindow::Workbench), m_cont(pstor) {}

WorkbenchWindow::~WorkbenchWindow() {}

IContainer* WorkbenchWindow::container() {
  return &m_cont;
}

bool WorkbenchWindow::onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) {
  // Forwarding call to container
  return m_cont.onSlotClicked(sid, isRmb, isShift, updatedItem);
}

bool WorkbenchWindow::onClose() {
  return m_cont.onClosed();
}
