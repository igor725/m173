#pragma once

#include "../luaobject.h"
#include "../vm/lua.hpp"
#include "items/itemstack.h"

LuaObject* lua_pushitemstack(lua_State* L);

LuaObject* lua_pushitemstack(lua_State* L, ItemStack* is);

int luaopen_itemstack(lua_State* L);
