#include "items.h"

#include "list/baseSword.h"
#include "list/block.h"
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

static class BlockRegistrator {
  public:
  BlockRegistrator() {
    m_list.reserve(256);
    for (BlockId i = 0; i <= 254; ++i) {
      m_list.emplace_back(i);
    }
  }

  private:
  std::vector<ItemBlock> m_list = {};
} blocks;

static ItemSword    ironSword(11, ItemSword::Iron);
static ItemSword    woodenSword(12, ItemSword::Wood);
static ItemSword    diamondSword(20, ItemSword::Diamond);
static ItemSword    goldSword(27, ItemSword::Gold);
static ItemSnowball snowBall(76);

Item::Item(ItemId iid): shiftedIndex(256 + iid) {
  if (g_itemsList[shiftedIndex] != nullptr) {
    spdlog::warn("Items conflict {}", iid);
  }

  g_itemsList[shiftedIndex] = this;
}

Item* Item::getById(ItemId iid) {
  if (iid == -1) return g_itemsList[0];
  if (iid < 0 || iid > 512) throw InvalidItemIdException(iid);
  return g_itemsList[iid];
}

Item::~Item() {
  g_itemsList[shiftedIndex] = nullptr;
}
