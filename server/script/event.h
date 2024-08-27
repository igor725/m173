#pragma once

#include "entity/entitybase.h"
#include "items/itemstack.h"

#include <string>

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

struct onMessage {
  bool                cancelled;
  EntityBase*         sender;
  const std::wstring& message;
  std::wstring&       finalMessage;
};

struct ScriptEvent {
  enum Type {
    onStart,
    onStop,
    preBlockPlace,
    onBlockDestroyed,
    onMessage,
  };

  Type  type;
  void* args;
};
