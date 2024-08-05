#include "manager.h"

#include <unordered_map>

class EntityManager: public IEntityManager {
  public:
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

  private:
  std::unordered_map<EntityId, std::unique_ptr<EntityBase>> m_loadedents;
};

IEntityManager& accessEntityManager() {
  static EntityManager inst;
  return inst;
}
