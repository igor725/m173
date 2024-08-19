#include "armorrecipes.h"
#include "blockrecipes.h"
#include "blocks/blockDB.h"
#include "items/itemDB.h"
#include "recipe.h"
#include "toolrecipes.h"

namespace RecipeDB::Crafting {
ToolRecipes  tools;
ArmorRecipes armor;
BlockRecipes blocks;

CraftingRecipeAR workbench(ItemStack(BlockDB::workbench.getId(), 1), "11 11", {BlockDB::planks.getId()});
CraftingRecipeAR furnace(ItemStack(BlockDB::furnace.getId(), 1), "111 101 111", {BlockDB::cobblestone.getId()});
CraftingRecipeAR chest(ItemStack(BlockDB::chest.getId(), 1), "111 101 111", {BlockDB::planks.getId()});
CraftingRecipeAR bed(ItemStack(ItemDB::bed.getId(), 1), "111 222", {BlockDB::wool.getId(), BlockDB::planks.getId()});
CraftingRecipeAR stick(ItemStack(ItemDB::stick.getId(), 3), "1", {BlockDB::planks.getId()});
CraftingRecipeAR planks(ItemStack(BlockDB::planks.getId(), 4), "1", {BlockDB::logBlock.getId()});
CraftingRecipeAR book(ItemStack(ItemDB::book.getId(), 1), "11 12", {ItemDB::paper.getId(), ItemDB::leather.getId()});
CraftingRecipeAR bookshelf(ItemStack(BlockDB::bookshelf.getId(), 1), "111 222 111", {BlockDB::planks.getId(), ItemDB::book.getId()});
CraftingRecipeAR fence(ItemStack(BlockDB::fence.getId(), 2), "111 111", {ItemDB::stick.getId()});
CraftingRecipeAR diamondBlock(ItemStack(BlockDB::diamondBlock.getId(), 1), "111 111 111", {ItemDB::diamond.getId()});
CraftingRecipeAR jukebox(ItemStack(BlockDB::jukebox.getId(), 1), "111 121 111", {BlockDB::planks.getId(), ItemDB::diamond.getId()});
CraftingRecipeAR noteblock(ItemStack(BlockDB::noteBlock.getId(), 1), "111 121 111", {BlockDB::planks.getId(), ItemDB::redstonePowder.getId()});
CraftingRecipeAR snowball(ItemStack(BlockDB::snowBlock.getId(), 1), "11 11", {ItemDB::snowBall.getId()});
CraftingRecipeAR sandStone(ItemStack(BlockDB::sandStone.getId(), 1), "11 11", {BlockDB::sand.getId()});
CraftingRecipeAR clay(ItemStack(BlockDB::clayBlock.getId(), 1), "11 11", {ItemDB::clay.getId()});
CraftingRecipeAR brick(ItemStack(BlockDB::brick.getId(), 1), "11 11", {ItemDB::brick.getId()});
CraftingRecipeAR glovestone(ItemStack(BlockDB::glovestone.getId(), 1), "11 11", {ItemDB::glowDust.getId()});
CraftingRecipeAR wool(ItemStack(BlockDB::wool.getId(), 1), "11 11", {ItemDB::string.getId()});
CraftingRecipeAR bow(ItemStack(ItemDB::bow.getId(), 1), "012 102 012", {ItemDB::stick.getId(), ItemDB::string.getId()});
CraftingRecipeAR arrow(ItemStack(ItemDB::arrow.getId(), 4), "1 2 3", {ItemDB::flint.getId(), ItemDB::stick.getId(), ItemDB::feather.getId()});
CraftingRecipeAR tnt(ItemStack(BlockDB::tnt.getId(), 1), "121 212 121", {ItemDB::gunpowder.getId(), BlockDB::sand.getId()});
CraftingRecipeAR rail(ItemStack(BlockDB::rail.getId(), 16), "101 121 101", {ItemDB::ironIngot.getId(), ItemDB::stick.getId()});
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
CraftingRecipeAR litPumpkin(ItemStack(BlockDB::litPumpkin.getId(), 1), "1 2", {BlockDB::pumpkin.getId(), BlockDB::torch.getId()});
CraftingRecipeAR painting(ItemStack(ItemDB::painting.getId(), 1), "111 121 111", {ItemDB::stick.getId(), BlockDB::wool.getId()});
CraftingRecipeAR bowl(ItemStack(ItemDB::bowl.getId(), 4), "101 010", {BlockDB::planks.getId()});
CraftingRecipeAR soup(ItemStack(ItemDB::soup.getId(), 1), "1 2 3", {BlockDB::mushroom2.getId(), BlockDB::mushroom.getId(), ItemDB::bowl.getId()});
CraftingRecipeAR soup2(ItemStack(ItemDB::soup.getId(), 1), "1 2 3", {BlockDB::mushroom.getId(), BlockDB::mushroom2.getId(), ItemDB::bowl.getId()});
CraftingRecipeAR cookie(ItemStack(ItemDB::cookie.getId(), 8), "121", {ItemDB::wheat.getId(), ItemStack(ItemDB::genericDye.getId(), 3)});
CraftingRecipeAR bucket(ItemStack(ItemDB::bucket.getId(), 1), "101 010", {ItemDB::ironIngot.getId()});
CraftingRecipeAR lighter(ItemStack(ItemDB::lighter.getId(), 1), "10 02", {ItemDB::ironIngot.getId(), ItemDB::flint.getId()});
CraftingRecipeAR stairs(ItemStack(BlockDB::stairs.getId(), 4), "1 11 111", {BlockDB::planks.getId()});
CraftingRecipeAR stoneStairs(ItemStack(BlockDB::stoneStairs.getId(), 4), "1 11 111", {BlockDB::cobblestone.getId()});
CraftingRecipeAR fishingRod(ItemStack(ItemDB::fishingRod.getId(), 1), "001 012 102", {ItemDB::stick.getId(), ItemDB::string.getId()});
CraftingRecipeAR goldenApple(ItemStack(ItemDB::goldenApple.getId(), 1), "111 121 111", {BlockDB::goldBlock.getId(), ItemDB::apple.getId()});
CraftingRecipeAR shears(ItemStack(ItemDB::shears.getId(), 1), "10 01", {ItemDB::ironIngot.getId()});

// Redstone related stuff
CraftingRecipeAR button(ItemStack(BlockDB::stoneButton.getId(), 1), "1 1", {BlockDB::stone.getId()});
CraftingRecipeAR lever(ItemStack(BlockDB::lever.getId(), 1), "1 2", {ItemDB::stick.getId(), BlockDB::cobblestone.getId()});
CraftingRecipeAR redTorch(ItemStack(BlockDB::redstoneTorch.getId(), 1), "1 2", {ItemDB::redstonePowder.getId(), ItemDB::stick.getId()});
CraftingRecipeAR woodenPressure(ItemStack(BlockDB::woodPressurePlate.getId(), 1), "11", {BlockDB::planks.getId()});
CraftingRecipeAR stonePressure(ItemStack(BlockDB::stonePressurePlate.getId(), 1), "11", {BlockDB::stone.getId()});
CraftingRecipeAR compass(ItemStack(ItemDB::compass.getId(), 1), "010 121 010", {ItemDB::ironIngot.getId(), ItemDB::redstonePowder.getId()});
CraftingRecipeAR stickyPiston(ItemStack(BlockDB::stickyPiston.getId(), 1), "1 2", {ItemDB::slime.getId(), BlockDB::piston.getId()});
CraftingRecipeAR piston(ItemStack(BlockDB::piston.getId(), 1), "111 232 242",
                        {BlockDB::planks.getId(), BlockDB::cobblestone.getId(), ItemDB::redstonePowder.getId(), ItemDB::redstonePowder.getId()});
CraftingRecipeAR dispenser(ItemStack(BlockDB::dispenser.getId(), 1), "111 121 131",
                           {BlockDB::cobblestone.getId(), ItemDB::bow.getId(), ItemDB::redstonePowder.getId()});
CraftingRecipeAR sundial(ItemStack(ItemDB::clockItem.getId(), 1), "010 121 010", {ItemDB::goldIngot.getId(), ItemDB::redstonePowder.getId()});
CraftingRecipeAR map(ItemStack(ItemDB::map.getId(), 1), "111 121 111", {ItemDB::paper.getId(), ItemDB::compass.getId()});
CraftingRecipeAR repeater(ItemStack(ItemDB::redstoneRepeater.getId(), 1), "121 333",
                          {BlockDB::redstoneTorch.getId(), ItemDB::redstonePowder.getId(), BlockDB::stone.getId()});
CraftingRecipeAR redRail(ItemStack(BlockDB::poweredRail.getId(), 6), "101 121 131",
                         {ItemDB::ironIngot.getId(), ItemDB::stick.getId(), ItemDB::redstonePowder.getId()});
CraftingRecipeAR btnRail(ItemStack(BlockDB::buttonRail.getId(), 6), "101 121 131",
                         {ItemDB::ironIngot.getId(), BlockDB::stonePressurePlate.getId(), ItemDB::redstonePowder.getId()});

// Entities/Objects
CraftingRecipeAR boat(ItemStack(ItemDB::boat.getId(), 1), "101 111", {BlockDB::planks.getId()});
CraftingRecipeAR minecart(ItemStack(ItemDB::minecart.getId(), 1), "101 111", {ItemDB::ironIngot.getId()});
CraftingRecipeAR chestMinecart(ItemStack(ItemDB::chestMinecart.getId(), 1), "1 2", {BlockDB::chest.getId(), ItemDB::minecart.getId()});
CraftingRecipeAR furnaceMinecart(ItemStack(ItemDB::furnaceMinecart.getId(), 1), "1 2", {BlockDB::furnace.getId(), ItemDB::minecart.getId()});

}; // namespace RecipeDB::Crafting
