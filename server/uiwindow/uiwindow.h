#pragma once

#include "helper.h"
#include "items/itemstack.h"

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

  UiWindow(const std::string_view name, Type type): m_name(name), m_type(type) {}

  bool operator==(const WinId id) const { return m_id == id; }

  const char* getName() const { return m_name.c_str(); }

  WinId getId() const { return m_id; }

  Type getType() const { return m_type; }

  virtual int16_t getSlotsCount() const = 0;

  void setWinid(WinId id) { m_id = id; }

  virtual bool onClick(SlotId sid, bool isRmb, bool isShift, ItemStack** updatedItem) = 0;

  virtual bool onClose() = 0;

  private:
  Type        m_type;
  WinId       m_id;
  std::string m_name;
};
