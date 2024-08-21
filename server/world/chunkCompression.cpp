#include "chunkCompression.h"

ChunkCompressor::ChunkCompressor(IZLibPP* compr, Chunk& chunk): m_chunk(chunk), m_compr(compr), m_state(0) {}

bool ChunkCompressor::feed() {
  if (m_compr->getAvailableInput() == 0) {
    switch (m_state++) {
      case 0: { // First things first, send the blocks array
        m_compr->setInput(m_chunk.m_blocks.data(), sizeof(m_chunk.m_blocks));
        return true;
      } break;
      case 1: { // Now the meta for blocks
        m_compr->setInput(m_chunk.m_meta.data(), sizeof(m_chunk.m_meta));
        return true;
      } break;
      case 2: { // Aaand block light array, whatever it means
        m_compr->setInput(m_chunk.m_light.data(), sizeof(m_chunk.m_light));
        return true;
      } break;
      case 3: { // This one I don't even know
        m_compr->setInput(m_chunk.m_sky.data(), sizeof(m_chunk.m_sky));
        return true;
      } break;
    }
  }

  return false;
}

ChunkDecompressor::ChunkDecompressor(IZLibPP* compr, Chunk& chunk): m_chunk(chunk), m_compr(compr), m_state(0) {}

bool ChunkDecompressor::feed() {
  if (m_compr->getAvailableOutput() == 0) {
    switch (m_state++) {
      case 0: { // First things first, send the blocks array
        m_compr->setOutput(m_chunk.m_blocks.data(), sizeof(m_chunk.m_blocks));
        return true;
      } break;
      case 1: { // Now the meta for blocks
        m_compr->setOutput(m_chunk.m_meta.data(), sizeof(m_chunk.m_meta));
        return true;
      } break;
      case 2: { // Aaand block light array, whatever it means
        m_compr->setOutput(m_chunk.m_light.data(), sizeof(m_chunk.m_light));
        return true;
      } break;
      case 3: { // This one I don't even know
        m_compr->setOutput(m_chunk.m_sky.data(), sizeof(m_chunk.m_sky));
        return true;
      } break;
    }
  }

  return false;
}
