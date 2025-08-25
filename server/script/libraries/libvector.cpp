#include "libvector.h"

#include <spdlog/spdlog.h>

class VectorScript {
  public:
  enum Type : int8_t {
    BVec3,
    IVec2,
    IVec3,
    DVec3,
    FAngle,
  };

  static VectorScript* create(lua_State* L, Type vtype, void* vptr, bool constant) {
    auto lobj = LuaObject::create(L, sizeof(VectorScript) + (vptr == nullptr ? getVectorSize(vtype) : 0));
    luaL_setmetatable(L, "Vector");
    auto vs     = lobj->get<VectorScript>(L);
    vs->m_vptr  = vptr == nullptr ? &vs[1] : vptr;
    vs->m_const = constant;
    vs->m_type  = vtype;
    return vs;
  }

  LuaObject* object() const { return (LuaObject*)&this[-1]; }

  bool isConst() const { return m_const; }

  template <typename T>
  T* getAs(lua_State* L) {
    if (getVectorSize(m_type) != sizeof(T)) {
      luaL_error(L, "Vector sizes mismatch!");
      return nullptr;
    }
    return (T*)m_vptr;
  }

  template <typename T>
  T* getAs() {
    return (T*)m_vptr;
  }

  Type getType() const { return m_type; }

  size_t getSize() const { return getVectorSize(m_type); }

  private:
  static inline size_t getVectorSize(Type type) {
    switch (type) {
      case BVec3: return sizeof(ByteVector3);
      case IVec2: return sizeof(IntVector2);
      case IVec3: return sizeof(IntVector3);
      case DVec3: return sizeof(DoubleVector3);
      case FAngle: return sizeof(FloatAngle);
    }

    spdlog::warn("Just pushed vector of 0 size!");
    return 0;
  }

  Type  m_type;
  bool  m_const;
  void* m_vptr;
};

LuaObject* lua_checkvector(lua_State* L, int idx) {
  return LuaObject::fromstack(L, idx, "Vector");
}

static inline void testvecptr(lua_State* L, void* ptr) {
  if (ptr == nullptr) luaL_error(L, "Pushed nullptr vector!");
}

LuaObject* lua_pushvector(lua_State* L, ByteVector3* vec, bool constant) {
  testvecptr(L, vec);
  return VectorScript::create(L, VectorScript::BVec3, vec, constant)->object();
}

LuaObject* lua_pushvector(lua_State* L, IntVector2* vec, bool constant) {
  testvecptr(L, vec);
  return VectorScript::create(L, VectorScript::IVec2, vec, constant)->object();
}

LuaObject* lua_pushvector(lua_State* L, IntVector3* vec, bool constant) {
  testvecptr(L, vec);
  return VectorScript::create(L, VectorScript::IVec3, vec, constant)->object();
}

LuaObject* lua_pushvector(lua_State* L, DoubleVector3* vec, bool constant) {
  testvecptr(L, vec);
  return VectorScript::create(L, VectorScript::DVec3, vec, constant)->object();
}

LuaObject* lua_pushangle(lua_State* L, FloatAngle* ang, bool constant) {
  testvecptr(L, ang);
  return VectorScript::create(L, VectorScript::FAngle, ang, constant)->object();
}

