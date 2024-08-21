#include "world.h"

#include "entity/manager.h"
#include "generators/glist.h"
#include "items/item.h"
#include "mcregion/mcregion.h"
#include "network/packets/World.h"
#include "platform/platform.h"
#include "runmanager/runmanager.h"

#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>

class World: public IWorld {
  template <typename T>
  using vec2_map = std::unordered_map<IntVector2, T, IntVector2::HashFunction>;

  public:
  World(int64_t seed): m_generator(createFlat(seed)) {
    m_tickThread = std::thread([this]() {
      Platform::SetCurrentThreadName("World ticker");

      auto curr = std::chrono::system_clock::now();
      auto prev = std::chrono::system_clock::now();

      while (RunManager::isRunning()) {
        prev = curr;
        curr = std::chrono::system_clock::now();

        advanceTick(std::chrono::duration_cast<std::chrono::milliseconds>(curr - prev).count());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }

      auto& rm = accessRegionManager();

      for (auto it = m_ldChunks.begin(); it != m_ldChunks.end();) {
        rm.saveChunk(it->first, it->second);
        it = m_ldChunks.erase(it);
      }
    });

    m_generator->getSpawnPoint(m_spawnPoint);
  }

  virtual ~World() = default;

  Chunk& allocChunk(const IntVector2& pos) final {
    std::unique_lock lock(m_accChunks);

    auto&& chunk = m_ldChunks.emplace(std::make_pair(pos, Chunk()));
    return chunk.first->second;
  }

  Chunk& openChunk(const IntVector2& pos) {
    std::unique_lock lock(m_accChunks);

    auto& chunk = allocChunk(pos);

    auto& rm = accessRegionManager();

    if (!rm.loadChunk(pos, chunk)) {
      m_generator->fillChunk(pos, chunk);
      rm.saveChunk(pos, chunk);
    }

    return chunk;
  }

  Chunk& getChunk(const IntVector2& pos) final {
    std::unique_lock lock(m_accChunks);

    auto it = m_ldChunks.find(pos);
    if (it == m_ldChunks.end()) return openChunk(pos);
    return it->second;
  }

  bool canBlockBePlacedHere(const IntVector3& pos, BlockId id) {
    // todo AABB collision
    return true;
  }

  bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) final {
    if (pos.y < 0 || pos.y > CHUNK_DIMS.y) return false;
    auto& chunk = getChunk({pos.x >> 4, pos.z >> 4});
    if (!canBlockBePlacedHere(pos, id)) return false;
    chunk.m_blocks[chunk.getBlockOffset(pos)] = id;
    chunk.m_meta.setNibble(chunk.getBlockOffset(chunk.toLocalChunkCoords(pos)), meta);
    return true;
  }

  bool setBlockWithNotify(const IntVector3& pos, BlockId id, int8_t meta, IPlayer* placer) final {
    if (setBlock(pos, id, meta)) {
      Packet::ToClient::BlockChange wdata(pos, id, meta);
      placer->sendToTrackedPlayers(wdata, true);
      return true;
    }

    BlockId block = getBlock(pos, &meta);

    Packet::ToClient::BlockChange wdata(pos, block, meta);
    wdata.sendTo(placer->getSocket());
    return false;
  }

  BlockId getBlock(const IntVector3& pos, int8_t* meta = nullptr) final {
    if (pos.y < 0 || pos.y > CHUNK_DIMS.y) return false;
    auto& chunk = getChunk({pos.x >> 4, pos.z >> 4});
    if (meta != nullptr) *meta = chunk.m_meta.getNibble(chunk.getBlockOffset(chunk.toLocalChunkCoords(pos)));
    return chunk.m_blocks[chunk.getBlockOffset(pos)];
  }

  void freeUnusedChunks(int64_t delta) {
    std::unique_lock lock(m_accChunks);

    auto& em = accessEntityManager();
    auto& rm = accessRegionManager();

    for (auto it = m_ldChunks.begin(); it != m_ldChunks.end();) {
      auto& pos   = it->first;
      auto& chunk = it->second;

      lock.unlock();
      if (em.IterPlayers([&pos](IPlayer* ply) -> bool { return !ply->isHoldingChunk(pos); })) {
        lock.lock();
        if ((chunk.unloadTimer -= delta) <= 0) {
          // No players in this chunk, so we can safely destroy it
          rm.saveChunk(pos, chunk);
          it = m_ldChunks.erase(it);
          continue;
        }
      } else {
        lock.lock();
        chunk.unloadTimer = CHUNK_UNLOAD_TIMER_INIT;
      }

      ++it;
    }
  }

  void advanceTick(int64_t delta) final {
    if ((m_witime += delta) > 1000) {
      freeUnusedChunks(m_witime);
      m_wtime += 20;
      m_witime = 0;
    }
    // todo physics
  }

  int64_t getTime() const final { return m_wtime; }

  int64_t getSeed() const final { return m_generator->getSeed(); }

  size_t getChunksCount() final {
    std::unique_lock lock(m_accChunks);
    return m_ldChunks.size();
  }

  const IntVector3& getSpawnPoint() const final { return m_spawnPoint; }

  void finish() final { m_tickThread.join(); }

  private:
  vec2_map<Chunk>      m_ldChunks;
  std::recursive_mutex m_accChunks;
  std::thread          m_tickThread;

  std::unique_ptr<IGenerator> m_generator;

  IntVector3 m_spawnPoint;
  int64_t    m_wtime  = 0;
  int64_t    m_witime = 0;
};

IWorld& accessWorld() {
  static World inst(0);
  return inst;
}
