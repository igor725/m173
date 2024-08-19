#pragma once

#include "containers/container.h"
#include "helper.h"
#include "items/itemstack.h"

#include <array>
#include <cstdint>
#include <string>

class CraftingRecipe {
  public:
  static const uint8_t WIDTH  = 3;
  static const uint8_t HEIGHT = 3;

  CraftingRecipe(const ItemStack& result, const std::string& recipe, const std::vector<ItemStack>& used);

  ~CraftingRecipe();

  uint8_t getSize() const { return m_width * m_height; }

  void registerme();

  bool matches(ItemStack** set, uint8_t setW, uint8_t setH);

  ItemId getResultItemId() { return m_result.itemId; }

  static bool scan(IContainer* cont, ItemStack** result);

  static uint32_t getCount();
  // static uint32_t getUncraftable();

  private:
  bool innerMatcher(ItemStack** set, uint8_t setW, uint8_t setH, uint8_t oX, uint8_t oY);

  std::array<int8_t, WIDTH * HEIGHT>    m_recipe;
  std::array<ItemStack, WIDTH * HEIGHT> m_usedItems;
  ItemStack                             m_result;
  uint8_t                               m_width  = 0;
  uint8_t                               m_height = 0;
};

class CraftingRecipeAR: public CraftingRecipe {
  public:
  CraftingRecipeAR(const ItemStack& result, const std::string& recipe, const std::vector<ItemStack>& used): CraftingRecipe(result, recipe, used) {
    registerme();
  }

  ~CraftingRecipeAR();
};
