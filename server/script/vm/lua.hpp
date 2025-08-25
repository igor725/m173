#pragma once

// clang-format off

extern "C" {
#include "lua.h" // IWYU pragma: export
#include "lualib.h" // IWYU pragma: export
#include "lauxlib.h" // IWYU pragma: export
}

// clang-format on

#define lua_stackprot_start(L) auto __cstkpos = lua_gettop(L);
#define lua_stackprot_end(L)                                                                                                                                   \
  if (__cstkpos != lua_gettop(L)) luaL_error(L, "Corrupted stack!");
