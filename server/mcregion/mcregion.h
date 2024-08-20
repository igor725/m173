#pragma once

#include "helper.h"
#include "world/chunk.h"

class IMCRegion {
  public:
  IMCRegion()          = default;
  virtual ~IMCRegion() = default;

  virtual bool loadChunk(const IntVector2& pos, Chunk& chunk) = 0;

  virtual bool saveChunk(const IntVector2& pos, Chunk& chunk) = 0;
};

IMCRegion& accessRegionManager();
