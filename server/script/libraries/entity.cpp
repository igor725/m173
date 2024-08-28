#include "entity.h"

class EntityScript {
  public:
  EntityScript(EntityBase* ent): m_ent(ent) {}

  // todo getName method

  static LuaObject* get(lua_State* L, EntityBase* ent) {
    if (m_entTabRef > 0) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, m_entTabRef);
      lua_pushlightuserdata(L, ent);
      lua_rawget(L, -2);
      if (lua_isuserdata(L, -1)) {
        lua_remove(L, -2);
        return (LuaObject*)lua_touserdata(L, -1);
      }

      lua_pop(L, 2);
    } else {
      lua_newtable(L);
      m_entTabRef = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    auto lobj = LuaObject::create(L, sizeof(EntityScript));

    switch (auto t = ent->getType()) {
      case EntityBase::Player: {
        luaL_setmetatable(L, "EntityPlayer");
      } break;

      default: {
        luaL_error(L, "Unknown entity type: %d", t);
      } break;
    }

    new (lobj->get<EntityScript>()) EntityScript(ent);

    lua_rawgeti(L, LUA_REGISTRYINDEX, m_entTabRef);
    lua_pushlightuserdata(L, ent);
    lua_pushvalue(L, -3);
    lua_rawset(L, -3);
    lua_pop(L, 1);

    return lobj;
  }

  static void unlink(lua_State* L, void* ptr) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, m_entTabRef);
    lua_pushlightuserdata(L, ptr);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);
  }

  private:
  static int  m_entTabRef;
  EntityBase* m_ent;
};

int EntityScript::m_entTabRef = -1;

LuaObject* lua_pushentity(lua_State* L, EntityBase* ent) {
  return EntityScript::get(L, ent);
}

void lua_unlinkentity(lua_State* L, void* ptr) {
  EntityScript::unlink(L, ptr);
}

int luaopen_entity(lua_State* L) {
  const luaL_Reg entitybase_reg[] = {
      {nullptr, nullptr},
  };
  luaL_newmetatable(L, "EntityBase");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, entitybase_reg, 0);

  const luaL_Reg entityplayer_reg[] = {
      {nullptr, nullptr},
  };
  luaL_newmetatable(L, "EntityPlayer");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setmetatable(L, "EntityBase");
  luaL_setfuncs(L, entityplayer_reg, 0);

  lua_pop(L, 2);
  return 0;
}
