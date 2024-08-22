#include "chunkCompression.h"

ChunkZlib::ChunkZlib(IZLibPP* compr, const ChunkUnique& chunk, Type t): m_chunk(chunk), m_compr(compr), m_state(0), m_type(t) {}

bool ChunkZlib::feed() {
  void*  data   = nullptr;
  size_t datasz = 0;

  if ((m_type == Type::Compressor && m_compr->getAvailableInput() == 0) || (m_type == Type::Decompressor && m_compr->getAvailableOutput() == 0)) {
    switch (m_state++) {
      case 0: { // First things first, send the blocks array
        data = m_chunk->m_blocks.data(), datasz = sizeof(m_chunk->m_blocks);
      } break;
      case 1: { // Now the meta for blocks
        data = m_chunk->m_meta.data(), datasz = sizeof(m_chunk->m_meta);
      } break;
      case 2: { // Aaand block light array, whatever it means
        data = m_chunk->m_light.data(), datasz = sizeof(m_chunk->m_light);
      } break;
      case 3: { // This one I don't even know
        data = m_chunk->m_sky.data(), datasz = sizeof(m_chunk->m_sky);
      } break;
    }
  }

  if (data != nullptr && datasz > 0) {
    switch (m_type) {
      case Type::Compressor: {
        m_compr->setInput(data, datasz);
        return true;
      } break;
      case Type::Decompressor: {
        m_compr->setOutput(data, datasz);
        return true;
      } break;

      default: break;
    }
  }

  return false;
}
