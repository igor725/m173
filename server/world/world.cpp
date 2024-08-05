#include "world.h"

#include "zlibpp/zlibpp.h"

#include <unordered_map>
#include <utility>

class World: public IWorld {
  public:
  World() { m_ldChunks.emplace(std::make_pair(0, Chunk())); }

  virtual ~World() = default;

  Chunk* getChunk(int32_t x, int32_t z) final {
    auto it = m_ldChunks.find((int64_t)x << 32 | z);
    if (it == m_ldChunks.end()) return nullptr;
    return &it->second;
  }

  const void* compressChunk(Chunk* chunk, unsigned long& size) final {
    auto compr = createCompressor();

    int32_t       index = 0;
    unsigned long lastOut;

    compr->setOutput(m_tempchunk.data(), m_tempchunk.size());
    do {
      if (compr->getAvailableInput() == 0) {
        switch (index++) {
          case 0: { // First things first, send blocks
            compr->setInput(chunk->m_blocks.data(), sizeof(chunk->m_blocks));
          } break;
          case 1: { // Now the meta for blocks
            compr->setInput(chunk->m_meta.data(), sizeof(chunk->m_meta));
          } break;
          case 2: { // Aaand block light array, whatever it means
            compr->setInput(chunk->m_light.data(), sizeof(chunk->m_light));
          } break;
          case 3: { // This one I don't even know
            compr->setInput(chunk->m_sky.data(), sizeof(chunk->m_sky));
          } break;
        }
      }
    } while (!compr->tick());

    size = compr->getTotalOutput();
    return m_tempchunk.data();
  }

  private:
  std::array<uint8_t, CHUNK_COMPR_SIZE> m_tempchunk;
  std::unordered_map<int64_t, Chunk>    m_ldChunks;
  int64_t                               m_seed = 0;
};

IWorld& accessWorld() {
  static World inst;
  return inst;
}
