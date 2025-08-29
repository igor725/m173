#pragma once

#include "entry/helper.h"
#include "world/chunk.h"

#include <memory>
#include <string>

class IRegionFile {
  public:
  IRegionFile()          = default;
  virtual ~IRegionFile() = default;

  virtual bool readChunk(const IntVector2& pos, const ChunkUnique& chunk) = 0;

  virtual bool writeChunk(const IntVector2& pos, const ChunkUnique& chunk) = 0;

  virtual bool canBeUnloaded() = 0;
};

std::unique_ptr<IRegionFile> createRegionFile(const std::string& fname);
