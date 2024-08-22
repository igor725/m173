#pragma once

#include "../interface.h"

class FlatGen: public IGenerator {
  public:
  FlatGen(int64_t seed);
  virtual ~FlatGen() = default;

  void getSpawnPoint(IntVector3& pos) final;

  void fillChunk(const IntVector2& pos, const ChunkUnique& chunk) final;

  private:
  IntVector3 m_spawnPoint;
};
