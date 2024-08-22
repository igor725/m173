#include "flat.h"

FlatGen::FlatGen(int64_t seed): IGenerator(seed) {
  m_spawnPoint = {0, 15, 0};
}

void FlatGen::getSpawnPoint(IntVector3& pos) {
  pos = m_spawnPoint;
}

void FlatGen::fillChunk(const IntVector2& pos, const ChunkUnique& chunk) {
  chunk->m_light.fill(Nibble(15, 15)); // All fullbright for now
  chunk->m_sky.fill(Nibble(15, 15));

  for (int32_t x = 0; x < 16; ++x) {
    for (int32_t y = 0; y < (m_spawnPoint.y - 2); ++y) {
      for (int32_t z = 0; z < 16; ++z) {
        chunk->m_blocks[chunk->getBlockOffset({x, y, z})] = y < 1 ? 7 : y < (m_spawnPoint.y - 3) ? 3 : 2;
      }
    }
  }
}
