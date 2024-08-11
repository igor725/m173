#include "manager.h"

#include <mutex>
#include <unordered_map>

class EntityManager: public IEntityManager {
  public:
  EntityManager() {
    std::thread em_thread([this]() {
      auto curr = std::chrono::system_clock::now();
      auto prev = std::chrono::system_clock::now();

      while (true) {
        prev = curr;
        curr = std::chrono::system_clock::now();

        DoEntityTicks(std::chrono::duration_cast<std::chrono::milliseconds>(curr - prev).count() / 1000.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
    em_thread.detach();
  }

  EntityId AddEntity(std::unique_ptr<EntityBase>&& entity) final {
    std::unique_lock lock(m_lock);

    static EntityId entcounter = 0;

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

    return entcounter;
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

  private:
  std::recursive_mutex                                      m_lock;
  std::unordered_map<EntityId, std::unique_ptr<EntityBase>> m_loadedents;
};

IEntityManager& accessEntityManager() {
  static EntityManager inst;
  return inst;
}
