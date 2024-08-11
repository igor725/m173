#include "items.h"

#include "list/baseSword.h"
#include "list/block.h"
#include "list/bow.h"
#include "list/lighter.h"
#include "list/snowBall.h"

#include <exception>

class InvalidItemIdException: public std::exception {
  public:
  InvalidItemIdException(ItemId iid) { m_what = std::format("Unknown item id {}", iid); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

std::array<Item*, 512> g_itemsList;

Item::Item(ItemId iid): shiftedIndex(256 + iid) {
  if (g_itemsList[shiftedIndex] != nullptr) {
    spdlog::warn("Items conflict {}", iid);
  }

  g_itemsList[shiftedIndex] = this;
}

Item* Item::getById(ItemId iid) {
  if (iid == -1) return g_itemsList[0];
  if (iid < 0 || iid > 512 || g_itemsList[iid] == nullptr) throw InvalidItemIdException(iid);
  return g_itemsList[iid];
}

Item::~Item() {
  g_itemsList[shiftedIndex] = nullptr;
}

static Item         ironShovel(0);
static Item         ironPickaxe(1);
static Item         ironAxe(2);
static ItemLighter  lighter(3);
static ItemLighter  apple(4);
static ItemBow      bow(5);
static Item         arrow(6);
static Item         coal(7);
static Item         diamond(8);
static Item         ironIngot(9);
static Item         goldIngot(10);
static ItemSword    ironSword(11, ToolMaterial::Iron);
static ItemSword    woodenSword(12, ToolMaterial::Wood);
static Item         woodenShovel(13);
static Item         woodenPickaxe(14);
static Item         woodenAxe(15);
static Item         stoneSword(16);
static Item         stoneShovel(17);
static Item         stonePickaxe(18);
static Item         stoneAxe(19);
static ItemSword    diamondSword(20, ToolMaterial::Diamond);
static Item         diamondShovel(21);
static Item         diamondPickaxe(22);
static Item         diamondAxe(23);
static Item         stick(24);
static Item         bowl(25);
static Item         soup(26);
static ItemSword    goldenSword(27, ToolMaterial::Gold);
static Item         goldenShovel(28);
static Item         goldenPickaxe(29);
static Item         goldenAxe(30);
static Item         string(31);
static Item         feather(32);
static Item         gunpowder(33);
static Item         woodenHoe(34);
static Item         stoneHoe(35);
static Item         ironHoe(36);
static Item         diamondHoe(37);
static Item         goldenHoe(38);
static Item         seeds(39);
static Item         wheat(40);
static Item         bread(41);
static Item         leatherHelmet(42);
static Item         leatherChest(43);
static Item         leatherLeggings(44);
static Item         leatherBoots(45);
static Item         chainHelmet(46);
static Item         chainChest(47);
static Item         chainLeggings(48);
static Item         chainBoots(49);
static Item         ironHelmet(50);
static Item         ironChest(51);
static Item         ironLeggings(52);
static Item         ironBoots(53);
static Item         diamonHelmet(54);
static Item         diamonChest(55);
static Item         diamonLeggings(56);
static Item         diamonBoots(57);
static Item         goldenHelmet(58);
static Item         goldenChest(59);
static Item         goldenLeggings(60);
static Item         goldenBoots(61);
static Item         flint(62);
static Item         rawPork(63);
static Item         cookedPork(64);
static Item         painting(65);
static Item         goldenApple(66);
static Item         sign(67);
static Item         door(68);
static Item         bucket(69);
static Item         waterBucket(70);
static Item         lavaBucket(71);
static Item         minecart(72);
static Item         saddle(73);
static Item         ironDoor(74);
static Item         redstonePowder(75);
static ItemSnowball snowBall(76);
static Item         boat(77);
static Item         leather(78);
static Item         milkBucket(79);
static Item         brick(80);
static Item         clay(81);
static Item         sugarCane(82);
static Item         paper(83);
static Item         book(84);
static Item         slime(85);
static Item         chestMinecart(86);
static Item         furnaceMinecart(87);
static Item         egg(88);
static Item         compass(89);
static Item         fishingRod(90);
static Item         clockItem(91);
static Item         glowDust(92);
static Item         fish(93);
static Item         cookedFish(94);
static Item         genericDye(95);
static Item         bone(96);
static Item         sugar(97);
static Item         cake(98);
static Item         bed(99);
static Item         redstoneRepeater(100);
static Item         cookie(101);
static Item         map(102);
static Item         shears(103);
