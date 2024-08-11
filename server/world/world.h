#pragma once

#include "entity/player/player.h"
#include "helper.h"

#include <array>
#include <cstddef>
#include <cstdint>

constexpr int32_t     CHUNK_SIZE       = 16 * 128 * 16;
constexpr ByteVector3 CHUNK_DIMS       = {15, 127, 15};
constexpr int32_t     CHUNK_COMPR_SIZE = CHUNK_SIZE * 2.5;

struct Nible {
  int8_t a : 4;
  int8_t b : 4;

  Nible(): a(0), b(0) {}

  Nible(int8_t _x): a(_x), b(_x) {}

  Nible(int8_t _a, int8_t _b): a(_a), b(_b) {}
};

template <std::size_t N>
class NibleArray {
  public:
  NibleArray(): m_data({0}) {}

  int8_t getNible(const IntVector3& pos) const {
    uint32_t pack = (uint32_t)pos.x << 11 | (uint32_t)pos.z << 7 | (uint32_t)pos.y;
    return (pack & 1) == 0 ? m_data[pack >> 1].a : m_data[pack >> 1].b;
  }

  void setNible(const IntVector3& pos, int8_t nible) {
    uint32_t pack = (uint32_t)pos.x << 11 | (uint32_t)pos.z << 7 | (uint32_t)pos.y;
    uint32_t idx  = pack >> 1;

    m_data[idx] = (pack & 1) == 0 ? (m_data[idx].b | (nible & 15)) : (m_data[idx].a | (nible & 15) << 4);
  }

  auto data() const { return m_data.data(); }

  auto size() const { return m_data.size(); }

  void fill(const Nible& nib) { m_data.fill(nib); }

  private:
  std::array<Nible, (N >> 1)> m_data;
};

class IWorld {
  public:
  struct Chunk {
    std::array<BlockId, CHUNK_SIZE> m_blocks = {};
    NibleArray<CHUNK_SIZE>          m_meta   = {};
    NibleArray<CHUNK_SIZE>          m_light  = {};
    NibleArray<CHUNK_SIZE>          m_sky    = {};

    static inline int32_t getLocalIndex(const IntVector3& pos) { return pos.y + (pos.z * 128) + (pos.x * 128 * 16); }

    static inline int32_t getWorldIndex(const IntVector3& pos) { return pos.y + ((pos.z & 15) * 128) + ((pos.x & 15) * 128 * 16); }

    static inline IntVector3 getPos(int32_t index) { return {index >> 11, index & 0x7f, (index & 0x780) >> 7}; }

    static inline IntVector2 toChunkCoords(const IntVector2& pos) { return {pos.x >> 4, pos.z >> 4}; }

    static inline IntVector3 toLocalChunkCoords(const IntVector3& pos) { return {pos.x & 15, pos.y, pos.z & 15}; }
  };

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

  virtual Chunk*      getChunk(const IntVector2& pos)                  = 0;
  virtual Chunk*      allocChunk(const IntVector2& pos)                = 0;
  virtual const void* compressChunk(Chunk* chunk, unsigned long& size) = 0;

  virtual bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) = 0;

  virtual bool setBlockWithNotify(const IntVector3& pos, BlockId id, int8_t meta, IPlayer* placer) = 0;

  virtual BlockId getBlock(const IntVector3& pos, int8_t* meta = nullptr) = 0;

  virtual void advanceTick(int64_t delta) = 0;

  virtual int64_t getTime() const = 0;

  virtual const IntVector3& getSpawnPoint() const = 0;
};

IWorld& accessWorld();
