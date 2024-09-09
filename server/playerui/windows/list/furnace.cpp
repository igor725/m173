#include "furnace.h"

FurnacehWindow::FurnacehWindow(PlayerStorage* pstor, SmeltingStorage* sstor): UiWindow("Furnace", UiWindow::Furnace), m_cont(pstor, sstor) {}

FurnacehWindow::~FurnacehWindow() {}

IContainer* FurnacehWindow::container() {
  return &m_cont;
}

bool FurnacehWindow::onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) {
  // Forwarding call to container
  return m_cont.onSlotClicked(sid, isRmb, isShift, updatedItem);
}

bool FurnacehWindow::onClose() {
  return m_cont.onClosed();
}
