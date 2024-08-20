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

/**
 * @brief This API is NOT thread safe! It should be used only
 * internally from World class and should never be accessed
 * by anything else outside of the World class.
 *
 * @return IMCRegion&, Minecraft Region manager
 */
IMCRegion& accessRegionManager();
