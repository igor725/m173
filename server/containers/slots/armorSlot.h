#include "../slot.h"

class ArmorSlot: public Slot {
  public:
  ArmorSlot(IStorage* stor, SlotId sid, int8_t armorType): Slot(stor, sid, Slot::Armor), m_armorType(armorType) {}

  bool isItemValid(const ItemStack& is) const final { return false; /* todo actual implementation */ }

  private:
  int8_t m_armorType;
};
