#pragma once

#include "entitybase.h"
#include "helper.h"
#include "player/player.h"

#include <cstdint>
#include <functional>
#include <memory>

class IEntityManager {
  public:
  IEntityManager()          = default;
  virtual ~IEntityManager() = default;

  typedef std::function<bool(IPlayer* player)> PlayerIterCallback;

  virtual bool IterPlayers(PlayerIterCallback cb) = 0;

  virtual EntityId    AddEntity(std::unique_ptr<EntityBase>&& entity) = 0;
  virtual bool        RemoveEntity(EntityId id)                       = 0;
  virtual EntityBase* GetEntity(EntityId id)                          = 0;
};

IEntityManager& accessEntityManager();
