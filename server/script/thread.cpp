#include "thread.h"

#include <spdlog/spdlog.h>

class ScriptThread: public IScriptThread {
  public:
  ScriptThread(lua_State* thread, const std::string& path, int threadId): m_self(thread), m_selfId(threadId), m_status(Alive), m_path(path) { reload(); }

  ~ScriptThread() {}

  void reload() {
    m_status = Dead;

    if (auto err = luaL_loadfile(m_self, m_path.c_str())) {
      spdlog::error("Syntax error in {}: {}", m_path, lua_tostring(m_self, -1));
      return;
    }

    int nres;
    if (lua_resume(m_self, nullptr, 0, &nres) != LUA_YIELD) {
      spdlog::error("Failed to enter {} coroutine loop: {}", m_path, lua_tostring(m_self, -1));
    } else {
      m_status = Alive;
    }

    lua_pop(m_self, nres);
  }

  Status getStatus() const final { return m_status; }

  int getId() const final { return m_selfId; }

  void postEvent(const ScriptEvent& ev) final {
    int nres = 0;
    int acnt = createEventArguments(ev);
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

    lua_pop(m_self, nres);
  }

  private:
  int createEventArguments(const ScriptEvent& ev) {
    switch (ev.type) {
      case ScriptEvent::onStart: {
        lua_pushliteral(m_self, "onStart");
        return 1;
      } break;
      case ScriptEvent::onStop: {
        lua_pushliteral(m_self, "onStop");
        return 1;
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
