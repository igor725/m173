#pragma once

#include "entity/player/player.h"
#include "helper.h"
#include "nibbleArray.h"

#include <array>
#include <cstdint>

constexpr ByteVector3 CHUNK_DIMS       = {15, 125, 15};
constexpr int32_t     CHUNK_SIZE       = (CHUNK_DIMS.x + 1) * (CHUNK_DIMS.y + 1) * (CHUNK_DIMS.x + 1);
constexpr int32_t     CHUNK_COMPR_SIZE = CHUNK_SIZE * 2.5;

struct Chunk { // todo unique access here
  std::array<BlockId, CHUNK_SIZE> m_blocks = {};
  NibbleArray<CHUNK_SIZE>         m_meta   = {};
  NibbleArray<CHUNK_SIZE>         m_light  = {};
  NibbleArray<CHUNK_SIZE>         m_sky    = {};

  static inline int32_t getLocalIndex(const IntVector3& pos) { return pos.y + (pos.z * (CHUNK_DIMS.y + 1)) + (pos.x * (CHUNK_DIMS.y + 1) * 16); }

  static inline int32_t getWorldIndex(const IntVector3& pos) { return pos.y + ((pos.z & 15) * (CHUNK_DIMS.y + 1)) + ((pos.x & 15) * (CHUNK_DIMS.y + 1) * 16); }

  static inline IntVector2 toChunkCoords(const IntVector2& pos) { return {pos.x >> 4, pos.z >> 4}; }

  static inline IntVector3 toLocalChunkCoords(const IntVector3& pos) { return {pos.x & 15, pos.y, pos.z & 15}; }
};

class IWorld {
  public:
  IWorld()          = default;
  virtual ~IWorld() = default;

  static inline int64_t packChunkPos(const IntVector2& pos) {
    union {
      int64_t offset;

      struct {
        int64_t x : 32;
        int64_t z : 32;
      };
    } u;

    u.x = pos.x;
    u.z = pos.z;

    return u.offset;
  }

  virtual Chunk* getChunk(const IntVector2& pos)   = 0;
  virtual Chunk* allocChunk(const IntVector2& pos) = 0;

  virtual bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) = 0;

  virtual bool setBlockWithNotify(const IntVector3& pos, BlockId id, int8_t meta, IPlayer* placer) = 0;

  virtual BlockId getBlock(const IntVector3& pos, int8_t* meta = nullptr) = 0;

  virtual void advanceTick(int64_t delta) = 0;

  virtual int64_t getSeed() const = 0;

  virtual int64_t getTime() const = 0;

  virtual size_t getChunksCount() = 0;

  virtual const IntVector3& getSpawnPoint() const = 0;

  virtual void finish() = 0;
};

IWorld& accessWorld();
