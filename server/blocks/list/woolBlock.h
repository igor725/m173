#pragma once

#include "../database.h"
#include "entry/helper.h"
#include "items/list/wool.h"

class WoolBlock: public Block {
  public:
  WoolBlock(BlockId bid): Block(bid), m_item(bid) {}

  private:
  ItemWool m_item;
};
