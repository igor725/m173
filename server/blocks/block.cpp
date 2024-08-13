#include "block.h"

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
