#pragma once

#include "../luaobject.h"
#include "../vm/lua.hpp"
#include "helper.h"

LuaObject* lua_pushvector(lua_State* L, ByteVector3* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, IntVector2* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, IntVector3* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, DoubleVector3* vec, bool constant = false);
LuaObject* lua_pushangle(lua_State* L, FloatAngle* ang, bool constant = false);

LuaObject* lua_newvector(lua_State* L, const ByteVector3& vec, bool constant);
LuaObject* lua_newvector(lua_State* L, const IntVector2& vec, bool constant);
LuaObject* lua_newvector(lua_State* L, const IntVector3& vec, bool constant);
LuaObject* lua_newvector(lua_State* L, const DoubleVector3& vec, bool constant);
LuaObject* lua_newangle(lua_State* L, const FloatAngle& ang, bool constant);

template <typename T>
LuaObject* lua_pushvector(lua_State* L, const T* vec) {
  return lua_pushvector(L, const_cast<T*>(vec), true);
}

LuaObject* lua_checkvector(lua_State* L, int idx);

int luaopen_vector(lua_State* L);
