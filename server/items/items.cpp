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

static ItemLighter  lighter(3);
static ItemBow      bow(5);
static ItemBow      arrow(6);
static ItemSword    ironSword(11, ToolMaterial::Iron);
static ItemSword    woodenSword(12, ToolMaterial::Wood);
static ItemSword    diamondSword(20, ToolMaterial::Diamond);
static ItemSword    goldSword(27, ToolMaterial::Gold);
static ItemSnowball snowBall(76);
static Item         compass(89);
