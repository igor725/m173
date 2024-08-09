#pragma once

#include "entity/player/player.h"

#include <string>

class Command;

class ICommandHandler {
  public:
  ICommandHandler()          = default;
  virtual ~ICommandHandler() = default;

  virtual bool registerCommand(Command* cmd)   = 0;
  virtual bool unregisterCommand(Command* cmd) = 0;

  virtual bool execute(IPlayer* caller, std::wstring command, std::wstring& out) = 0;
};

ICommandHandler& accessCommandHandler();

class Command {
  public:
  Command(const wchar_t* name, const wchar_t* help): m_commandName(name), m_helpMessage(help) { accessCommandHandler().registerCommand(this); }

  ~Command() { accessCommandHandler().unregisterCommand(this); }

  virtual bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) = 0;

  const std::wstring& getName() const { return m_commandName; }

  const std::wstring& getHelp() const { return m_helpMessage; }

  inline bool isNamesEqual(Command* cmd) const { return cmd == this || cmd->getName() == getName(); }

  private:
  const std::wstring m_commandName;
  const std::wstring m_helpMessage;
};
