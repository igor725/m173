#include "handler.h"

#include <sstream>

class Test: public Command {
  public:
  Test(): Command(L"test", L"Test message") {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    (void)caller;
    (void)args;
    out = L"Yay! Test command!!!";
    return true;
  }
};

class Killme: public Command {
  public:
  Killme(): Command(L"killme", L"Kills you") {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    caller->setHealth(0);
    return true;
  }
};

class Hurtme: public Command {
  public:
  Hurtme(): Command(L"hurtme", L"Hurts you") {}

  bool execute(IPlayer* caller, std::vector<std::wstring_view>& args, std::wstring& out) final {
    std::wstringstream ss;
    ss << args[0].data();
    int16_t hcnt;
    ss >> hcnt;

    caller->setHealth(caller->getHealth() - hcnt);
    return true;
  }
};

static Killme killme_reg;
static Hurtme hurtme_reg;
static Test   test_reg;
