#include "../slot.h"

class ArmorSlot: public Slot {
  public:
  ArmorSlot(IStorage* stor, SlotId sid, int8_t armorType): Slot(stor, sid), m_armorType(armorType) {}

  bool isItemValid(ItemStack& is) { return false; /* todo actual implementation */ }

  private:
  int8_t m_armorType;
};
