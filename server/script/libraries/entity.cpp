#include "entity.h"

#include "entity/creatures/player.h"
#include "entity/manager.h"

#include <string>

class EntityScript {
  public:
  EntityScript(EntityBase* ent): m_ent(ent) {}

  EntityBase* entity() const { return m_ent; }

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
  if (ent == nullptr) {
    lua_pushnil(L);
    return nullptr;
  }

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
      {"chat",
       [](lua_State* L) -> int {
         auto msg  = std::string_view(luaL_checkstring(L, 2));
         auto lobj = LuaObject::fromstack(L, 1);
         auto sent = lobj->get<EntityScript>()->entity();

         if (sent->getType() != EntityBase::Player) {
           luaL_error(L, ":chat() called on non-player? Huh??");
           return 0;
         }

         std::wstring wtext;
         std::mbtowc(nullptr, nullptr, 0);
         for (auto it = msg.begin(); it != msg.end(); ++it) {
           wchar_t ch;
           if (std::mbtowc(&ch, &(*it), 1) > 0) wtext.push_back(ch);
         }

         dynamic_cast<IPlayer*>(sent)->sendChat(std::wstring_view(wtext.begin(), wtext.end()));

         return 0;
       }},

      {nullptr, nullptr},
  };
  luaL_newmetatable(L, "EntityPlayer");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setmetatable(L, "EntityBase");
  luaL_setfuncs(L, entityplayer_reg, 0);

  lua_pop(L, 2);

  const luaL_Reg entity_lib[] = {
      {"player",
       [](lua_State* L) -> int {
         auto aname = std::string_view(luaL_checkstring(L, 1));

         std::wstring name;
         name.reserve(aname.length());
         std::mbtowc(nullptr, nullptr, 0);
         for (auto it = aname.begin(); it != aname.end(); ++it) {
           wchar_t ch;
           if (std::mbtowc(&ch, &(*it), 1) > 0) name.push_back(ch);
         }

         lua_pushentity(L, accessEntityManager().getPlayerByName(name));
         return 1;
       }},

      {nullptr, nullptr},
  };

  lua_newtable(L);
  luaL_setfuncs(L, entity_lib, 0);
  return 1;
}
