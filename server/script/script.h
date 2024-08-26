#pragma once

#include "event.h"

#include <filesystem>

class IScriptVM {
  public:
  IScriptVM()          = default;
  virtual ~IScriptVM() = default;

  virtual void loadScriptsFrom(const std::filesystem::path& path) = 0;

  virtual void postEvent(const ScriptEvent& ev) = 0;
};

IScriptVM& accessScript();
