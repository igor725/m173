#include "event.h"

#include "libraries/itemstack.h"
#include "vm/lua.hpp"

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
         auto arg       = *lobj->get<preBlockPlaceArgumentEvent*>();
         arg->cancelled = true;
         return 0;
       }},
      {"isCancelled",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>();
         lua_pushboolean(L, arg->cancelled);
         return 1;
       }},
      {"getPosition",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>();
         return pushVector(L, arg->pos);
       }},
      {"getDirection",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>();
         lua_pushinteger(L, arg->direction);
         return 1;
       }},
      {"getPlacer",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<preBlockPlaceArgumentEvent*>();
         lua_pushlightuserdata(L, arg->user);
         return 1;
       }},
      {"getItemStack",
       [](lua_State* L) -> int {
         auto lobj    = LuaObject::fromstack(L, 1);
         auto arg     = *lobj->get<preBlockPlaceArgumentEvent*>();
         auto is_lobj = lua_pushitemstack(L, &arg->is);
         lobj->link(is_lobj);
         return 1;
       }},

      {nullptr, nullptr},
  };
  createEventArg(L, "preBlockPlaceEvent", reg_place);

  static luaL_Reg reg_destroy[] = {
      {"getPosition",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onBlockDestroyedEvent*>();
         return pushVector(L, arg->pos);
       }},
      {"getDestroyer",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto arg  = *lobj->get<onBlockDestroyedEvent*>();
         lua_pushlightuserdata(L, arg->user);
         return 1;
       }},
      {"getItemStack",
       [](lua_State* L) -> int {
         auto lobj    = LuaObject::fromstack(L, 1);
         auto arg     = *lobj->get<onBlockDestroyedEvent*>();
         auto is_lobj = lua_pushitemstack(L, &arg->is);
         lobj->link(is_lobj);
         return 1;
       }},

      {nullptr, nullptr},
  };
  createEventArg(L, "onBlockDestroyedEvent", reg_destroy);
}

void registerMetaTables(lua_State* L) {
  regBlockEvents(L);
}
