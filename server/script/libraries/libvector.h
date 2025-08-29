#pragma once

#include "../luaobject.h"
#include "../vm/lua.hpp"
#include "entry/helper.h"

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

    return 0;
  }

  Type  m_type;
  bool  m_const;
  void* m_vptr;
};

LuaObject* lua_pushvector(lua_State* L, ByteVector3* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, IntVector2* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, IntVector3* vec, bool constant = false);
LuaObject* lua_pushvector(lua_State* L, DoubleVector3* vec, bool constant = false);
LuaObject* lua_pushangle(lua_State* L, FloatAngle* ang, bool constant = false);

LuaObject* lua_newvector(lua_State* L, const ByteVector3& vec, bool constant);
LuaObject* lua_newvector(lua_State* L, const IntVector2& vec, bool constant);
LuaObject* lua_newvector(lua_State* L, const IntVector3& vec, bool constant);
LuaObject* lua_newvector(lua_State* L, const DoubleVector3& vec, bool constant);
LuaObject* lua_newangle(lua_State* L, const FloatAngle& ang, bool constant);

template <typename T>
LuaObject* lua_pushvector(lua_State* L, const T* vec) {
  return lua_pushvector(L, const_cast<T*>(vec), true);
}

LuaObject* lua_checkvector(lua_State* L, int idx);

int luaopen_vector(lua_State* L);
