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

LuaObject* lua_newitemstack(lua_State* L) {
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

ItemStack* lua_checkitemstack(lua_State* L, int idx) {
  return LuaObject::fromstack(L, idx, "ItemStack")->get<ItemStackScript>(L)->get();
}

int luaopen_itemstack(lua_State* L) {
  static const luaL_Reg isfuncs[] = {
      {"__tostring",
       [](lua_State* L) -> int {
         auto is = lua_checkitemstack(L, 1);
         if (is->isEmpty()) {
           lua_pushliteral(L, "ItemStack(empty)");
           return 1;
         }
         lua_pushfstring(L, "ItemStack(%d of %d:%d)", is->stackSize, is->itemId, is->itemDamage);
         return 1;
       }},
      {"decrementBy",
       [](lua_State* L) -> int {
         auto stacksz = luaL_checkinteger(L, 2);
         auto is      = lua_checkitemstack(L, 1);

         lua_pushboolean(L, is->decrementBy(stacksz));
         return 1;
       }},
      {"incrementBy",
       [](lua_State* L) -> int {
         auto stacksz = luaL_checkinteger(L, 2);
         auto is      = lua_checkitemstack(L, 1);

         lua_pushboolean(L, is->incrementBy(stacksz));
         return 1;
       }},
      {"itemId",
       [](lua_State* L) -> int {
         auto is = lua_checkitemstack(L, 1);

         if (lua_isinteger(L, 2)) {
           ItemId id = lua_tointeger(L, 2);
           if (Item::exists(id)) {
             is->itemId = id;
             lua_pushboolean(L, true);
           } else {
             lua_pushboolean(L, false);
           }
         } else {
           lua_pushinteger(L, is->itemId);
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
         auto is = lua_checkitemstack(L, 1);

         if (lua_isinteger(L, 2)) {
           lua_pushboolean(L, is->damageItem(lua_tointeger(L, 2), nullptr));
         } else {
           lua_pushinteger(L, is->itemDamage);
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

  const luaL_Reg itemstack_lib[] = {
      {"create",
       [](lua_State* L) -> int {
         auto is = lua_newitemstack(L);
         return 1;
       }},

      {nullptr, nullptr},
  };

  lua_newtable(L);
  luaL_setfuncs(L, itemstack_lib, 0);
  return 1;
}
