#include "libentity.h"

#include "entity/creatures/pig.h"
#include "entity/creatures/player.h"
#include "entity/entitybase.h"
#include "entity/interact/pickup.h"
#include "entity/manager.h"
#include "entity/objects/arrow.h"
#include "entity/objects/snowball.h"
#include "entity/objects/thunderbolt.h"
#include "entry/helper.h"
#include "lauxlib.h"
#include "libitemstack.h"
#include "libvector.h"
#include "lua.h"
#include "script/libraries/libitemstack.h"
#include "script/libraries/libvector.h"

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

static void parseMetaDataFor(Entities::Base* ent, lua_State* L, int32_t mtidx) {
  mtidx = lua_absindex(L, mtidx);
  luaL_checktype(L, mtidx, LUA_TTABLE);

  { // Global things for all entities
    lua_getfield(L, mtidx, "isOnFire");
    lua_getfield(L, mtidx, "isCrouching");
    lua_getfield(L, mtidx, "isRiding");

    int8_t flags = ent->peekFlags();
    if (!lua_isnoneornil(L, -3)) {
      if (lua_toboolean(L, -3)) {
        flags |= Entities::Base::Flags::IsOnFire;
      } else {
        flags &= ~Entities::Base::Flags::IsOnFire;
      }
    }
    if (!lua_isnoneornil(L, -2)) {
      if (lua_toboolean(L, -2)) {
        flags |= Entities::Base::Flags::IsCrouching;
      } else {
        flags &= ~Entities::Base::Flags::IsCrouching;
      }
    }
    if (!lua_isnoneornil(L, -1)) {
      if (lua_toboolean(L, -1)) {
        flags |= Entities::Base::Flags::IsRiding;
      } else {
        flags &= ~Entities::Base::Flags::IsRiding;
      }
    }
    ent->putFlags(flags);

    lua_pop(L, 3);
  }

  switch (auto entType = ent->getType()) {
    case Entities::Base::Creature: {
      if (ent->isPlayer()) {
        // todo?
      } else {
        if (auto mob = dynamic_cast<Entities::MobBase*>(ent)) {
          switch (auto mobType = mob->getMobType()) {
            case Entities::MobBase::Creeper: {
            } break;
            case Entities::MobBase::Skeleton: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::Spider: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::GiantZombie: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::Zombie: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::Slime: break;
            case Entities::MobBase::Ghast: break;
            case Entities::MobBase::ZombiePigmen: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::Pig: {
              lua_getfield(L, mtidx, "hasSaddle");

              if (!lua_isnoneornil(L, -1)) {
                dynamic_cast<Entities::IPig*>(ent)->setSaddle(lua_toboolean(L, -1));
              }

              lua_pop(L, 1);
            } break;
            case Entities::MobBase::Sheep: break;
            case Entities::MobBase::Cow: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::Chicken: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::Squid: {
              // No metadata for this mob
            } break;
            case Entities::MobBase::Wolf: break;

            default: {
              luaL_error(L, "Unhandled mob type in metadata parse: %d", (int32_t)mobType);
            } break;
          }
        }
      }
    } break;
    case Entities::Base::Object: {
      // todo?
    } break;
    case Entities::Base::Thunderbolt: {
      // todo?
    } break;
    case Entities::Base::Pickup: {
      // todo?
    } break;

    default: {
      luaL_error(L, "Unhandled entity type in metadata parse: %d", (int32_t)entType);
    } break;
  }
}

class EntityScript {
  public:
  EntityScript(Entities::Base* ent): m_ent(ent) {}

  Entities::Base* entity() const { return m_ent; }

  // todo getName method

  static LuaObject* get(lua_State* L, Entities::Base* ent) {
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
      case Entities::Base::Creature: {
        switch (auto ct = dynamic_cast<CreatureBase*>(ent)->getCreatureType()) {
          case CreatureBase::Player: {
            luaL_setmetatable(L, "EntityPlayer");
          } break;

          default: {
            luaL_setmetatable(L, "CreatureBase");
          } break;
        }
      } break;

      default: {
        luaL_setmetatable(L, "EntityBase");
      } break;
    }

    new (lobj->get<EntityScript>(L)) EntityScript(ent);

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
    lua_rawget(L, -2);
    if (auto ent = LuaObject::fromstack(L, -1)) {
      ent->invalidate();
      lua_pushlightuserdata(L, ptr);
      lua_pushnil(L);
      lua_rawset(L, -4);
    }
    lua_pop(L, 2);
  }

  private:
  static int      m_entTabRef;
  Entities::Base* m_ent;
};

