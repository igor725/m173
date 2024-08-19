#pragma once

#include "blocks/blockDB.h"
#include "items/itemDB.h"
#include "recipe.h"

#include <array>
#include <vector>

class BlockRecipes {
  public:
  BlockRecipes() {
    static const char* PATTERN      = "111 111 111";
    static const char* PATTERN_BACK = "1";

    static std::array<ItemStack, 24> MATERIALS = {
        ItemStack(ItemDB::goldIngot.getId(), 9), // Gold
        BlockDB::goldBlock.getId(),

        ItemStack(ItemDB::ironIngot.getId(), 9), // Iron
        BlockDB::ironBlock.getId(),

        ItemStack(ItemDB::diamond.getId(), 9), // Diamond
        BlockDB::diamondBlock.getId(),

        ItemStack(ItemDB::genericDye.getId(), 9, 4), // Lapis
        BlockDB::lapisBlock.getId(),
    };

    m_recipes.reserve(MATERIALS.size());

    ItemStack item;
    for (auto it = MATERIALS.begin(); it != MATERIALS.end(); ++it) {
      auto curr = it;
      auto next = ++it;

      auto& recBlock = m_recipes.emplace_back(CraftingRecipe(*next, PATTERN, {*curr}));
      auto& recIngot = m_recipes.emplace_back(CraftingRecipe(*curr, PATTERN_BACK, {*next}));

      recBlock.registerme();
      recIngot.registerme();
    }
  }

  private:
  std::vector<CraftingRecipe> m_recipes;
};
