#pragma once

#include "entity/entitybase.h"
#include "items/itemstack.h"

struct preBlockPlaceArgumentEvent {
  bool              cancelled;
  ItemStack&        is;
  BlockId           id;
  EntityBase*       user;
  const IntVector3& pos;
  int8_t            direction;
};

struct onBlockDestroyedEvent {
  ItemStack&        is;
  EntityBase*       user;
  const IntVector3& pos;
  BlockId           id;
};

struct ScriptEvent {
  enum Type {
    onStart,
    onStop,
    preBlockPlace,
    onBlockDestroyed,
  };

  Type  type;
  void* args;
};
