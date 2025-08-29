#pragma once

#include "entity/entitybase.h"
#include "entry/helper.h"

class Block {
  public:
  Block(BlockId bid);

  ~Block();

  virtual bool blockActivated(const IntVector3& pos, Entities::Base* activator) { return false; }

  BlockId getId() const { return m_index; }

  float_t getHardness() const { return m_hardness; }

  static Block* getById(BlockId bid);

  protected:
  float_t m_hardness = 0.1f;

  private:
  BlockId m_index;
};
