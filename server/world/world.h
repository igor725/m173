#pragma once

#include "chunk.h"
#include "entity/player/player.h"
#include "helper.h"

#include <cstdint>

class IWorld {
  public:
  IWorld()          = default;
  virtual ~IWorld() = default;

  virtual ChunkUnique getChunk(const IntVector2& pos) = 0;

  virtual bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) = 0;

  virtual bool setBlockWithNotify(const IntVector3& pos, BlockId id, int8_t meta, IPlayer* placer) = 0;

  virtual BlockId getBlock(const IntVector3& pos, int8_t* meta = nullptr) = 0;

  virtual int64_t getSeed() const = 0;

  virtual int64_t getTime() const = 0;

  virtual size_t getChunksCount() = 0;

  virtual const IntVector3& getSpawnPoint() const = 0;

  virtual void finish() = 0;
};

IWorld& accessWorld();
