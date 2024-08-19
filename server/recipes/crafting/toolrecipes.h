#pragma once

#include "blocks/blockDB.h"
#include "items/itemDB.h"
#include "recipe.h"

#include <array>
#include <vector>

class ToolRecipes {
  public:
  ToolRecipes() {
    static std::array<const char*, 5> PATTERNS = {
        "22 01 01",    // HOE_PATTERN
        "2 2 1",       // SWORD_PATTERN
        "222 010 010", // PICKAXE_PATTERN
        "22 21 01",    // AXE_PATTERN
        "2 1 1",       // SHOVEL_PATTERN
    };

    static std::array<ItemId, 30> MATERIALS = {
        BlockDB::planks.getId(),       // Wooden tools start
        ItemDB::woodenHoe.getId(),     //
        ItemDB::woodenSword.getId(),   //
        ItemDB::woodenPickaxe.getId(), //
        ItemDB::woodenAxe.getId(),     //
        ItemDB::woodenShovel.getId(),  //

        BlockDB::cobblestone.getId(), // Stone tools start
        ItemDB::stoneHoe.getId(),     //
        ItemDB::stoneSword.getId(),   //
        ItemDB::stonePickaxe.getId(), //
        ItemDB::stoneAxe.getId(),     //
        ItemDB::stoneShovel.getId(),  //

        ItemDB::goldIngot.getId(),     // Golden tools start
        ItemDB::goldenHoe.getId(),     //
        ItemDB::goldenSword.getId(),   //
        ItemDB::goldenPickaxe.getId(), //
        ItemDB::goldenAxe.getId(),     //
        ItemDB::goldenShovel.getId(),  //

        ItemDB::ironIngot.getId(),   // Iron tools start
        ItemDB::ironHoe.getId(),     //
        ItemDB::ironSword.getId(),   //
        ItemDB::ironPickaxe.getId(), //
        ItemDB::ironAxe.getId(),     //
        ItemDB::ironShovel.getId(),  //

        ItemDB::diamond.getId(),        // Diamond tools start
        ItemDB::diamondHoe.getId(),     //
        ItemDB::diamondSword.getId(),   //
        ItemDB::diamondPickaxe.getId(), //
        ItemDB::diamondAxe.getId(),     //
        ItemDB::diamondShovel.getId(),  //
    };

    m_recipes.reserve(MATERIALS.size());

    std::vector<ItemStack> m_items = {ItemDB::stick.getId(), 0};
    for (auto it = MATERIALS.begin(); it != MATERIALS.end(); ++it) {
      auto num = std::distance(MATERIALS.begin(), it) % 6;
      if (num == 0) {
        // Setting the material type
        m_items.at(1) = *it;
      } else {
        auto& recipe = m_recipes.emplace_back(CraftingRecipe(ItemStack(*it, 1), PATTERNS[num - 1], m_items));
        recipe.registerme();
      }
    }
  }

  private:
  std::vector<CraftingRecipe> m_recipes;
};
