#include "event.h"

#include "libraries/entity.h"
#include "libraries/itemstack.h"
#include "vm/lua.hpp"

#include <cstdlib>
#include <format>

static void createEventArg(lua_State* L, const char* name, const luaL_Reg* reg) {
  luaL_newmetatable(L, name);
  lua_pushliteral(L, "__tostring");
  lua_pushcfunction(L, [](lua_State* L) -> int {
    lua_pushstring(L, std::format("EventArguments: {}", (void*)LuaObject::fromstack(L, 1)).c_str());
    return 1;
  });
  lua_rawset(L, -3);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);
  luaL_setfuncs(L, reg, 0);
}

static int pushVector(lua_State* L, const IntVector3& vec) {
  lua_pushinteger(L, vec.x);
  lua_pushinteger(L, vec.y);
  lua_pushinteger(L, vec.z);
  return 3;
}

static void regBlockEvents(lua_State* L) {
  static luaL_Reg reg_place[] = {
      {"cancel",
       [](lua_State* L) -> int {
         auto lobj      = LuaObject::fromstack(L, 1);
         auto arg       = *lobj->get<preBlockPlaceArgumentEvent*>(L);
         arg->cancelled = true;
         return 0;
       }},
      {"isCancelled",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>(L);
         lua_pushboolean(L, arg->cancelled);
         return 1;
       }},
      {"position",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>(L);
         return pushVector(L, arg->pos);
       }},
      {"direction",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>(L);
         lua_pushinteger(L, arg->direction);
         return 1;
       }},
      {"placer",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>(L);
         lua_pushentity(L, arg->user);
         return 1;
       }},
      {"itemStack",
       [](lua_State* L) -> int {
         auto lobj    = LuaObject::fromstack(L, 1);
         auto arg     = *lobj->get<preBlockPlaceArgumentEvent*>(L);
         auto is_lobj = lua_pushitemstack(L, &arg->is);
         lobj->link(is_lobj);
         return 1;
       }},

      {nullptr, nullptr},
  };
  createEventArg(L, "preBlockPlaceEvent", reg_place);

  static luaL_Reg reg_destroy[] = {
      {"position",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onBlockDestroyedEvent*>(L);
         return pushVector(L, arg->pos);
       }},
      {"destroyer",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onBlockDestroyedEvent*>(L);
         lua_pushentity(L, arg->user);
         return 1;
       }},
      {"itemStack",
       [](lua_State* L) -> int {
         auto lobj    = LuaObject::fromstack(L, 1);
         auto arg     = *lobj->get<onBlockDestroyedEvent*>(L);
         auto is_lobj = lua_pushitemstack(L, &arg->is);
         lobj->link(is_lobj);
         return 1;
       }},

      {nullptr, nullptr},
  };
  createEventArg(L, "onBlockDestroyedEvent", reg_destroy);
}

static void regMessage(lua_State* L) {
  static luaL_Reg reg_msg[] = {
      // todo create LuaObject for strings maybe?
      {"cancel",
       [](lua_State* L) -> int {
         auto lobj      = LuaObject::fromstack(L, 1);
         auto arg       = *lobj->get<onMessageEvent*>(L);
         arg->cancelled = true;
         return 0;
       }},
      {"isCancelled",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onMessageEvent*>(L);
         lua_pushboolean(L, arg->cancelled);
         return 1;
       }},
      {"message",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onMessageEvent*>(L);

         std::string mb;
         mb.reserve(arg->message.length());

         std::wctomb(nullptr, 0);
         for (auto it = arg->message.begin(); it != arg->message.end(); ++it) {
           char dst;
           if (std::wctomb(&dst, (*it)) > 0) mb.push_back(dst);
         }

         if (lua_pushstring(L, mb.c_str()) != nullptr) {
           mb.clear();
           return 1;
         }

         return 0;
       }},
      {"sender",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onMessageEvent*>(L);
         lua_pushentity(L, arg->sender);
         return 1;
       }},
      {"finalMessage",
       [](lua_State* L) -> int {
         auto fin  = std::string_view(luaL_checkstring(L, 2));
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onMessageEvent*>(L);

         arg->finalMessage.clear();
         std::mbtowc(nullptr, nullptr, 0);
         for (auto it = fin.begin(); it != fin.end(); ++it) {
           wchar_t dst;
           if (std::mbtowc(&dst, &(*it), 1) > 0) arg->finalMessage.push_back(dst);
         }

         return 0;
       }},

      {nullptr, nullptr},
  };
  createEventArg(L, "onMessageEvent", reg_msg);
}

static void regPlayer(lua_State* L) {
  static luaL_Reg reg_conn[] = {
      {"cancel",
       [](lua_State* L) -> int {
         auto lobj      = LuaObject::fromstack(L, 1);
         auto arg       = *lobj->get<onPlayerConnectedEvent*>(L);
         arg->reason    = luaL_optstring(L, 2, "Kicked by Server");
         arg->cancelled = true;
         return 0;
       }},
      {"isCancelled",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onPlayerConnectedEvent*>(L);
         lua_pushboolean(L, arg->cancelled);
         return 1;
       }},
      {"entity",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onPlayerConnectedEvent*>(L);
         lua_pushentity(L, arg->ent);
         return 1;
       }},

      {nullptr, nullptr},
  };
  createEventArg(L, "onPlayerConnectedEvent", reg_conn);
}

void registerMetaTables(lua_State* L) {
  regBlockEvents(L);
  regMessage(L);
  regPlayer(L);
}
