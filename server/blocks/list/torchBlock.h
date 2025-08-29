#pragma once

#include "../block.h"
#include "entry/helper.h"
#include "items/list/torch.h"

class TorchBlock: public Block {
  public:
  TorchBlock(BlockId bid): Block(bid), m_item(bid) { m_hardness = 0.0f; }

  private:
  ItemTorch m_item;
};
