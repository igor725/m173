#pragma once

#include "entry/helper.h"
#include "items/itemstack.h"
#include "playerui/container.h"

#include <string>

class UiWindow {
  public:
  enum Type : int8_t {
    Inventory = -1, // Only for internal use
    Basic     = 0,
    Workbench = 1,
    Furnace   = 2,
    Dispenser = 3,
  };

  UiWindow(std::string_view name, Type type): m_name(name), m_type(type) {}

  virtual ~UiWindow() = default;

  bool operator==(const WinId id) const { return m_id == id; }

  const char* getName() const { return m_name.c_str(); }

  WinId getId() const { return m_id; }

  Type getType() const { return m_type; }

  virtual IContainer* container() = 0;

  void setWinid(WinId id) { m_id = id; }

  virtual bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) = 0;

  virtual bool onClose() = 0;

  private:
  Type        m_type;
  WinId       m_id;
  std::string m_name;
};
