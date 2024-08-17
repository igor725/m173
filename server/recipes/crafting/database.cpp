#include "blocks/blockDB.h"
#include "items/itemDB.h"
#include "recipe.h"

namespace RecipeDB::Crafting {
CraftingRecipe stick(ItemStack(ItemDB::stick.getId(), 3), "1", {BlockDB::planks.getId()});
CraftingRecipe workbench(ItemStack(BlockDB::workbench.getId(), 1), "11 11", {BlockDB::planks.getId()});
CraftingRecipe paper(ItemStack(ItemDB::paper.getId(), 3), "111", {ItemDB::sugarCane.getId()});
CraftingRecipe book(ItemStack(ItemDB::book.getId(), 1), "11 12", {ItemDB::paper.getId(), ItemDB::leather.getId()});
CraftingRecipe bookshelf(ItemStack(BlockDB::bookshelf.getId(), 1), "111 222 111", {BlockDB::planks.getId(), ItemDB::book.getId()});
CraftingRecipe fence(ItemStack(BlockDB::fence.getId(), 2), "111 111", {ItemDB::stick.getId()});
CraftingRecipe diamondBlock(ItemStack(BlockDB::diamondBlock.getId(), 1), "111 111 111", {ItemDB::diamond.getId()});
}; // namespace RecipeDB::Crafting
