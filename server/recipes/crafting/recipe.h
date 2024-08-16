#pragma once

#include "helper.h"

#include <array>
#include <cstdint>
#include <vector>

class CraftingRecipe {
  CraftingRecipe()          = default;
  virtual ~CraftingRecipe() = default;

  private:
  std::array<int8_t, 9> m_recipe = {-1};
  std::array<ItemId, 9> m_usedItems;
};
