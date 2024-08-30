#include "libitemstack.h"

#include "items/item.h"

class ItemStackScript {
  public:
  ItemStackScript(): m_isPtr(false), m_itemStack() {}

  ItemStackScript(ItemStack* is): m_isPtr(true), m_itemStackPtr(is) {}

  ItemStack* get() {
    if (m_isPtr) return m_itemStackPtr;
    return &m_itemStack;
  }

  private:
  bool m_isPtr;

  union {
    ItemStack* m_itemStackPtr;
    ItemStack  m_itemStack;
  };
};

LuaObject* lua_pushitemstack(lua_State* L) {
  auto lobj = LuaObject::create(L, sizeof(ItemStackScript));
  luaL_setmetatable(L, "ItemStack");
  new (lobj->get<ItemStackScript>(L)) ItemStackScript();
  return lobj;
}

LuaObject* lua_pushitemstack(lua_State* L, ItemStack* is) {
  auto lobj = LuaObject::create(L, sizeof(ItemStackScript));
  luaL_setmetatable(L, "ItemStack");
  new (lobj->get<ItemStackScript>(L)) ItemStackScript(is);
  return lobj;
}

int luaopen_itemstack(lua_State* L) {
  static const luaL_Reg isfuncs[] = {
      {"decrementBy",
       [](lua_State* L) -> int {
         auto stacksz = luaL_checkinteger(L, 2);
         auto lobj    = LuaObject::fromstack(L, 1);
         auto iss     = lobj->get<ItemStackScript>(L);

         lua_pushboolean(L, iss->get()->decrementBy(stacksz));
         return 1;
       }},
      {"incrementBy",
       [](lua_State* L) -> int {
         auto stacksz = luaL_checkinteger(L, 2);
         auto lobj    = LuaObject::fromstack(L, 1);
         auto iss     = lobj->get<ItemStackScript>(L);

         lua_pushboolean(L, iss->get()->incrementBy(stacksz));
         return 1;
       }},
      {"itemId",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto iss  = lobj->get<ItemStackScript>(L);

         if (lua_isinteger(L, 2)) {
           ItemId id = lua_tointeger(L, 2);
           if (Item::exists(id)) {
             iss->get()->itemId = id;
             lua_pushboolean(L, true);
           } else {
             lua_pushboolean(L, false);
           }
         } else {
           lua_pushinteger(L, iss->get()->itemId);
         }

         return 1;
       }},
      {"stackSize",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto iss  = lobj->get<ItemStackScript>(L);

         if (lua_isinteger(L, 2)) { // todo check there?
           iss->get()->stackSize = lua_tointeger(L, 2);
           return 0;
         }

         lua_pushinteger(L, iss->get()->stackSize);
         return 1;
       }},
      {"damage",
       [](lua_State* L) -> int {
         auto lobj = LuaObject::fromstack(L, 1);
         auto iss  = lobj->get<ItemStackScript>(L);

         if (lua_isinteger(L, 2)) {
           lua_pushboolean(L, iss->get()->damageItem(lua_tointeger(L, 2), nullptr));
         } else {
           lua_pushinteger(L, iss->get()->itemDamage);
         }

         return 1;
       }},

      {nullptr, nullptr},
  };

  luaL_newmetatable(L, "ItemStack");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, isfuncs, 0);
  lua_pop(L, 1);
  return 0;
}
