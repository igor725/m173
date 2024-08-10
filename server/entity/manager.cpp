#include "manager.h"

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

        DoEntityTicks(std::chrono::duration_cast<std::chrono::milliseconds>(curr - prev).count());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });
    em_thread.detach();
  }

  EntityId AddEntity(std::unique_ptr<EntityBase>&& entity) final {
    static EntityId entcounter = 0;

    auto p = m_loadedents.emplace(std::make_pair(++entcounter, std::move(entity)));
    p.first->second.get()->_setEntId(entcounter);

    return entcounter;
  }

  EntityBase* GetEntity(EntityId id) final {
    auto it = m_loadedents.find(id);
    if (it == m_loadedents.end()) return nullptr;
    return it->second.get();
  }

  bool RemoveEntity(EntityId id) final {
    auto it = m_loadedents.find(id);
    if (it == m_loadedents.end()) return false;
    m_loadedents.erase(it);
    return true;
  }

  bool IterPlayers(PlayerIterCallback cb) final {
    for (auto it = m_loadedents.begin(); it != m_loadedents.end(); ++it) {
      if (auto entity = it->second.get()) {
        if (entity->getType() == EntityBase::Type::Player && !cb(dynamic_cast<IPlayer*>(entity))) return false;
      }
    }

    return true;
  }

  bool IterEntities(EntityIterCallback cb) final {
    for (auto it = m_loadedents.begin(); it != m_loadedents.end(); ++it) {
      if (!cb(it->second.get())) return false;
    }

    return true;
  }

  void DoEntityTicks(int64_t delta) {
    for (auto it = m_loadedents.begin(); it != m_loadedents.end(); ++it) {
      it->second.get()->tick(delta);
    }
  }

  private:
  std::unordered_map<EntityId, std::unique_ptr<EntityBase>> m_loadedents;
};

IEntityManager& accessEntityManager() {
  static EntityManager inst;
  return inst;
}
