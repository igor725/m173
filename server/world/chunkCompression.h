#pragma once

#include "chunk.h"
#include "zlibpp/zlibpp.h"

class ChunkCompressor {
  public:
  ChunkCompressor(IZLibPP* compr, Chunk& chunk);

  bool feed();

  private:
  Chunk&   m_chunk;
  uint32_t m_state;
  IZLibPP* m_compr;
};

class ChunkDecompressor {
  public:
  ChunkDecompressor(IZLibPP* compr, Chunk& chunk);

  bool feed();

  private:
  Chunk&   m_chunk;
  uint32_t m_state;
  IZLibPP* m_compr;
};
