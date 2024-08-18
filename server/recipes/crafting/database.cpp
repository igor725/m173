#include "armorrecipes.h"
#include "blocks/blockDB.h"
#include "items/itemDB.h"
#include "recipe.h"
#include "toolrecipes.h"

namespace RecipeDB::Crafting {
ToolRecipes  tools;
ArmorRecipes armor;

CraftingRecipeAR workbench(ItemStack(BlockDB::workbench.getId(), 1), "11 11", {BlockDB::planks.getId()});
CraftingRecipeAR stick(ItemStack(ItemDB::stick.getId(), 3), "1", {BlockDB::planks.getId()});
CraftingRecipeAR paper(ItemStack(ItemDB::paper.getId(), 3), "111", {ItemDB::sugarCane.getId()});
CraftingRecipeAR book(ItemStack(ItemDB::book.getId(), 1), "11 12", {ItemDB::paper.getId(), ItemDB::leather.getId()});
CraftingRecipeAR bookshelf(ItemStack(BlockDB::bookshelf.getId(), 1), "111 222 111", {BlockDB::planks.getId(), ItemDB::book.getId()});
CraftingRecipeAR fence(ItemStack(BlockDB::fence.getId(), 2), "111 111", {ItemDB::stick.getId()});
CraftingRecipeAR diamondBlock(ItemStack(BlockDB::diamondBlock.getId(), 1), "111 111 111", {ItemDB::diamond.getId()});
}; // namespace RecipeDB::Crafting
