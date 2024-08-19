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
CraftingRecipeAR planks(ItemStack(BlockDB::planks.getId(), 4), "1", {BlockDB::logBlock.getId()});
CraftingRecipeAR book(ItemStack(ItemDB::book.getId(), 1), "11 12", {ItemDB::paper.getId(), ItemDB::leather.getId()});
CraftingRecipeAR bookshelf(ItemStack(BlockDB::bookshelf.getId(), 1), "111 222 111", {BlockDB::planks.getId(), ItemDB::book.getId()});
CraftingRecipeAR fence(ItemStack(BlockDB::fence.getId(), 2), "111 111", {ItemDB::stick.getId()});
CraftingRecipeAR diamondBlock(ItemStack(BlockDB::diamondBlock.getId(), 1), "111 111 111", {ItemDB::diamond.getId()});
CraftingRecipeAR jukebox(ItemStack(BlockDB::jukebox.getId(), 1), "111 121 111", {BlockDB::planks.getId(), ItemDB::diamond.getId()});
CraftingRecipeAR noteblock(ItemStack(BlockDB::noteBlock.getId(), 1), "111 121 111", {BlockDB::planks.getId(), ItemDB::redstonePowder.getId()});
CraftingRecipeAR snowball(ItemStack(BlockDB::snowBlock.getId(), 1), "11 11", {ItemDB::snowBall.getId()});
CraftingRecipeAR clay(ItemStack(BlockDB::clayBlock.getId(), 1), "11 11", {ItemDB::clay.getId()});
CraftingRecipeAR brick(ItemStack(BlockDB::brick.getId(), 1), "11 11", {ItemDB::brick.getId()});
CraftingRecipeAR glovestone(ItemStack(BlockDB::glovestone.getId(), 1), "11 11", {ItemDB::glowDust.getId()});
CraftingRecipeAR wool(ItemStack(BlockDB::wool.getId(), 1), "11 11", {ItemDB::string.getId()});
CraftingRecipeAR tnt(ItemStack(BlockDB::wool.getId(), 1), "121 212 121", {ItemDB::gunpowder.getId(), BlockDB::sand.getId()});
CraftingRecipeAR stoneSlab(ItemStack(BlockDB::slab.getId(), 3, 0), "111", {BlockDB::stone.getId()});
CraftingRecipeAR sandSlab(ItemStack(BlockDB::slab.getId(), 3, 1), "111", {BlockDB::sandStone.getId()});
CraftingRecipeAR woodenSlab(ItemStack(BlockDB::slab.getId(), 3, 2), "111", {BlockDB::planks.getId()});
CraftingRecipeAR cobbleSlab(ItemStack(BlockDB::slab.getId(), 3, 3), "111", {BlockDB::cobblestone.getId()});
CraftingRecipeAR ladder(ItemStack(BlockDB::ladder.getId(), 2), "101 111 101", {ItemDB::stick.getId()});
CraftingRecipeAR trapdoor(ItemStack(BlockDB::trapdoor.getId(), 2), "111 111", {BlockDB::planks.getId()});
CraftingRecipeAR woodenDoor(ItemStack(ItemDB::door.getId(), 1), "11 11 11", {BlockDB::planks.getId()});
CraftingRecipeAR ironDoor(ItemStack(ItemDB::ironDoor.getId(), 1), "11 11 11", {ItemDB::ironIngot.getId()});
CraftingRecipeAR sign(ItemStack(ItemDB::sign.getId(), 1), "111 111 020", {BlockDB::planks.getId(), ItemDB::stick.getId()});
CraftingRecipeAR cake(ItemStack(ItemDB::cake.getId(), 1), "111 232 444",
                      {ItemDB::milkBucket.getId(), ItemDB::sugar.getId(), ItemDB::egg.getId(), ItemDB::wheat.getId()});
CraftingRecipeAR sugar(ItemStack(ItemDB::sugar.getId(), 1), "1", {ItemDB::sugarCane.getId()});
CraftingRecipeAR bread(ItemStack(ItemDB::bread.getId(), 1), "111", {ItemDB::wheat.getId()});
CraftingRecipeAR paper(ItemStack(ItemDB::paper.getId(), 3), "111", {ItemDB::sugarCane.getId()});
CraftingRecipeAR torch(ItemStack(BlockDB::torch.getId(), 4), "1 2", {ItemDB::coal.getId(), ItemDB::stick.getId()});
CraftingRecipeAR torch2(ItemStack(BlockDB::torch.getId(), 4), "1 2", {ItemStack(ItemDB::coal.getId(), 1, 1), ItemDB::stick.getId()});
CraftingRecipeAR bowl(ItemStack(ItemDB::bowl.getId(), 4), "101 010", {BlockDB::planks.getId()});
CraftingRecipeAR bucket(ItemStack(ItemDB::bucket.getId(), 1), "101 010", {ItemDB::ironIngot.getId()});
CraftingRecipeAR lighter(ItemStack(ItemDB::lighter.getId(), 1), "10 02", {ItemDB::ironIngot.getId(), ItemDB::flint.getId()});
CraftingRecipeAR stairs(ItemStack(BlockDB::stairs.getId(), 4), "1 11 111", {BlockDB::planks.getId()});
CraftingRecipeAR stoneStairs(ItemStack(BlockDB::stoneStairs.getId(), 4), "1 11 111", {BlockDB::cobblestone.getId()});
CraftingRecipeAR fishingRod(ItemStack(ItemDB::fishingRod.getId(), 1), "001 012 102", {ItemDB::stick.getId(), ItemDB::string.getId()});
CraftingRecipeAR goldenApple(ItemStack(ItemDB::goldenApple.getId(), 1), "111 121 111", {BlockDB::goldBlock.getId(), ItemDB::apple.getId()});
CraftingRecipeAR compass(ItemStack(ItemDB::compass.getId(), 1), "010 121 010", {ItemDB::ironIngot.getId(), ItemDB::redstonePowder.getId()});
}; // namespace RecipeDB::Crafting
