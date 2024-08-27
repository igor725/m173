#include "event.h"
#include "libraries/itemstack.h"
#include "script.h"
#include "thread.h"
#include "vm/lua.hpp"

#include <mutex>
#include <new>
#include <spdlog/spdlog.h>
#include <vector>

void registerMetaTables(lua_State* L);

class ScriptVM: public IScriptVM {
  public:
  ScriptVM() {
    m_mainState = luaL_newstate();
    if (m_mainState == nullptr) throw std::bad_alloc();

    lua_newtable(m_mainState);
    m_threadsTable = luaL_ref(m_mainState, LUA_REGISTRYINDEX);

    lua_atpanic(m_mainState, [](lua_State* L) -> int {
      spdlog::critical("=====================================");
      spdlog::critical("Lua at panic!");
      spdlog::critical("Error: {}", lua_tostring(L, -1));
      spdlog::critical("=====================================");
      return 0;
    });

    static const luaL_Reg libs[] = {
        // std libraries
        {LUA_GNAME, luaopen_base},
        {LUA_COLIBNAME, luaopen_coroutine},
        {LUA_MATHLIBNAME, luaopen_math},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_TABLIBNAME, luaopen_table},
        {LUA_UTF8LIBNAME, luaopen_utf8},

        // server libraries
        {"itemstack", luaopen_itemstack},

        {nullptr, nullptr},
    };

    const luaL_Reg* lib;
    for (lib = libs; lib->func; lib++) {
      luaL_requiref(m_mainState, lib->name, lib->func, 1);
      lua_pop(m_mainState, 1);
    }

    lua_pushcfunction(m_mainState, [](lua_State* L) -> int {
      int count = lua_gettop(L);
      if (count > 0) {
        lua_getglobal(L, "tostring");
        lua_pushliteral(L, "");

        for (int i = 1; i <= count; ++i) {
          lua_pushliteral(L, " ");
          lua_pushvalue(L, -3);
          lua_pushvalue(L, i);
          lua_call(L, 1, 1);
          lua_concat(L, 3);
        }

        spdlog::info("Lua[{}]: {}", (const void*)L, lua_tostring(L, -1));
      }

      return 0;
    });
    lua_setglobal(m_mainState, "print");

    registerMetaTables(m_mainState);
  }

  ~ScriptVM() {
    for (auto it = m_threads.begin(); it != m_threads.end();) {
      (*it)->postEvent({ScriptEvent::onStop});
      it = m_threads.erase(it);
    }

    lua_close(m_mainState);
  }

  void openScript(const std::filesystem::path& scr) {
    std::unique_lock lock(m_stateLock);

    auto preCall = lua_gettop(m_mainState);

    auto thstate = lua_newthread(m_mainState);
    if (thstate == nullptr) throw std::bad_alloc();

    lua_rawgeti(m_mainState, LUA_REGISTRYINDEX, m_threadsTable);
    lua_pushvalue(m_mainState, -2);
    auto& thread = m_threads.emplace_back(createThread(thstate, scr, luaL_ref(m_mainState, -2)));
    thread->postEvent({ScriptEvent::onStart});
    lua_settop(m_mainState, preCall);
  }

  void loadScriptsFrom(const std::filesystem::path& path) final {
    std::unique_lock lock(m_stateLock);

    if (!std::filesystem::exists(path)) std::filesystem::create_directories(path);
    for (const auto& entry: std::filesystem::directory_iterator(path)) {
      if (entry.is_regular_file() && entry.path().extension() == ".lua") {
        openScript(entry.path());
      }
    }
  }

  void postEvent(const ScriptEvent& ev) final {
    std::unique_lock lock(m_stateLock);

    for (auto it = m_threads.begin(); it != m_threads.end();) {
      switch ((*it)->getStatus()) {
        case IScriptThread::Alive: {
          (*it)->postEvent(ev);
          ++it;
        } break;
        case IScriptThread::Dead: {
          ++it;
        } break;
        case IScriptThread::Closed: {
          lua_rawgeti(m_mainState, LUA_REGISTRYINDEX, m_threadsTable);
          luaL_unref(m_mainState, -1, (*it)->getId());
          lua_pop(m_mainState, 1);
          it = m_threads.erase(it);
        } break;
      }
    }
  }

  private:
  std::recursive_mutex                        m_stateLock;
  lua_State*                                  m_mainState;
  int32_t                                     m_threadsTable;
  std::vector<std::unique_ptr<IScriptThread>> m_threads;
};

IScriptVM& accessScript() {
  static ScriptVM inst;
  return inst;
}
