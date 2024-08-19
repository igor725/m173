#include "manager.h"

#include "platform/platform.h"
#include "runmanager/runmanager.h"

#include <exception>
#include <mutex>
#include <thread>
#include <unordered_map>

class EntityCounterOverflowException: public std::exception {
  public:
  EntityCounterOverflowException() {}

  const char* what() const noexcept override { return "EntityId counter got overflowed"; }
};

class EntityManager: public IEntityManager {
  public:
  EntityManager() {
    m_tickThread = std::thread([this]() {
      Platform::SetCurrentThreadName("Entity ticker");

      auto curr = std::chrono::system_clock::now();
      auto prev = std::chrono::system_clock::now();

      while (RunManager::isRunning()) {
        prev = curr;
        curr = std::chrono::system_clock::now();

        DoEntityTicks(std::chrono::duration_cast<std::chrono::milliseconds>(curr - prev).count() / 1000.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }

      WaitPlayerThreads();
    });
  }

  EntityBase* AddEntity(std::unique_ptr<EntityBase>&& entity) final {
    std::unique_lock lock(m_lock);

    static EntityId entcounter = 0;
    if (entcounter == UINT_MAX) throw EntityCounterOverflowException();

    auto p    = m_loadedents.emplace(std::make_pair(++entcounter, std::move(entity)));
    auto eptr = p.first->second.get();
    eptr->_setEntId(entcounter);

    if (eptr->getType() != EntityBase::Player) {
      IterPlayers([eptr](IPlayer* ply) -> bool {
        ply->addTrackedEntity(eptr);
        return true;
      });
    }

    eptr->onSpawned();

    return eptr;
  }

  EntityBase* GetEntity(EntityId id) final {
    std::unique_lock lock(m_lock);

    auto it = m_loadedents.find(id);
    if (it == m_loadedents.end()) return nullptr;
    return it->second.get();
  }

  bool RemoveEntity(EntityId id) final {
    std::unique_lock lock(m_lock);

    auto it = m_loadedents.find(id);
    if (it == m_loadedents.end()) return false;
    m_loadedents.erase(it);
    return true;
  }

  bool IterPlayers(PlayerIterCallback cb) final {
    std::unique_lock lock(m_lock);

    for (auto it = m_loadedents.begin(); it != m_loadedents.end(); ++it) {
      if (auto entity = it->second.get()) {
        if (entity->getType() == EntityBase::Type::Player && !cb(dynamic_cast<IPlayer*>(entity))) return false;
      }
    }

    return true;
  }

  bool IterEntities(EntityIterCallback cb) final {
    std::unique_lock lock(m_lock);

    for (auto it = m_loadedents.begin(); it != m_loadedents.end(); ++it) {
      if (!cb(it->second.get())) return false;
    }

    return true;
  }

  void DoEntityTicks(double_t delta) {
    {
      std::unique_lock lock(m_lock);

      for (auto it = m_loadedents.begin(); it != m_loadedents.end();) {
        auto ent = it->second.get();
        ent->tick(delta);

        if (ent->isMarkedForDestruction()) {
          it = m_loadedents.erase(it);
          continue;
        }

        ++it;
      }
    }

    {
      std::unique_lock lock(m_ptLock);
      for (auto it = m_playerThreads.begin(); it != m_playerThreads.end();) {
        if (it->second.canBeDestroyed) {
          auto& thread = it->second.thread;
          if (thread.joinable()) thread.join();
          it = m_playerThreads.erase(it);
          continue;
        }

        ++it;
      }
    }
  }

  void AddPlayerThread(std::thread&& thread, uint64_t ref) final {
    std::unique_lock lock(m_ptLock);
    m_playerThreads.emplace(std::make_pair(ref, std::move(thread)));
  }

  void RemovePlayerThread(uint64_t ref) final {
    if (!RunManager::isRunning()) return;
    std::unique_lock lock(m_ptLock);

    for (auto it = m_playerThreads.begin(); it != m_playerThreads.end(); ++it) {
      if (it->first == ref) {
        it->second.canBeDestroyed = true;
        return;
      }
    }
  }

  void WaitPlayerThreads() {
    std::unique_lock lock(m_ptLock);
    for (auto it = m_playerThreads.begin(); it != m_playerThreads.end();) {
      if (it->second.thread.joinable()) it->second.thread.join();
      it = m_playerThreads.erase(it);
    }
  }

  void finish() final { m_tickThread.join(); }

  private:
  struct PlayerThread {
    std::thread thread;
    bool        canBeDestroyed;
  };

  std::thread          m_tickThread;
  std::recursive_mutex m_lock;

  std::unordered_map<EntityId, std::unique_ptr<EntityBase>> m_loadedents;
  std::recursive_mutex                                      m_ptLock;
  std::unordered_map<uint64_t, PlayerThread>                m_playerThreads;
};

IEntityManager& accessEntityManager() {
  static EntityManager inst;
  return inst;
}
