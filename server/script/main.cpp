#include "event.h"
#include "script.h"
#include "thread.h"
#include "vm/lua.hpp"

#include <mutex>
#include <new>
#include <spdlog/spdlog.h>
#include <vector>

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

    luaopen_base(m_mainState);
    lua_setglobal(m_mainState, LUA_GNAME);
    luaopen_coroutine(m_mainState);
    lua_setglobal(m_mainState, LUA_COLIBNAME);
    luaopen_math(m_mainState);
    lua_setglobal(m_mainState, LUA_MATHLIBNAME);
    luaopen_string(m_mainState);
    lua_setglobal(m_mainState, LUA_STRLIBNAME);
    luaopen_table(m_mainState);
    lua_setglobal(m_mainState, LUA_TABLIBNAME);
    luaopen_utf8(m_mainState);
    lua_setglobal(m_mainState, LUA_UTF8LIBNAME);

    lua_pushcfunction(m_mainState, [](lua_State* L) -> int {
      spdlog::info("Lua[{}]: {}", (const void*)L, lua_tostring(L, 1));
      return 0;
    });
    lua_setglobal(m_mainState, "print");
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
