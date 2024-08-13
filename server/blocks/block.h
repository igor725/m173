#pragma once

#include "entity/entitybase.h"
#include "helper.h"
#include "items/list/block.h"

class Block {
  public:
  Block(BlockId bid);
  ~Block();

  virtual bool blockActivated(const IntVector3& pos, EntityBase* activator) { return false; }

  BlockId getId() const { return m_index; }

  static Block* getById(BlockId bid);

  private:
  BlockId m_index;
};