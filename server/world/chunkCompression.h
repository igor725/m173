#pragma once

#include "chunk.h"
#include "zlibpp/zlibpp.h"

class ChunkZlib {
  public:
  enum Type {
    Unspecified,
    Compressor,
    Decompressor,
  };

  ChunkZlib(IZLibPP* compr, const ChunkUnique& chunk, Type t);

  bool feed();

  private:
  const ChunkUnique& m_chunk;
  uint32_t           m_state;
  IZLibPP*           m_compr;
  const Type         m_type;
};
