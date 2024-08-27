#include "thread.h"

#include "luaobject.h"

#include <spdlog/spdlog.h>

class ScriptThread: public IScriptThread {
  public:
  ScriptThread(lua_State* thread, const std::filesystem::path& path, int threadId): m_self(thread), m_selfId(threadId), m_status(Alive), m_path(path) {
    reload();
  }

  ~ScriptThread() {}

  void reload() final {
    m_status = Dead;
    if (lua_closethread(m_self, nullptr) != LUA_OK) { // Clearing the thread call stack and shit
      spdlog::warn("Failed to close {} thread", (const void*)m_self);
      return;
    }

    auto strpath = m_path.string();
    if (auto err = luaL_loadfile(m_self, strpath.c_str())) {
      spdlog::error("Syntax error in {}: {}", strpath.c_str(), lua_tostring(m_self, -1));
      return;
    }

    int nres;
    switch (lua_resume(m_self, nullptr, 0, &nres)) {
      case LUA_ERRMEM: throw std::bad_alloc();
      case LUA_YIELD: {
        m_status = Alive;
      } break;
      case LUA_OK: {
        spdlog::warn("Script {} does not contain coroutine loop, closing...", strpath.c_str());
        m_status = Closed;
      } break;
      case LUA_ERRERR: {
        spdlog::error("Script {} got runtime error: {}", strpath.c_str(), lua_tostring(m_self, -1));
      } break;
    }

    lua_pop(m_self, nres);
  }

  bool isNamesEqual(const std::filesystem::path& name) const final { return m_path.filename() == name; }

  Status getStatus() const final { return m_status; }

  const std::wstring& getStatusStr() const final {
    static const std::wstring statuses[] = {L"Alive", L"Dead", L"Closed"};
    return statuses[m_status];
  }

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
      case ScriptEvent::onMessage: {
        lua_pushliteral(m_self, "onMessage");
        *obj = LuaObject::create(m_self, sizeof(void*));

        *(*obj)->get<void*>() = ev.args;
        luaL_setmetatable(m_self, "onMessageEvent");
        return 2;
      } break;
    }

    spdlog::error("Just pushed empty event to thread {}!", (const void*)m_self);
    return 0;
  }

  private:
  Status     m_status;
  lua_State* m_self;
  int        m_selfId;

  std::filesystem::path m_path;
};

std::unique_ptr<IScriptThread> createThread(lua_State* thread, const std::filesystem::path& path, int threadId) {
  return std::make_unique<ScriptThread>(thread, path.string(), threadId);
}
