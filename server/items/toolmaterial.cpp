#include "toolmaterial.h"

const ToolMaterial& ToolMaterial::select(ToolMaterial::Index idx) {
  static const ToolMaterial NONE_MATERIAL(0, 0, 0.0f, 0);
  static const ToolMaterial WOOD_MATERIAL(0, 59, 2.0f, 0);
  static const ToolMaterial STONE_MATERIAL(1, 131, 4.0f, 1);
  static const ToolMaterial IRON_MATERIAL(2, 250, 6.0f, 2);
  static const ToolMaterial GOLD_MATERIAL(0, 32, 12.0f, 0);
  static const ToolMaterial DIAMOND_MATERIAL(3, 1561, 8.0f, 3);

  switch (idx) {
    case Index::Wood: return WOOD_MATERIAL;
    case Index::Stone: return STONE_MATERIAL;
    case Index::Iron: return IRON_MATERIAL;
    case Index::Gold: return GOLD_MATERIAL;
    case Index::Diamond: return DIAMOND_MATERIAL;

    default: return NONE_MATERIAL;
  }
}
