#include "world.h"

#include "zlibpp/zlibpp.h"

#include <thread>
#include <unordered_map>
#include <utility>

class World: public IWorld {
  public:
  World() {
    std::thread worldtick([]() {
      auto& world = accessWorld();

      auto curr = std::chrono::system_clock::now();
      auto prev = std::chrono::system_clock::now();

      while (true) {
        prev = curr;
        curr = std::chrono::system_clock::now();

        world.advanceTick(std::chrono::duration_cast<std::chrono::milliseconds>(curr - prev).count());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
    worldtick.detach();

    m_spawnPoint = {5, 10, 5};
  }

  virtual ~World() = default;

  Chunk* allocChunk(const IntVector2& pos) {
    auto&& chunk = m_ldChunks.emplace(std::make_pair(0, Chunk()));
    return &chunk.first->second;
  }

  Chunk* getChunk(const IntVector2& pos) final {
    auto it = m_ldChunks.find(packChunkPos(pos));
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

  bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) final {
    auto chunk = getChunk({pos.x >> 4, pos.z >> 4});
    if (chunk == nullptr) return false;
    chunk->m_blocks[chunk->getWorldIndex(pos)] = id;
    // todo save meta too
    return true;
  }

  void advanceTick(int64_t delta) final {
    if ((m_witime += delta) > 1000) {
      m_wtime += 20;
      m_witime = 0;
    }
    // todo physics
  }

  int64_t getTime() const final { return m_wtime; }

  const IntVector3& getSpawnPoint() const final { return m_spawnPoint; }

  private:
  IntVector3                            m_spawnPoint;
  std::array<uint8_t, CHUNK_COMPR_SIZE> m_tempchunk;
  std::unordered_map<int64_t, Chunk>    m_ldChunks;
  int64_t                               m_seed   = 0;
  int64_t                               m_wtime  = 0;
  int64_t                               m_witime = 0;
};

IWorld& accessWorld() {
  static World inst;
  return inst;
}
