#pragma once

#include "helper.h"

#include <array>
#include <cstdint>

constexpr int32_t     CHUNK_SIZE       = 16 * 127 * 16;
constexpr ByteVector3 CHUNK_DIMS       = {15, 126, 15};
constexpr int32_t     CHUNK_COMPR_SIZE = CHUNK_SIZE * 2.5;

class IWorld {
  public:
  struct Chunk {
    struct BlockPack {
      uint8_t block1 : 4;
      uint8_t block2 : 4;
    };

    static_assert(sizeof(BlockPack) == 1, "BlockPack should be 1 byte!");

    std::array<BlockId, CHUNK_SIZE>       m_blocks = {};
    std::array<BlockPack, CHUNK_SIZE / 2> m_meta   = {};
    std::array<BlockPack, CHUNK_SIZE / 2> m_light  = {};
    std::array<BlockPack, CHUNK_SIZE / 2> m_sky    = {};

    static inline int32_t getLocalIndex(const IntVector3& pos) { return pos.y + (pos.z * 127) + (pos.x * 127 * 16); }

    static inline int32_t getWorldIndex(const IntVector3& pos) { return (pos.y & 127) + ((pos.z & 15) * 127) + ((pos.x & 15) * 127 * 16); }

    static inline IntVector3 getPos(int32_t index) { return {index >> 11, index & 0x7f, (index & 0x780) >> 7}; }
  };

  IWorld()          = default;
  virtual ~IWorld() = default;

  static inline uint64_t packChunkPos(const IntVector2& pos) { return (int64_t)pos.x << 32 | pos.z; }

  virtual Chunk*      getChunk(const IntVector2& pos)                  = 0;
  virtual Chunk*      allocChunk(const IntVector2& pos)                = 0;
  virtual const void* compressChunk(Chunk* chunk, unsigned long& size) = 0;

  virtual bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) = 0;
};

IWorld& accessWorld();
