#pragma once

#include "../luaobject.h"
#include "../vm/lua.hpp"
#include "helper.h"

LuaObject* lua_pushvector(lua_State* L, ByteVector3* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, IntVector2* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, IntVector3* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, DoubleVector3* vec, bool constant = false);
LuaObject* lua_pushangle(lua_State* L, FloatAngle* ang, bool constant = false);

template <typename T>
LuaObject* lua_pushvector(lua_State* L, const T* vec) {
  return lua_pushvector(L, const_cast<T*>(vec), true);
}

LuaObject* lua_checkvector(lua_State* L, int idx);

int luaopen_vector(lua_State* L);
