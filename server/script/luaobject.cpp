#include "luaobject.h"

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

LuaObject* LuaObject::fromstack(lua_State* L, int idx) {
  auto lobj = (LuaObject*)lua_touserdata(L, idx);
  if (lobj->m_fingerprint != 0xDEADBEEF) {
    luaL_error(L, "An attempt to cast %s to LuaObject was made", luaL_typename(L, idx));
    return nullptr;
  }
  (void)lobj->get<void>(); // Test for invalidation
  return lobj;
}
