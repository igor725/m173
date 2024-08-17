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

  CraftingRecipe(const ItemStack& result, const std::string& recipe, const std::vector<ItemId> used);

  virtual ~CraftingRecipe() = default;

  uint8_t getSize() const { return m_width * m_height; }

  bool matches(ItemStack** set, uint8_t width, uint8_t height);

  static bool scan(IContainer* cont, ItemStack** result);

  static uint32_t getCount();

  private:
  std::array<int8_t, WIDTH * HEIGHT> m_recipe;
  std::array<ItemId, WIDTH * HEIGHT> m_usedItems;
  ItemStack                          m_result;
  uint8_t                            m_width  = 0;
  uint8_t                            m_height = 0;
};
