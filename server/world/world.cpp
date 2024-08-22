#include "world.h"

#include "config/config.h"
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

        advanceTick(std::chrono::duration_cast<std::chrono::milliseconds>(curr - prev).count() / 1000.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }

      auto& rm = accessRegionManager();

      for (auto it = m_ldChunks.begin(); it != m_ldChunks.end();) {
        rm.saveChunk(it->first, ChunkUnique(it->second));
        it = m_ldChunks.erase(it);
      }
    });

    m_generator->getSpawnPoint(m_spawnPoint);

    auto& cfg = accessConfig();

    m_unloadInterval = cfg.getItem("chunk.unload_interval").getValue<uint32_t>();
    m_nextSave = m_saveInterval = cfg.getItem("world.save_interval").getValue<uint32_t>();
  }

  virtual ~World() = default;

  ChunkUnique openChunk(const IntVector2& pos) {
    std::unique_lock lock(m_accChunks);

    auto&& mapPlace = m_ldChunks.emplace(std::make_pair(pos, std::make_unique<Chunk>(m_unloadInterval)));
    auto   chunk    = ChunkUnique(mapPlace.first->second);

    auto& rm = accessRegionManager();

    if (!rm.loadChunk(pos, chunk)) {
      m_generator->fillChunk(pos, chunk);
      chunk->m_wasUpdated = !rm.saveChunk(pos, chunk);
    }

    return chunk;
  }

  ChunkUnique getChunk(const IntVector2& pos) final {
    std::unique_lock lock(m_accChunks);

    auto it = m_ldChunks.find(pos);
    if (it == m_ldChunks.end()) return openChunk(pos);
    return ChunkUnique(it->second);
  }

  bool canBlockBePlacedHere(const IntVector3& pos, BlockId id) {
    // todo AABB collision
    return true;
  }

  bool setBlock(const IntVector3& pos, BlockId id, int8_t meta) final {
    if (pos.y < 0 || pos.y > CHUNK_DIMS.y) return false;
    auto chunk = getChunk({pos.x >> 4, pos.z >> 4});
    if (!canBlockBePlacedHere(pos, id)) return false;
    chunk->m_blocks[chunk->getBlockOffset(pos)] = id;
    chunk->m_meta.setNibble(chunk->getBlockOffset(chunk->toLocalChunkCoords(pos)), meta);
    chunk->m_wasUpdated |= true;
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
    auto chunk = getChunk({pos.x >> 4, pos.z >> 4});
    if (meta != nullptr) *meta = chunk->m_meta.getNibble(chunk->getBlockOffset(chunk->toLocalChunkCoords(pos)));
    return chunk->m_blocks[chunk->getBlockOffset(pos)];
  }

  void tickChunks(double_t delta) {
    std::unique_lock lock(m_accChunks);

    auto& rm = accessRegionManager();

    bool shouldSave = false;
    if ((m_nextSave -= delta) <= 0.0) {
      m_nextSave = m_saveInterval;
      shouldSave = true;
    }

    for (auto it = m_ldChunks.begin(); it != m_ldChunks.end();) {
      auto& pos   = it->first;
      auto& chunk = it->second;
      chunk->m_lock.lock();

      if (chunk->m_uses <= 0) {
        chunk->m_uses = 0; // less than zero should not happen in practice, but just in case
        if ((chunk->m_unloadTimer -= delta) <= 0.0) {
          // No players in this chunk, so we can safely destroy it
          rm.saveChunk(pos, chunk);
          chunk->m_lock.unlock();
          it = m_ldChunks.erase(it);
          continue;
        }
      } else {
        chunk->m_unloadTimer = m_unloadInterval;
        if (shouldSave && chunk->m_wasUpdated) {
          chunk->m_wasUpdated = !rm.saveChunk(pos, chunk);
        }
      }

      chunk->m_lock.unlock();
      ++it;
    }
  }

  void advanceTick(double_t delta) {
    if ((m_witime += delta) >= 1.0) {
      tickChunks(m_witime); // todo should I move this to another thread?
      m_wtime += 20.0 * std::floor(m_witime);
      m_witime -= 1.0;
    }
    // todo physics

    if (m_witime >= 1.0) advanceTick(0);
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
  vec2_map<std::unique_ptr<Chunk>> m_ldChunks;
  std::recursive_mutex             m_accChunks;
  std::thread                      m_tickThread;

  std::unique_ptr<IGenerator> m_generator;

  IntVector3 m_spawnPoint;
  int64_t    m_wtime  = 0;
  double_t   m_witime = 0;

  double_t m_unloadInterval = 500;
  double_t m_saveInterval   = 500;
  double_t m_nextSave       = 500;
};

IWorld& accessWorld() {
  static World inst(0);
  return inst;
}
