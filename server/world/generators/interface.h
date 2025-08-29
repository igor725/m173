#pragma once

#include "../chunk.h"
#include "entry/helper.h"

class IGenerator {
  public:
  IGenerator(int64_t seed): m_seed(seed) {}

  virtual ~IGenerator() = default;

  int64_t getSeed() const { return m_seed; }

  virtual void getSpawnPoint(IntVector3& pos) = 0;

  virtual void fillChunk(const IntVector2& pos, const ChunkUnique& chunk) = 0;

  protected:
  int64_t m_seed;
};
