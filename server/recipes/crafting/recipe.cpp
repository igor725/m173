#include "recipe.h"

#include <spdlog/spdlog.h>
#include <sstream>
#include <vector>

namespace {
std::vector<CraftingRecipe*> g_recipes;
}

CraftingRecipe::CraftingRecipe(const ItemStack& result, const std::string& recipe, const std::vector<ItemId> used): m_result(result) {
  m_recipe.fill(-1);
  m_usedItems.fill(-1);

  std::stringstream ss(recipe);

  if (used.size() == 0) {
    spdlog::warn("0 used items in recipe!");
    return;
  }
  if (used.size() < m_usedItems.size()) {
    for (auto it = used.begin(); it != used.end(); ++it) {
      m_usedItems[std::distance(used.begin(), it)] = *it;
    }
  } else {
    spdlog::warn("Too much used items in recipe!");
    return;
  }

  {
    std::string row;
    while (std::getline(ss, row, ' ')) {
      m_width = std::max(m_width, uint8_t(row.length()));
      for (auto it = row.begin(); it != row.end(); ++it) {
        auto value = m_recipe[m_height * 3 + std::distance(row.begin(), it)] = *it - '1';
        if (value >= used.size() || value < 0) {
          spdlog::warn("Invalid recipe!");
          return;
        }
      }
      ++m_height;
    }
  }

  g_recipes.push_back(this);
}

bool CraftingRecipe::matches(ItemStack** set, uint8_t setW, uint8_t setH) { // todo optimize this fuckery
  for (auto oX = 0; oX <= (setW - m_width); ++oX) {
    for (auto oY = 0; oY <= (setH - m_height); ++oY) {
      bool recipeOk = true;

      if (oX > 0 || oY > 0) {
        for (auto x = 0; recipeOk && (x < setW); ++x) {
          for (auto y = 0; recipeOk && (y < setH); ++y) {
            if ((oX > 0 && x < oX) || (oY > 0 && x < oY)) {
              auto setItem = set[y * setW + x];
              if (setItem != nullptr && !setItem->isEmpty()) {
                recipeOk = false;
                break;
              }
            }
          }
        }
      }

      for (auto x = 0; recipeOk && (x < setW); ++x) {
        for (auto y = 0; recipeOk && (y < setH); ++y) {
          auto setItem = set[(oY + y) * setW + (oX + x)];
          if ((x >= m_width || y >= m_height) && (setItem != nullptr && !setItem->isEmpty())) {
            recipeOk = false;
            break;
          }

          auto crid = m_recipe[y * WIDTH + x];
          if (crid < int8_t(0)) {
            recipeOk = setItem == nullptr || setItem->isEmpty();
            continue;
          }

          recipeOk = setItem->itemId == m_usedItems[crid];
        }
      }

      if (recipeOk) return true;
    }
  }

  return false;
}

bool CraftingRecipe::scan(IContainer* cont, ItemStack** result) {
  if (cont == nullptr) return false;
  ItemStack* setrecipe[3 * 3] = {nullptr};
  uint8_t    width = 0, height = 0;

  if (cont->getRecipe(setrecipe, result, width, height)) {
    for (auto it = g_recipes.begin(); it != g_recipes.end(); ++it) {
      if (auto recipe = (*it)) {
        if (recipe->getSize() > (width * height)) continue;
        if (recipe->matches(setrecipe, width, height)) {
          **result = recipe->m_result;
          return true;
        }
      }
    }

    **result = ItemStack();
    return true;
  }

  return false;
}

uint32_t CraftingRecipe::getCount() {
  return g_recipes.size();
}
