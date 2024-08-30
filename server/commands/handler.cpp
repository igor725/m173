#include "handler.h"

#include <algorithm>
#include <cstddef>

class CommandHandler: public ICommandHandler {
  public:
  CommandHandler() {}

  bool registerCommand(Command* cmd) final {
    for (auto it = m_commands.begin(); it != m_commands.end(); ++it) {
      if ((*it)->isNamesEqual(cmd)) return false;
    }

    if (!cmd->isOperatorOnly()) ++m_nonOpCommands;
    m_commands.push_back(cmd);
    return true;
  }

  bool unregisterCommand(Command* cmd) {
    for (auto it = m_commands.begin(); it != m_commands.end(); ++it) {
      if ((*it) == cmd) {
        if (!(*it)->isOperatorOnly()) --m_nonOpCommands;
        m_commands.erase(it);
        return true;
      }
    }

    return false;
  }

  bool execute(PlayerBase* caller, std::wstring command, std::wstring& out) final {
    if (command.at(0) == L'/') command.erase(0, 1);
    auto start_of_args = command.find_first_of(L' ');
    if (start_of_args == std::wstring::npos) start_of_args = command.size();

    std::wstring_view cmdname(command.c_str(), command.c_str() + start_of_args);

    for (auto it = m_commands.begin(); it != m_commands.end(); ++it) {
      if (Helper::stricmp((*it)->getName(), cmdname)) {
        if ((*it)->isPlayerOnly() && caller == nullptr) {
          out = L"\u00a7cPlayer-only command!";
          return false;
        }
        if ((*it)->isOperatorOnly() && caller != nullptr && !caller->isOperator()) {
          out = L"\u00a7cPermission denied!";
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

  size_t cmdCountFor(PlayerBase* user) const { return user == nullptr || user->isOperator() ? m_commands.size() : m_nonOpCommands; }

  void genHelp(int32_t page, int32_t perpage, PlayerBase* user, std::wstring& out) final {
    auto pagecnt = int32_t(cmdCountFor(user)) / perpage;
    page         = std::min(pagecnt, std::max(0, page));

    int32_t pstart = page * perpage;

    out = std::format(L"Server commands help (page {}/{}):", page + 1, pagecnt + 1);
    for (auto it = m_commands.begin(); it != m_commands.end(); ++it) {
      if (!(*it)->canBeUsedBy(user)) continue;
      if (pstart-- > 0) continue;
      if (perpage-- < 1) break;
      out += std::format(L"\n  \u00a7e/{}\u00a7f - {}", (*it)->getName(), (*it)->getHelp());
    }
  }

  private:
  std::vector<Command*> m_commands;

  size_t m_nonOpCommands = 0;
};

ICommandHandler& accessCommandHandler() {
  static CommandHandler inst;
  return inst;
}
