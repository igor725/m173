#include "blocks.h"

#include "list/noteBlock.h"

#include <array>
#include <exception>
#include <format>
#include <spdlog/spdlog.h>
#include <string>

class InvalidBlockIdException: public std::exception {
  public:
  InvalidBlockIdException(BlockId bid) { m_what = std::format("Unknown item id {}", bid); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

std::array<Block*, 256> g_blocks = {};

Block::Block(BlockId bid): m_index(bid), m_item(bid) {
  if (g_blocks[bid] != nullptr) {
    spdlog::warn("Block id conflict {}!", bid);
  }

  g_blocks[bid] = this;
}

Block::~Block() {
  g_blocks[m_index] = nullptr;
}

Block* Block::getById(BlockId bid) {
  return g_blocks[bid];
}

static Block     air(0);
static Block     stone(1);
static Block     grassBlock(2);
static Block     dirt(3);
static Block     cobblestone(4);
static Block     wood(5);
static Block     sapling(6);
static Block     bedrock(7);
static Block     water(8);
static Block     waterStill(9);
static Block     lava(10);
static Block     lavaStill(11);
static Block     sand(12);
static Block     gravel(13);
static Block     goldOre(14);
static Block     ironOre(15);
static Block     coalOre(16);
static Block     logBlock(17);
static Block     leaves(18);
static Block     sponge(19);
static Block     glass(20);
static Block     lapisOre(21);
static Block     lapisBlock(22);
static Block     dispenser(23);
static Block     sandStone(24);
static NoteBlock noteBlock(25);
static Block     bed(26);
static Block     poweredRail(27);
static Block     buttonRail(28);
static Block     stickyPiston(29);
static Block     cobweb(30);
static Block     bush(31);
static Block     anotherBush(32);
static Block     piston(33);
static Block     pistonMover(34);
static Block     wool(35);
static Block     yellowFlower(37);
static Block     redFlower(38);
static Block     mushroom(39);
static Block     mushroom2(40);
static Block     goldBlock(41);
static Block     ironBlock(42);
static Block     slabBlock(43);
static Block     slab(44);
static Block     brick(45);
static Block     tnt(46);
static Block     bookshelf(47);
static Block     mossyStone(48);
static Block     obsidian(49);
static Block     torch(50);
static Block     fire(51);
static Block     spawner(52);
static Block     stairs(53);
static Block     chest(54);
static Block     redstone(55);
static Block     diamondOre(56);
static Block     diamondBlock(57);
static Block     workbench(58);
static Block     spring(59);
static Block     shovDirt(60);
static Block     furnace(61);
static Block     woodDoorFrame(64);
static Block     ladder(65);
static Block     rail(66);
static Block     stoneStairs(67);
static Block     lever(69);
static Block     stonePressurePlate(70);
static Block     ironDoorFrame(71);
static Block     woodPressurePlate(72);
static Block     actRedstoneOre(73);
static Block     redstoneOre(74);
static Block     unlitRedstoneTorch(75);
static Block     redstoneTorch(76);
static Block     stoneButton(77);
static Block     snow(78);
static Block     ice(79);
static Block     snowBlock(80);
static Block     cactus(81);
static Block     clay(82);
static Block     sugarcane(83);
static Block     jukebox(84);
static Block     fence(85);
static Block     pumpkin(86);
static Block     netherrack(87);
static Block     soulsand(88);
static Block     glovestone(89);
static Block     portal(90);
static Block     litPumpkin(91);
static Block     cake(92);
static Block     trapdoor(96);