int EntityScript::m_entTabRef = -1;

LuaObject* lua_pushentity(lua_State* L, Entities::Base* ent) {
  if (ent == nullptr) {
    lua_pushnil(L);
    return nullptr;
  }

  return EntityScript::get(L, ent);
}

void lua_unlinkentity(lua_State* L, void* ptr) {
  EntityScript::unlink(L, ptr);
}

Entities::Base* lua_checkentity(lua_State* L, int idx) {
  auto lobj = LuaObject::fromstack(L, idx);
  return lobj->get<EntityScript>(L)->entity();
}

Entities::Base* lua_checkentity(lua_State* L, int idx, Entities::Base::Type type) {
  auto sent = lua_checkentity(L, idx);

  if (sent->getType() != type) {
    luaL_error(L, "Invalid entity type (%d expected, got %d)", type, sent->getType());
    return nullptr;
  }

  return sent;
}

CreatureBase* lua_checkcreature(lua_State* L, int idx) {
  auto ent = lua_checkentity(L, idx, Entities::Base::Creature);
  return dynamic_cast<CreatureBase*>(ent);
}

CreatureBase* lua_checkcreature(lua_State* L, int idx, CreatureBase::Type type) {
  auto creat = lua_checkcreature(L, idx);
  if (creat->getCreatureType() != type) {
    luaL_error(L, "Invalid creature type (%d expected, got %d)", type, creat->getCreatureType());
    return nullptr;
  }

  return creat;
}

