#include "recipe.h"

#include "runmanager/runmanager.h"

#include <spdlog/spdlog.h>
#include <sstream>
#include <vector>

namespace {
std::vector<CraftingRecipe*> g_recipes;
}

CraftingRecipe::CraftingRecipe(const ItemStack& result, const std::string& recipe, const std::vector<ItemStack>& used): m_result(result) {
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
        if (value >= int8_t(used.size()) || value < -1) {
          spdlog::warn("Invalid recipe!");
          return;
        }
      }
      ++m_height;
    }
  }
}

CraftingRecipe::~CraftingRecipe() {
  for (auto it = g_recipes.begin(); it != g_recipes.end(); ++it) {
    if (*it == this) {
      g_recipes.erase(it);
      return;
    }
  }
}

void CraftingRecipe::registerme() {
  g_recipes.push_back(this);
}

bool CraftingRecipe::innerMatcher(ItemStack** set, uint8_t setW, uint8_t setH, uint8_t oX, uint8_t oY, bool inverse) {
  for (int8_t x = 0; x < setW; ++x) {
    for (int8_t y = 0; y < setH; ++y) {
      int8_t ax = x - oX, ay = y - oY, idx = 0;
      int8_t craftItem = -1;

      if (ax >= 0 && ay >= 0 && ax < m_width && ay < m_height) {
        craftItem = m_recipe[ax + ay * 3];
      }

      auto setItem          = set[inverse ? setW - x - 1 + y * setW : x + y * setW];
      bool setItemPresent   = (setItem != nullptr && !setItem->isEmpty());
      bool craftItemPresent = craftItem != -1;

      if (setItemPresent || craftItemPresent) {
        if ((setItemPresent != craftItemPresent)) return false;
        if (!m_usedItems[craftItem].isSimilarTo(*setItem)) return false;
      }
    }
  }

  return true;
}

bool CraftingRecipe::matches(ItemStack** set, uint8_t setW, uint8_t setH) {
  if (m_width > setW || m_height > setH) return false;

  for (auto oX = 0; oX <= (setW - m_width); ++oX) {
    for (auto oY = 0; oY <= (setH - m_height); ++oY) {
      if (innerMatcher(set, setW, setH, oX, oY, false)) return true;
      if (innerMatcher(set, setW, setH, oX, oY, true)) return true;
    }
  }

  return false;
}

bool CraftingRecipe::scan(IContainer* cont, ItemStack** result) {
  if (cont == nullptr) return false;

  std::array<ItemStack*, 3 * 3> setrecipe = {nullptr};

  uint8_t width = 0, height = 0;

  if (cont->getRecipe(setrecipe.data(), result, width, height)) {
    for (auto it = g_recipes.begin(); it != g_recipes.end(); ++it) {
      if (auto recipe = (*it)) {
        if (recipe->getSize() > (width * height)) continue;
        if (recipe->matches(setrecipe.data(), width, height)) {
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

CraftingRecipeAR::~CraftingRecipeAR() {
  if (RunManager::isRunning()) {
    spdlog::warn("Autoregistered recipe {:#010x} got destroyed in runtime, that's definitely not good!!", reinterpret_cast<std::uintptr_t>(this));
  }
}
