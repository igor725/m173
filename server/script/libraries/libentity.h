#pragma once

#include "../luaobject.h"
#include "../vm/lua.hpp"
#include "entity/creaturebase.h"
#include "entity/entitybase.h"

LuaObject* lua_pushentity(lua_State* L, Entities::Base* ent);

Entities::Base* lua_checkentity(lua_State* L, int idx, Entities::Base::Type type);

CreatureBase* lua_checkcreature(lua_State* L, int idx);
CreatureBase* lua_checkcreature(lua_State* L, int idx, CreatureBase::Type type);

void lua_unlinkentity(lua_State* L, void* ptr);

int luaopen_entity(lua_State* L);
