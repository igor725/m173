#pragma once

#include "../luaobject.h"
#include "../vm/lua.hpp"
#include "entity/entitybase.h"

LuaObject* lua_pushentity(lua_State* L, EntityBase* ent);

void lua_unlinkentity(lua_State* L, void* ptr);

int luaopen_entity(lua_State* L);
