#pragma once

#include "helper.h"
#include "nibbleArray.h"

#include <array>
#include <cstdint>

constexpr ByteVector3 CHUNK_DIMS              = {15, 125, 15};
constexpr int32_t     CHUNK_SIZE              = (CHUNK_DIMS.x + 1) * (CHUNK_DIMS.y + 1) * (CHUNK_DIMS.x + 1);
constexpr int32_t     CHUNK_UNLOAD_TIMER_INIT = 60000;

struct Chunk { // todo unique access here
  std::array<BlockId, CHUNK_SIZE> m_blocks = {};
  NibbleArray<CHUNK_SIZE>         m_meta   = {};
  NibbleArray<CHUNK_SIZE>         m_light  = {};
  NibbleArray<CHUNK_SIZE>         m_sky    = {};

  int32_t unloadTimer = CHUNK_UNLOAD_TIMER_INIT;

  static inline int32_t getBlockOffset(const IntVector3& pos) {
    return pos.y + ((pos.z & CHUNK_DIMS.z) * (CHUNK_DIMS.y + 1)) + ((pos.x & CHUNK_DIMS.x) * (CHUNK_DIMS.y + 1) * (CHUNK_DIMS.z + 1));
  }

  static inline IntVector2 toChunkCoords(const IntVector2& pos) { return {pos.x >> 4, pos.z >> 4}; }

  static inline IntVector3 toLocalChunkCoords(const IntVector3& pos) { return {pos.x & CHUNK_DIMS.x, pos.y, pos.z & CHUNK_DIMS.z}; }
};
