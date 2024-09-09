#include "armorSlot.h"

#pragma region("armorSlot.h")

bool ArmorSlot::isItemValid(const ItemStack& is) const {
  return (m_armorType == ArmorType::Head && is.itemId == BlockDB::pumpkin.getId()) || armorTest(is.itemId); /* todo actual implementation */
}

bool ArmorSlot::armorTest(ItemId iid) const {
  auto armor = dynamic_cast<ItemArmor*>(Item::getById(iid));
  return armor != nullptr && armor->isValidType(m_armorType);
}

#pragma endregion()
