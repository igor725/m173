#pragma once

#include "entitybase.h"
#include "helper.h"

#include <cstdint>
#include <memory>

class IEntityManager {
  public:
  IEntityManager()          = default;
  virtual ~IEntityManager() = default;

  virtual EntityId    AddEntity(std::unique_ptr<EntityBase>&& entity) = 0;
  virtual EntityBase* GetEntity(EntityId id)                          = 0;
};

IEntityManager& accessEntityManager();
