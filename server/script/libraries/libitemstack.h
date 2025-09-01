#pragma once

#include "../luaobject.h"
#include "../vm/lua.hpp"
#include "items/itemstack.h"

LuaObject* lua_newitemstack(lua_State* L);

LuaObject* lua_pushitemstack(lua_State* L, ItemStack* is);

ItemStack* lua_checkitemstack(lua_State* L, int idx);

int luaopen_itemstack(lua_State* L);