int luaopen_entity(lua_State* L) {
  const luaL_Reg entitybase_reg[] = {
      {"position",
       [](lua_State* L) -> int {
         auto ent = lua_checkentity(L, 1);
         lua_pushvector(L, &ent->getPosition());
         return 1;
       }},
      {"metadata",
       [](lua_State* L) -> int {
         auto ent = lua_checkentity(L, 1);
         parseMetaDataFor(ent, L, 2);
         return 0;
       }},

      {nullptr, nullptr},
  };
  luaL_newmetatable(L, "EntityBase");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, entitybase_reg, 0);

  const luaL_Reg creaturebase_reg[] = {
      {"health",
       [](lua_State* L) -> int {
         auto creat = lua_checkcreature(L, 1);

         if (lua_isinteger(L, 2)) {
           creat->setHealth(lua_tointeger(L, 2));
         }

         lua_pushinteger(L, creat->getHealth());
         return 1;
       }},
      {"chunk",
       [](lua_State* L) -> int {
         auto creat = lua_checkcreature(L, 1);

         lua_newvector(L, creat->getCurrentChunk(), true);
         return 1;
       }},

      {nullptr, nullptr},
  };
  luaL_newmetatable(L, "CreatureBase");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setmetatable(L, "EntityBase");
  luaL_setfuncs(L, creaturebase_reg, 0);

  const luaL_Reg entityplayer_reg[] = {
      {"heldItem",
       [](lua_State* L) -> int {
         auto ent = lua_checkcreature(L, 1, CreatureBase::Player);
         lua_pushitemstack(L, &dynamic_cast<Entities::PlayerBase*>(ent)->getHeldItem());
         return 1;
       }},
      {"resendItem",
       [](lua_State* L) -> int {
         auto ent = lua_checkcreature(L, 1, CreatureBase::Player);
         auto is  = lua_checkitemstack(L, 2);
         auto ply = dynamic_cast<Entities::PlayerBase*>(ent);
         lua_pushboolean(L, ply->resendItem(*is));
         return 1;
       }},
      {"chat",
       [](lua_State* L) -> int {
         auto ent = lua_checkcreature(L, 1, CreatureBase::Player);

         auto wtext = Helper::cvtToUCS2(luaL_checkstring(L, 2));
         dynamic_cast<Entities::PlayerBase*>(ent)->sendChat(std::wstring_view(wtext.begin(), wtext.end()));

         return 0;
       }},
      {"isLocal",
       [](lua_State* L) -> int {
         auto ply = dynamic_cast<Entities::PlayerBase*>(lua_checkcreature(L, 1, CreatureBase::Player));
         lua_pushboolean(L, ply->isLocal());
         return 1;
       }},
      {"operator",
       [](lua_State* L) -> int {
         auto ply = dynamic_cast<Entities::PlayerBase*>(lua_checkcreature(L, 1, CreatureBase::Player));

         if (lua_isboolean(L, 1)) {
           ply->setOperator(lua_toboolean(L, 1));
         }

         lua_pushboolean(L, ply->isOperator());
         return 1;
       }},
      {"hunger",
       [](lua_State* L) -> int {
         auto ply = dynamic_cast<Entities::PlayerBase*>(lua_checkcreature(L, 1, CreatureBase::Player));
         lua_pushboolean(L, ply->getHunger());
         return 1;
       }},
      {"saturation",
       [](lua_State* L) -> int {
         auto ply = dynamic_cast<Entities::PlayerBase*>(lua_checkcreature(L, 1, CreatureBase::Player));
         lua_pushboolean(L, ply->getSatur());
         return 1;
       }},

      {nullptr, nullptr},
  };
  luaL_newmetatable(L, "EntityPlayer");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_setmetatable(L, "CreatureBase");
  luaL_setfuncs(L, entityplayer_reg, 0);

  lua_pop(L, 2);

  const luaL_Reg entity_lib[] = {
      {"player",
       [](lua_State* L) -> int {
         auto name = Helper::cvtToUCS2(luaL_checkstring(L, 1));
         lua_pushentity(L, Entities::Access::manager().getPlayerByName(name));
         return 1;
       }},

      {"create",
       [](lua_State* L) -> int {
         luaL_checktype(L, 1, LUA_TSTRING);
         luaL_checktype(L, 2, LUA_TTABLE);

         std::unique_ptr<Entities::Base> entptr = {};

         static const int32_t     entnum[]   = {0, 1, 2, 3, 4};
         static const char* const entnames[] = {"arrow", "thunderbolt", "snowball", "pig", "pickup", nullptr};

         auto stackStart = lua_gettop(L);
         switch (luaL_checkoption(L, 1, nullptr, entnames)) {
           case 0: {
             lua_getfield(L, 2, "position");
             lua_getfield(L, 2, "owner");
             lua_getfield(L, 2, "motion");

             auto posVec = lua_checkvector(L, -3)->get<VectorScript>(L);
             auto ownEnt = lua_checkentity(L, -2);
             auto dirVec = lua_checkvector(L, -1)->get<VectorScript>(L);

             entptr = Entities::Create::arrow(*posVec->getAs<DoubleVector3>(L), ownEnt->getEntityId(), *dirVec->getAs<DoubleVector3>(L));
           } break;
           case 1: {
             lua_getfield(L, 2, "position");

             auto posVec = lua_checkvector(L, -1)->get<VectorScript>(L);

             entptr = Entities::Create::thunderbolt(*posVec->getAs<DoubleVector3>(L));
           } break;
           case 2: {
             lua_getfield(L, 2, "position");
             lua_getfield(L, 2, "owner");
             lua_getfield(L, 2, "motion");

             auto posVec = lua_checkvector(L, -3)->get<VectorScript>(L);
             auto ownEnt = lua_checkentity(L, -2);
             auto dirVec = lua_checkvector(L, -1)->get<VectorScript>(L);

             entptr = Entities::Create::snowball(*posVec->getAs<DoubleVector3>(L), ownEnt->getEntityId(), *dirVec->getAs<DoubleVector3>(L));
           } break;
           case 3: {
             lua_getfield(L, 2, "position");

             auto posVec = lua_checkvector(L, -1)->get<VectorScript>(L);

             entptr = Entities::Create::pig(*posVec->getAs<DoubleVector3>(L));
           } break;
           case 4: {
             lua_getfield(L, 2, "position");
             lua_getfield(L, 2, "item");

             auto posVec = lua_checkvector(L, -2)->get<VectorScript>(L);
             auto is     = lua_checkitemstack(L, -1);

             entptr = Entities::Create::pickup(*posVec->getAs<DoubleVector3>(L), *is);
           } break;
         }

         lua_getfield(L, 2, "metadata");
         if (lua_istable(L, -1)) parseMetaDataFor(entptr.get(), L, -1);

         lua_settop(L, stackStart);

         lua_pushentity(L, Entities::Access::manager().AddEntity(std::move(entptr)));
         return 1;
       }},

      {nullptr, nullptr},
  };

  lua_newtable(L);
  luaL_setfuncs(L, entity_lib, 0);
  return 1;
}
