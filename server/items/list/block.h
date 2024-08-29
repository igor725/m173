#pragma once

#include "../item.h"

class ItemBlock: public Item {
  public:
  ItemBlock(BlockId bid): Item((ItemId)bid - 256), m_blockId(bid) { maxStackSize = 64; }

  virtual bool place(EntityBase* user, const IntVector3& pos, int8_t direction, int8_t meta);

  bool onUseItemOnBlock(ItemStack& is, EntityBase* user, const IntVector3& pos, int8_t direction) override;

  bool onBlockDestroyed(ItemStack& is, const IntVector3& pos, BlockId id, EntityBase* destroyer) override;

  protected:
  BlockId m_blockId;
};
