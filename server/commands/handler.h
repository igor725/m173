#pragma once

#include "entity/creatures/player.h"

#include <string>

class Command;

class ICommandHandler {
  public:
  ICommandHandler()          = default;
  virtual ~ICommandHandler() = default;

  virtual bool registerCommand(Command* cmd)   = 0;
  virtual bool unregisterCommand(Command* cmd) = 0;

  virtual bool execute(PlayerBase* caller, std::wstring command, std::wstring& out) = 0;

  virtual void genHelp(int32_t page, int32_t perpage, std::wstring& out) = 0;
};

ICommandHandler& accessCommandHandler();

class Command {
  public:
  Command(const wchar_t* name, const wchar_t* help, bool playerOnly = false): m_commandName(name), m_helpMessage(help), m_playerOnly(playerOnly) {
    accessCommandHandler().registerCommand(this);
  }

  ~Command() { accessCommandHandler().unregisterCommand(this); }

  virtual bool execute(PlayerBase* caller, std::vector<std::wstring_view>& args, std::wstring& out) = 0;

  const std::wstring& getName() const { return m_commandName; }

  const std::wstring& getHelp() const { return m_helpMessage; }

  bool isPlayerOnly() const { return m_playerOnly; }

  inline bool isNamesEqual(Command* cmd) const { return cmd == this || Helper::stricmp(cmd->getName(), getName()); }

  private:
  const std::wstring m_commandName;
  const std::wstring m_helpMessage;
  const bool         m_playerOnly;
};
