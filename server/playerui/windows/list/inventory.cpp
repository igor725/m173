#include "inventory.h"

InventoryWindow::InventoryWindow(PlayerContainer& cont): UiWindow("Inventory", UiWindow::Inventory), m_cont(cont) {}

InventoryWindow::~InventoryWindow() {}

IContainer* InventoryWindow::container() {
  return &m_cont;
}

bool InventoryWindow::onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) {
  // Forwarding call to container
  return m_cont.onSlotClicked(sid, isRmb, isShift, updatedItem);
}

bool InventoryWindow::onClose() {
  return m_cont.onClosed();
}
