#include "item.h"

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
