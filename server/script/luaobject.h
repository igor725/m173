#pragma once

#include "vm/lua.hpp"

#include <cstdint>
#include <vector>

class LuaObject {
  static const uint64_t FINGERPRINT = 0xB00B1EC0DE501337;

  public:
  LuaObject(lua_State* L): m_state(L), m_invalidated(false) {}

  template <typename T>
  T* get() {
    if (m_invalidated) {
      luaL_error(m_state, "An attempt to access invalidated LuaObject{%p} was made!", this);
      return nullptr;
    }

    return (T*)&this[1];
  }

  void invalidate();

  void link(LuaObject* lobj);

  static LuaObject* create(lua_State* L, size_t sz);

  static LuaObject* fromstack(lua_State* L, int idx);

  private:
  uint64_t                m_fingerprint = FINGERPRINT;
  bool                    m_invalidated;
  lua_State*              m_state;
  std::vector<LuaObject*> m_linkedObjs;
};
