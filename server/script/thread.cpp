#include "thread.h"

#include "luaobject.h"

#include <spdlog/spdlog.h>

class ScriptThread: public IScriptThread {
  public:
  ScriptThread(lua_State* thread, const std::string& path, int threadId): m_self(thread), m_selfId(threadId), m_status(Alive), m_path(path) { reload(); }

  ~ScriptThread() {}

  void reload() final {
    lua_settop(m_self, 0); // Clearing the thread stack just in case
    m_status = Dead;

    if (auto err = luaL_loadfile(m_self, m_path.c_str())) {
      spdlog::error("Syntax error in {}: {}", m_path, lua_tostring(m_self, -1));
      return;
    }

    int nres;
    switch (lua_resume(m_self, nullptr, 0, &nres)) {
      case LUA_ERRMEM: throw std::bad_alloc();
      case LUA_YIELD: {
        m_status = Alive;
      } break;
      case LUA_OK: {
        spdlog::warn("Script {} does not contain coroutine loop, closing...", m_path);
        m_status = Closed;
      } break;
      case LUA_ERRERR: {
        spdlog::error("Script {} got runtime error: {}", m_path, lua_tostring(m_self, -1));
      } break;
    }

    lua_pop(m_self, nres);
  }

  Status getStatus() const final { return m_status; }

  int getId() const final { return m_selfId; }

  void postEvent(const ScriptEvent& ev) final {
    int nres = 0;

    LuaObject* root = nullptr;

    int acnt = createEventArguments(ev, &root);
    switch (lua_resume(m_self, nullptr, acnt, &nres)) {
      case LUA_ERRMEM: throw std::bad_alloc();
      case LUA_YIELD: {
        // Nothing bad happened to the thread
      } break;
      case LUA_OK: {
        m_status = Closed;
      } break;
      default: {
        spdlog::error("Runtime error in {}: {}", (const void*)m_self, lua_tostring(m_self, -1));
        m_status = Dead;
      } break;
    }

    if (root) root->invalidate();
    lua_pop(m_self, nres);
  }

  private:
  int createEventArguments(const ScriptEvent& ev, LuaObject** obj) {
    *obj = nullptr;

    switch (ev.type) {
      case ScriptEvent::onStart: {
        lua_pushliteral(m_self, "onStart");
        return 1;
      } break;
      case ScriptEvent::onStop: {
        lua_pushliteral(m_self, "onStop");
        return 1;
      } break;
      case ScriptEvent::preBlockPlace: {
        lua_pushliteral(m_self, "preBlockPlace");
        *obj = LuaObject::create(m_self, sizeof(void*));

        *(*obj)->get<void*>() = ev.args;
        luaL_setmetatable(m_self, "preBlockPlaceEvent");
        return 2;
      } break;
      case ScriptEvent::onBlockDestroyed: {
        lua_pushliteral(m_self, "onBlockDestroyed");
        *obj = LuaObject::create(m_self, sizeof(void*));

        *(*obj)->get<void*>() = ev.args;
        luaL_setmetatable(m_self, "onBlockDestroyedEvent");
        return 2;
      } break;
    }

    spdlog::error("Just pushed empty event to thread {}!", (const void*)m_self);
    return 0;
  }

  private:
  Status      m_status;
  lua_State*  m_self;
  int         m_selfId;
  std::string m_path;
};

std::unique_ptr<IScriptThread> createThread(lua_State* thread, const std::filesystem::path& path, int threadId) {
  return std::make_unique<ScriptThread>(thread, path.string(), threadId);
}
