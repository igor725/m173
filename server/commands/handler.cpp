#include "handler.h"

#include <cstddef>

class CommandHandler: public ICommandHandler {
  public:
  CommandHandler() {}

  bool registerCommand(Command* cmd) final {
    for (auto it = m_commands.begin(); it != m_commands.end(); ++it) {
      if ((*it)->isNamesEqual(cmd)) return false;
    }

    m_commands.push_back(cmd);
    return true;
  }

  bool unregisterCommand(Command* cmd) {
    for (auto it = m_commands.begin(); it != m_commands.end(); ++it) {
      if ((*it) == cmd) {
        m_commands.erase(it);
        return true;
      }
    }

    return false;
  }

  bool execute(IPlayer* caller, std::wstring command, std::wstring& out) final {
    if (caller != nullptr && !caller->isLocal()) {
      out = L"Only local players can call commands at the moment!";
      return true;
    }

    if (command.at(0) == L'/') command.erase(0, 1);
    auto start_of_args = command.find_first_of(L' ');
    if (start_of_args == std::wstring::npos) start_of_args = command.size();

    std::wstring_view cmdname(command.c_str(), command.c_str() + start_of_args);

    for (auto it = m_commands.begin(); it != m_commands.end(); ++it) {
      if ((*it)->getName() == cmdname) {
        if ((*it)->isPlayerOnly() && caller == nullptr) {
          out = L"\u00a7cThis command cannot be executed by the console!";
          return false;
        }
        std::vector<std::wstring_view> args;

        size_t start = 0, end = start_of_args;
        while ((start = command.find_first_not_of(L' ', end)) != std::wstring::npos) {
          end = command.find(L' ', start);
          if (end == std::wstring::npos) end = command.length();
          args.push_back(std::wstring_view(command.c_str() + start, command.c_str() + end));
        }

        return (*it)->execute(caller, args, out);
      }
    }

    out = std::format(L"\u00a7cUnknown command\u00a7f: \"{}\"!", cmdname);
    return true;
  }

  private:
  std::vector<Command*> m_commands;
};

ICommandHandler& accessCommandHandler() {
  static CommandHandler inst;
  return inst;
}
