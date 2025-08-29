#pragma once

#include "block.h"

class ItemTorch: public ItemBlock {
  public:
  ItemTorch(BlockId bid): ItemBlock(bid) {}

  bool place(Entities::Base* user, const IntVector3& npos, int8_t direction, int8_t meta) final;
};
