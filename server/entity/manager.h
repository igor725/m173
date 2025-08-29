#pragma once

#include "creatures/player.h"
#include "entitybase.h"
#include "entry/helper.h"

#include <functional>
#include <memory>
#include <thread>

namespace Entities {
class IManager {
  public:
  IManager()          = default;
  virtual ~IManager() = default;

  typedef std::function<bool(Entities::PlayerBase* player)> PlayerIterCallback;
  typedef std::function<bool(Entities::Base* ent)>          EntityIterCallback;

  virtual bool                  IterPlayers(PlayerIterCallback cb)        = 0;
  virtual bool                  IterEntities(EntityIterCallback cb)       = 0;
  virtual Entities::PlayerBase* getPlayerByName(const std::wstring& name) = 0;

  virtual void     AddPlayerThread(std::thread&& thread, uint64_t ref) = 0;
  virtual void     RemovePlayerThread(uint64_t ref)                    = 0;
  virtual uint32_t GetPlayersCount() const                             = 0;

  virtual Entities::Base* AddEntity(std::unique_ptr<Entities::Base>&& entity) = 0;
  virtual bool            RemoveEntity(EntityId id)                           = 0;
  virtual Entities::Base* GetEntity(EntityId id)                              = 0;

  virtual void finish() = 0;
};

namespace Access {
IManager& manager();
}

} // namespace Entities
