#pragma once

#include "event.h"

#include <filesystem>
#include <string>

class IScriptVM {
  public:
  IScriptVM()          = default;
  virtual ~IScriptVM() = default;

  virtual void registerDirectory(const std::filesystem::path& path) = 0;

  virtual bool openScript(const std::filesystem::path& scr) = 0;

  virtual void reloadAll() = 0;

  virtual bool reload(const std::filesystem::path& name) = 0;

  virtual void getStatus(std::wstring& out) = 0;

  virtual void getStatus(std::wstring& out, const std::filesystem::path& name) = 0;

  virtual void postEvent(const ScriptEvent& ev) = 0;
};

IScriptVM& accessScript();
