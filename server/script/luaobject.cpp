#include "luaobject.h"

#include <algorithm>

void LuaObject::invalidate() {
  m_invalidated = true;
  for (auto it = m_linkedObjs.begin(); it != m_linkedObjs.end(); ++it) {
    (*it)->invalidate();
  }
}

void LuaObject::link(LuaObject* lobj) {
  if (this == lobj) return;
  if (std::find(m_linkedObjs.begin(), m_linkedObjs.end(), lobj) != m_linkedObjs.end()) return;
  m_linkedObjs.push_back(lobj);
}

LuaObject* LuaObject::create(lua_State* L, size_t sz) {
  auto lo = (LuaObject*)lua_newuserdata(L, sizeof(LuaObject) + sz);
  return new (lo) LuaObject(L);
}

static inline void casterror(lua_State* L, int idx) {
  luaL_error(L, "An attempt to cast %s to LuaObject was made", luaL_typename(L, idx));
}

LuaObject* LuaObject::fromstack(lua_State* L, int idx) {
  if (lua_getmetatable(L, idx) == 0) {
    if (!lua_isnil(L, -1)) casterror(L, idx);
    return nullptr;
  } else {
    lua_pop(L, 1);
  }

  auto lobj = (LuaObject*)lua_touserdata(L, idx);
  if (lobj == nullptr || lobj->m_fingerprint != FINGERPRINT) { // Not the safest way actually, but it should not crash in normal conditions
    casterror(L, idx);
    return nullptr;
  }
  (void)lobj->get<void>(L); // Test for invalidation
  return lobj;
}

LuaObject* LuaObject::fromstack(lua_State* L, int idx, const char* meta) {
  auto lobj = (LuaObject*)luaL_checkudata(L, idx, meta);
  if (lobj == nullptr || lobj->m_fingerprint != FINGERPRINT) {
    casterror(L, idx);
    return nullptr;
  }
  (void)lobj->get<void>(L); // Test for invalidation
  return lobj;
}
