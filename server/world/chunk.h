#pragma once

#include "helper.h"
#include "nibbleArray.h"

#include <array>
#include <cstdint>
#include <memory>
#include <mutex>

constexpr ByteVector3 CHUNK_DIMS = {15, 125, 15};
constexpr int32_t     CHUNK_SIZE = (CHUNK_DIMS.x + 1) * (CHUNK_DIMS.y + 1) * (CHUNK_DIMS.x + 1);

struct Chunk {
  std::array<BlockId, CHUNK_SIZE> m_blocks = {};
  NibbleArray<CHUNK_SIZE>         m_meta   = {};
  NibbleArray<CHUNK_SIZE>         m_light  = {};
  NibbleArray<CHUNK_SIZE>         m_sky    = {};

  std::recursive_mutex m_lock;
  int32_t              m_uses;
  double_t             m_unloadTimer;
  bool                 m_wasUpdated;
  IntVector2           m_position;

  Chunk(double_t utimerInit, const IntVector2& pos): m_unloadTimer(utimerInit), m_wasUpdated(false), m_uses(0), m_position(pos) {}

  static inline int32_t getBlockOffset(const IntVector3& pos) {
    return pos.y + ((pos.z & CHUNK_DIMS.z) * (CHUNK_DIMS.y + 1)) + ((pos.x & CHUNK_DIMS.x) * (CHUNK_DIMS.y + 1) * (CHUNK_DIMS.z + 1));
  }

  inline IntVector3 getStartBlock() const { return {m_position.x << 4, 0, m_position.z << 4}; }

  static inline IntVector2 toChunkCoords(const IntVector2& pos) { return {pos.x >> 4, pos.z >> 4}; }

  static inline IntVector3 toLocalChunkCoords(const IntVector3& pos) { return {pos.x & CHUNK_DIMS.x, pos.y, pos.z & CHUNK_DIMS.z}; }
};

class ChunkUnique {
  public:
  ChunkUnique(std::unique_ptr<Chunk>& chunk): m_chunk(chunk) { chunk->m_lock.lock(); }

  ~ChunkUnique() { m_chunk->m_lock.unlock(); }

  Chunk* operator->() const { return m_chunk.get(); }

  private:
  std::unique_ptr<Chunk>& m_chunk;
};
