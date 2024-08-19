#pragma once

#include "items/database.h"
#include "recipe.h"

#include <array>
#include <vector>

class ArmorRecipes {
  public:
  ArmorRecipes() {
    static std::array<const char*, 4> PATTERNS = {
        "111 101",     // HELMET_PATTERN
        "101 111 111", // CHEST_PATTERN
        "111 101 101", // LEGGINGS_PATTERN
        "101 101",     // BOOTS_PATTERN
    };

    static std::array<ItemId, 24> MATERIALS = {
        ItemDB::leather.getId(),         // Leather armor start
        ItemDB::leatherHelmet.getId(),   //
        ItemDB::leatherChest.getId(),    //
        ItemDB::leatherLeggings.getId(), //
        ItemDB::leatherBoots.getId(),    //

        ItemDB::ironIngot.getId(),    // Iron armor start
        ItemDB::ironHelmet.getId(),   //
        ItemDB::ironChest.getId(),    //
        ItemDB::ironLeggings.getId(), //
        ItemDB::ironBoots.getId(),    //

        ItemDB::diamond.getId(),         // Diamond armor start
        ItemDB::diamondHelmet.getId(),   //
        ItemDB::diamondChest.getId(),    //
        ItemDB::diamondLeggings.getId(), //
        ItemDB::diamondBoots.getId(),    //

        ItemDB::goldIngot.getId(),      // Golden armor start
        ItemDB::goldenHelmet.getId(),   //
        ItemDB::goldenChest.getId(),    //
        ItemDB::goldenLeggings.getId(), //
        ItemDB::goldenBoots.getId(),    //

    };

    m_recipes.reserve(MATERIALS.size());

    ItemStack item;
    for (auto it = MATERIALS.begin(); it != MATERIALS.end(); ++it) {
      auto num = std::distance(MATERIALS.begin(), it) % 5;
      if (num == 0) {
        // Setting the material type
        item = *it;
      } else {
        auto& recipe = m_recipes.emplace_back(CraftingRecipe(ItemStack(*it, 1), PATTERNS[num - 1], {item}));
        recipe.registerme();
      }
    }
  }

  private:
  std::vector<CraftingRecipe> m_recipes;
};
