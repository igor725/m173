#pragma once

#include "vm/lua.hpp"

#include <cstdint>
#include <vector>

class LuaObject {
  static const uint64_t FINGERPRINT = 0xB00B1EC0DE501337;

  public:
  LuaObject(lua_State* L): m_invalidated(false) {}

  template <typename T>
  T* get(lua_State* L) {
    if (m_invalidated) {
      luaL_error(L, "An attempt to access invalidated LuaObject{%p} was made!", this);
      return nullptr;
    }

    return (T*)&this[1];
  }

  void invalidate();

  void link(LuaObject* lobj);

  static LuaObject* create(lua_State* L, size_t sz);

  static LuaObject* fromstack(lua_State* L, int idx);

  static LuaObject* fromstack(lua_State* L, int idx, const char* meta);

  private:
  uint64_t                m_fingerprint = FINGERPRINT;
  bool                    m_invalidated;
  std::vector<LuaObject*> m_linkedObjs;
};
