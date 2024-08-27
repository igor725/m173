#pragma once

#include "event.h"
#include "vm/lua.hpp"

#include <filesystem>
#include <memory>

class IScriptThread {
  public:
  enum Status {
    Alive,
    Dead,
    Closed,
  };

  IScriptThread()          = default;
  virtual ~IScriptThread() = default;

  virtual bool isNamesEqual(const std::filesystem::path& name) const = 0;

  virtual Status getStatus() const = 0;

  virtual const std::wstring& getStatusStr() const = 0;

  virtual int getId() const = 0;

  virtual void reload() = 0;

  virtual void postEvent(const ScriptEvent& ev) = 0;
};

std::unique_ptr<IScriptThread> createThread(lua_State* thread, const std::filesystem::path& path, int threadId);