int luaopen_vector(lua_State* L) {
  static auto getVInfo = [](lua_State* L, VectorScript::Type type, int& mcnt, int& mtype) {
    switch (type) {
      case VectorScript::BVec3: {
        mcnt  = 3;
        mtype = 0;
      } break;
      case VectorScript::IVec2: {
        mcnt  = 2;
        mtype = 1;
      } break;
      case VectorScript::IVec3: {
        mcnt  = 3;
        mtype = 1;
      } break;
      case VectorScript::DVec3: {
        mcnt  = 3;
        mtype = 2;
      } break;
      case VectorScript::FAngle: {
        mcnt  = 3;
        mtype = 3;
      } break;
      default: {
        mcnt  = -1;
        mtype = -1;
        luaL_error(L, "Unknown vector type passed to getVInfo!");
      } break;
    }
  };

  static const luaL_Reg vector_meta[] = {
      {"__tostring",
       [](lua_State* L) -> int {
         auto vs = lua_checkvector(L, 1)->get<VectorScript>(L);

         auto _const = vs->isConst() ? "const " : "";
         switch (vs->getType()) {
           case VectorScript::BVec3: {
             auto bvec = vs->getAs<ByteVector3>(L);
             lua_pushfstring(L, "%sByteVector(%d,%d,%d)", _const, bvec->x, bvec->y, bvec->z);
           } break;
           case VectorScript::IVec2: {
             auto bvec = vs->getAs<IntVector2>(L);
             lua_pushfstring(L, "%sIntVector2(%d,%d)", _const, bvec->x, bvec->z);
           } break;
           case VectorScript::IVec3: {
             auto bvec = vs->getAs<IntVector3>(L);
             lua_pushfstring(L, "%sIntVector3(%d,%d,%d)", _const, bvec->x, bvec->y, bvec->z);
           } break;
           case VectorScript::DVec3: {
             auto bvec = vs->getAs<DoubleVector3>(L);
             lua_pushfstring(L, "%sDoubleVector3(%f,%f,%f)", _const, bvec->x, bvec->y, bvec->z);
           } break;
           case VectorScript::FAngle: {
             auto bvec = vs->getAs<FloatAngle>(L);
             lua_pushfstring(L, "%sFloatAngle(%f,%f)", _const, bvec->yaw, bvec->pitch);
           } break;

           default: {
             luaL_error(L, "Unknown vector type");
           } break;
         }
         return 1;
       }},
      {"set",
       [](lua_State* L) -> int {
         auto vs = lua_checkvector(L, 1)->get<VectorScript>(L);
         if (vs->isConst()) {
           luaL_error(L, "Attempt to change values of constant vector");
           return 0;
         }

         int membCnt, membType;
         getVInfo(L, vs->getType(), membCnt, membType);

         auto setmemb = [L, membType, &vs](int membNum) {
           auto vmem = vs->getAs<char>();
           int  idx  = 2 + membNum;

           switch (membType) {
             case 0: {
               if (lua_isinteger(L, idx)) vmem[membNum] = lua_tointeger(L, idx);
             } break;
             case 1: {
               if (lua_isinteger(L, idx)) ((int32_t*)vmem)[membNum] = lua_tointeger(L, idx);
             } break;
             case 2: {
               if (lua_isnumber(L, idx)) ((double_t*)vmem)[membNum] = lua_tonumber(L, idx);
             } break;
             case 3: {
               if (lua_isnumber(L, idx)) ((float_t*)vmem)[membNum] = (float_t)lua_tonumber(L, idx);
             } break;

             default: break;
           }
         };

         for (int i = 0; i < membCnt; ++i) {
           setmemb(i);
         }

         return 0;
       }},
      {"get",
       [](lua_State* L) -> int {
         auto vs = lua_checkvector(L, 1)->get<VectorScript>(L);

         int membCnt, membType;
         getVInfo(L, vs->getType(), membCnt, membType);

         auto vmem = vs->getAs<char>();
         for (int i = 0; i < membCnt; ++i) {
           switch (membType) {
             case 0: {
               lua_pushinteger(L, vmem[i]);
             } break;
             case 1: {
               lua_pushinteger(L, ((int32_t*)vmem)[i]);
             } break;
             case 2: {
               lua_pushnumber(L, ((double_t*)vmem)[i]);
             } break;
             case 3: {
               lua_pushnumber(L, ((float_t*)vmem)[i]);
             } break;
           }
         }

         return membCnt;
       }},

      // todo __newindex??

      {nullptr, nullptr},
  };
  luaL_newmetatable(L, "Vector");
  luaL_setfuncs(L, vector_meta, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  lua_pop(L, 1);

  const luaL_Reg vector_reg[] = {
      {"bvec3",
       [](lua_State* L) -> int {
         auto vobj  = VectorScript::create(L, VectorScript::Type::BVec3, nullptr, lua_toboolean(L, 4));
         auto bvec3 = vobj->getAs<ByteVector3>(L);
         if (lua_gettop(L) >= 3) {
           bvec3->x = (int8_t)luaL_checkinteger(L, 1);
           bvec3->y = (int8_t)luaL_checkinteger(L, 2);
           bvec3->z = (int8_t)luaL_checkinteger(L, 3);
         } else {
           *bvec3 = {0, 0, 0};
         }
         return 1;
       }},
      {"ivec2",
       [](lua_State* L) -> int {
         auto vobj  = VectorScript::create(L, VectorScript::Type::IVec2, nullptr, lua_toboolean(L, 3));
         auto ivec2 = vobj->getAs<IntVector2>(L);
         if (lua_gettop(L) >= 2) {
           ivec2->x = (int32_t)luaL_checkinteger(L, 1);
           ivec2->z = (int32_t)luaL_checkinteger(L, 2);
         } else {
           *ivec2 = {0, 0};
         }
         return 1;
       }},
      {"ivec3",
       [](lua_State* L) -> int {
         auto vobj  = VectorScript::create(L, VectorScript::Type::IVec3, nullptr, lua_toboolean(L, 4));
         auto ivec3 = vobj->getAs<IntVector3>(L);
         if (lua_gettop(L) >= 3) {
           ivec3->x = (int32_t)luaL_checkinteger(L, 1);
           ivec3->y = (int32_t)luaL_checkinteger(L, 2);
           ivec3->z = (int32_t)luaL_checkinteger(L, 3);
         } else {
           *ivec3 = {0, 0, 0};
         }
         return 1;
       }},
      {"dvec3",
       [](lua_State* L) -> int {
         auto vobj  = VectorScript::create(L, VectorScript::Type::DVec3, nullptr, lua_toboolean(L, 4));
         auto dvec3 = vobj->getAs<DoubleVector3>(L);
         if (lua_gettop(L) >= 3) {
           dvec3->x = (double_t)luaL_checknumber(L, 1);
           dvec3->y = (double_t)luaL_checknumber(L, 2);
           dvec3->z = (double_t)luaL_checknumber(L, 3);
         } else {
           *dvec3 = {0.0, 0.0, 0.0};
         }
         return 1;
       }},

      {nullptr, nullptr},
  };
  lua_newtable(L);
  luaL_setfuncs(L, vector_reg, 0);
  return 1;
}
