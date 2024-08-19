#pragma once

#include "block.h"

class ItemWool: public ItemBlock {
  public:
  enum Colour : int8_t {
    White,
    Orange,
    Magenta,
    LightBlue,
    Yellow,
    Lime,
    Pink,
    Gray,
    LightGray,
    Cyan,
    Purple,
    Blue,
    Brown,
    Green,
    Red,
    Black,
  };

  ItemWool(BlockId bid): ItemBlock(bid) {
    maxStackSize = 64;
    maxDamage    = 0;
  }

  int8_t getMetadata(int16_t dmg) const final { return Colour(dmg); }
};
