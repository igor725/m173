#include "block.h"

#include <array>
#include <exception>
#include <format>
#include <spdlog/spdlog.h>
#include <string>

class InvalidBlockIdException: public std::exception {
  public:
  InvalidBlockIdException(BlockId bid) { m_what = std::format("Unknown block id {}", bid); }

  const char* what() const noexcept override { return m_what.c_str(); }

  private:
  std::string m_what;
};

std::array<Block*, 256> g_blocksList = {};

Block::Block(BlockId bid): m_index(bid) {
  if (g_blocksList[bid] != nullptr) {
    spdlog::warn("Block id conflict {}!", bid);
  }

  g_blocksList[bid] = this;
}

Block::~Block() {
  g_blocksList[m_index] = nullptr;
}

Block* Block::getById(BlockId bid) {
  return g_blocksList[bid];
}
