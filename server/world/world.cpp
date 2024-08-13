#include "world.h"

#include "items/item.h"
#include "network/packets/World.h"

#include <mutex>
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

    m_spawnPoint = {5, 16, 5};
  }

  virtual ~World() = default;

  Chunk* allocChunk(const IntVector2& pos) {
    std::unique_lock lock(m_accChunks);

    auto&& chunk = m_ldChunks.emplace(std::make_pair(packChunkPos(pos), Chunk()));
    return &chunk.first->second;
  }

  Chunk* genChunk(const IntVector2& pos) {
    std::unique_lock lock(m_accChunks);

    auto chunk = allocChunk(pos);
    chunk->m_light.fill(Nible(15, 15)); // All fullbright for now
    chunk->m_sky.fill(Nible(15, 15));

    for (int32_t x = 0; x < 16; ++x) {
      for (int32_t y = 0; y < (m_spawnPoint.y - 2); ++y) {
        for (int32_t z = 0; z < 16; ++z) {
          chunk->m_blocks[chunk->getLocalIndex({x, y, z})] = y < 1 ? 7 : y < (m_spawnPoint.y - 3) ? 3 : 2;
        }
      }
    }

    return chunk;
  }

  Chunk* getChunk(const IntVector2& pos) final {
    std::unique_lock lock(m_accChunks);

    auto it = m_ldChunks.find(packChunkPos(pos));
    if (it == m_ldChunks.end()) return genChunk(pos);
    return &it->second;
  }

  bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) final {
    if (pos.y < 0 || pos.y > CHUNK_DIMS.y) return false;
    auto chunk = getChunk({pos.x >> 4, pos.z >> 4});
    if (chunk == nullptr) return false;
    chunk->m_blocks[chunk->getWorldIndex(pos)] = id;
    chunk->m_meta.setNible(chunk->toLocalChunkCoords(pos), meta);
    return true;
  }

  bool setBlockWithNotify(const IntVector3& pos, BlockId id, int8_t meta, IPlayer* placer) final {
    if (setBlock(pos, id, meta)) {
      Packet::ToClient::BlockChange wdata(pos, id, meta);
      placer->sendToTrackedPlayers(wdata, true);
      return true;
    }

    auto bid = getBlock(pos, &meta);

    Packet::ToClient::BlockChange wdata(pos, bid, meta);
    wdata.sendTo(placer->getSocket());
    placer->resendItem(placer->getHeldItem());
    return false;
  }

  BlockId getBlock(const IntVector3& pos, int8_t* meta = nullptr) final {
    if (pos.y < 0 || pos.y > CHUNK_DIMS.y) return false;
    auto chunk = getChunk({pos.x >> 4, pos.z >> 4});
    if (chunk == nullptr) return 0;
    if (meta != nullptr) *meta = chunk->m_meta.getNible(chunk->toLocalChunkCoords(pos));
    return chunk->m_blocks[chunk->getWorldIndex(pos)];
  }

  void advanceTick(int64_t delta) final {
    if ((m_witime += delta) > 1000) {
      m_wtime += 20;
      m_witime = 0;
    }
    // todo physics
  }

  int64_t getTime() const final { return m_wtime; }

  int64_t getSeed() const final { return m_seed; }

  const IntVector3& getSpawnPoint() const final { return m_spawnPoint; }

  private:
  std::unordered_map<int64_t, Chunk> m_ldChunks;
  std::recursive_mutex               m_accChunks;

  IntVector3 m_spawnPoint;
  int64_t    m_seed   = 0;
  int64_t    m_wtime  = 0;
  int64_t    m_witime = 0;
};

IWorld& accessWorld() {
  static World inst;
  return inst;
}
